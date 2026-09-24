// Bme280.hpp
#pragma once

#include <Arduino.h> // for Arduino framework
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "common.hpp" // Include the header file for program constants
#include "serialLogger.hpp" // Include the header file for serial logger

// Class for BME280 sensor handling via SPI communication
class Bme280viaSPI {
    private:
        // Definitions of constants for BME280 handling
        inline static constexpr const uint32_t SPI_CLOCK_SPEED = 500000; // SPI clock speed (500 kHz) for BME280 communication
        inline static constexpr const uint8_t PIN_SPI_SCK  = 10; // Pin number for SPI SCK
        inline static constexpr const uint8_t PIN_SPI_MOSI = 11; // Pin number for SPI MOSI
        inline static constexpr const uint8_t PIN_SPI_MISO = 12; // Pin number for SPI MISO
        inline static constexpr const uint8_t PIN_SPI_CS   = 13; // Chip select pin for BME280 sensor
        inline static constexpr uint32_t DELAY_TIME_SPI_PINS_STABILIZE = 200; // Delay time in milliseconds for pin initialization

        // Instance of Adafruit_BME280 for BME280 sensor handling
        inline static Adafruit_BME280 *bme280Sensor = nullptr; // Instance of Adafruit_BME280 for BME280 sensor handling
        // Definitions of mutex for thread safety
        inline static SemaphoreHandle_t bme280Mutex; // Mutex for BME280 access to ensure thread safety

    public:

        // Constructor
        Bme280viaSPI()
        {
            bme280Mutex = nullptr; // Initialize bme280Mutex to nullptr
        };
    
        // Destructor
        ~Bme280viaSPI() {
            if (bme280Mutex != nullptr) {
                vSemaphoreDelete(bme280Mutex);
                bme280Mutex = nullptr;
            }
        }

        // Initialize the BME280 sensor
        bool initialize(){
            namespace delayTimes = Delay_Times; // Use the namespace for delay times

            // Create a mutex for BME280 access
            bme280Mutex = xSemaphoreCreateMutex();
            if (bme280Mutex == nullptr)
            {
                //-> Mutex creation fails
                // Send the error message to the serial monitor
                SerialLogger* serialLogger = reinterpret_cast<SerialLogger*>(Global_Variables::serialLoggerInstanceHolder); // Retrieve the serial logger instance from the global variable
                serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::BME280_MUTEX_CREATION_FAILS);
                return false; // Exit initialize() if mutex creation fails
            }

            if (xSemaphoreTake(bme280Mutex, pdMS_TO_TICKS(delayTimes::BME280_MUTEX)) != pdTRUE)
            {
                //-> Mutex acquisition fails

                // Send the error message to the serial monitor
                vTaskDelay(pdMS_TO_TICKS(delayTimes::BME280_MUTEX_RETRY)); // Delay for a short time before retrying
            }

            // Initialize SPI communication with specified pins
            SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SPI_CS);
            vTaskDelay(pdMS_TO_TICKS(DELAY_TIME_SPI_PINS_STABILIZE)); // Delay for a short time to allow pins to stabilize
            SPI.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE0)); // Begin SPI transaction with specified settings
            digitalWrite(PIN_SPI_CS, HIGH); // Set chip select pin to HIGH (inactive state)
            SPI.transfer(0x00); // Send a dummy byte to ensure proper SPI communication
            digitalWrite(PIN_SPI_CS, LOW); // Set chip select pin to LOW (active state)
            SPI.endTransaction(); // End SPI transaction

            // Initialize the BME280 sensor with the specified SPI settings
            bme280Sensor = new Adafruit_BME280(PIN_SPI_CS, &SPI);
            if (!(bme280Sensor && bme280Sensor->begin())) {
                //-> BME280 initialization fails

                if (bme280Sensor != nullptr) {
                    delete bme280Sensor; // Release instance, ignore failure
                    bme280Sensor = nullptr;
                }

                // Release the mutex before returning
                xSemaphoreGive(bme280Mutex);
                vSemaphoreDelete(bme280Mutex); // Release the mutex
                bme280Mutex = nullptr; // Reset the pointer to nullptr

                // Send the error message to the serial monitor
                SerialLogger* serialLogger = reinterpret_cast<SerialLogger*>(Global_Variables::serialLoggerInstanceHolder); // Retrieve the serial logger instance from the global variable
                serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::BME280_INITIALIZATION_FAILS);
                
                return false; // Exit initialize() if BME280 initialization fails
            }

            // Release the mutex
            xSemaphoreGive(bme280Mutex);

            return true; // Exit initialize() after setting up the BME280 sensor
        };

        bool isInitialized(){
            return (bme280Mutex != nullptr); // Check if bme280Mutex is not nullptr
        };

        // update the state of the BME280 sensor based on the ledStateFlag
        bool updateState(){
            namespace globalVars = Global_Variables; // Use the namespace for global variables
            namespace delayTimes = Delay_Times; // Use the namespace for delay times

            // Check if the mutex for BME280 access is created
            if (bme280Mutex == nullptr)
            {
                //-> Mutex retrieval fails

                // Send the error message to the serial monitor
                SerialLogger* serialLogger = reinterpret_cast<SerialLogger*>(globalVars::serialLoggerInstanceHolder); // Retrieve the serial logger instance from the global variable
                serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::BME280_MUTEX_CREATION_FAILS);

                return false; // Exit updateState() if mutex retrieval fails
            }

            // Acquire the mutex for BME280 access
            if (xSemaphoreTake(bme280Mutex, pdMS_TO_TICKS(delayTimes::BME280_MUTEX)) != pdTRUE)
            {
                //-> Mutex acquisition fails

                // Send the error message to the serial monitor
                vTaskDelay(pdMS_TO_TICKS(delayTimes::BME280_MUTEX_RETRY)); // Delay for a short time before retrying
            }

            // Read sensor values from BME280
            float temperature = bme280Sensor->readTemperature();
            float humidity    = bme280Sensor->readHumidity();
            float pressure    = bme280Sensor->readPressure() / 100.0F;

            // Release the mutex for BME280 access
            xSemaphoreGive(bme280Mutex);

            // Retrieve the mutex for sensor vars access
            if (xSemaphoreTake(globalVars::sensorVarsMutex, pdMS_TO_TICKS(delayTimes::SENSOR_VARS_MUTEX)) != pdTRUE)
            {
                //-> Mutex acquisition fails

                // Send the error message to the serial monitor
                vTaskDelay(pdMS_TO_TICKS(delayTimes::SENSOR_VARS_MUTEX_RETRY)); // Delay for a short time before retrying
            }

            // Retrieve the ledStateFlag from the global variable
            globalVars::sensorTemperature = temperature; // Retrieve the sensorTemperature from the global variable
            globalVars::sensorHumidity = humidity; // Retrieve the sensorHumidity from the global variable
            globalVars::sensorPressure = pressure; // Retrieve the sensorPressure from the global variable

            // Release the mutex for sensor vars access
            xSemaphoreGive(globalVars::sensorVarsMutex);

            return true; // Exit updateState() after updating the external LED state
        };
};