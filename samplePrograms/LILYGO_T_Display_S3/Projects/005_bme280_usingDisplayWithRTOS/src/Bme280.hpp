// Bme280.hpp
#pragma once
// Include necessary components for C/C++ environment
#include <new>       // for error handling in dynamic memory allocation
// Include necessary libraries for Arduino and Adafruit BME280 sensor library
#include <Arduino.h>         // for Arduino framework
#include <Wire.h>            // for I2C communication
#include <SPI.h>             // for SPI communication
#include <Adafruit_Sensor.h> // for Adafruit sensor library
#include <Adafruit_BME280.h> // for Adafruit BME280 sensor library
// Include necessary header files for program constants and serial logger
#include "common.hpp"       // Include the header file for program constants
#include "serialLogger.hpp" // Include the header file for serial logger

// Class for BME280 sensor access interface
class Bme280Access
{
    public:
    //  Struct to hold sensor data
        struct SensorData {
            float temperature = -1.0f /* dummy data */; // Temperature value in degrees Celsius
            float humidity = -1.0f /* dummy data */;    // Humidity value in percentage
            float pressure = -1.0f /* dummy data */;    // Pressure value in hPa
        };

    protected:
        Adafruit_BME280 *bme280SensorInstance = nullptr; // Instance of Adafruit_BME280 for BME280 sensor handling

    public:
        // Constructor
        Bme280Access()
        {
            bme280SensorInstance = nullptr; // Initialize the BME280 sensor handle to nullptr
        }

        // Destructor
        ~Bme280Access()
        {
            // Release the BME280 sensor instance if it was created
            if (bme280SensorInstance != nullptr) {
                delete bme280SensorInstance;
                bme280SensorInstance = nullptr;
            }
        }

        // Initialize the BME280 sensor
        virtual bool initialize(){ return false; /* default dummy implementation */ } // Pure virtual function to be implemented by derived classes

        // Check if the BME280 sensor is initialized
        bool isInitialized() {
            return (bme280SensorInstance != nullptr); // Return true if the BME280 sensor is initialized, false otherwise
        }

        // Get sensor data from the BME280 sensor
        bool getSensorData(SensorData& sensorData)
        {
            // Retrieve sensor data from the BME280 sensor
            if (bme280SensorInstance == nullptr) {
                return false; // Sensor not initialized
            }

            // Acquire the mutex for BME280 access
            sensorData.temperature = bme280SensorInstance->readTemperature();
            sensorData.humidity = bme280SensorInstance->readHumidity();
            sensorData.pressure = bme280SensorInstance->readPressure();

            return true; // Return true to indicate successful retrieval of sensor data
        }
};

// Class for BME280 sensor handling via I2C communication inherits from Bme280Access
class Bme280viaI2C : public Bme280Access
{
    private:
        // Definitions of constants for I2C communication, the BME280 CSB -> 3.3V means the sensor is in I2C mode
        inline static constexpr const uint32_t I2C_CLOCK_SPEED = 400 * 1000 /* 400KHz */; // I2C clock speed in Hz
        inline static constexpr const uint8_t PIN_I2C_SDA = 1 /* GPIO01 */; // Pin number for I2C SDA
        inline static constexpr const uint8_t PIN_I2C_SCL = 2 /* GPIO02 */; // Pin number for I2C SCL
        inline static constexpr const uint8_t BME280_I2C_ADDRESS = 0x76 /* <= SDO -> GND */; // I2C address for BME280 sensor
        inline static constexpr uint32_t DELAY_TIME_I2C_IN_INITIALIZE = 200; // Delay time in milliseconds for I2C pins initialization

    public:
        // Initialize the BME280 sensor via I2C communication
        bool initialize() override
        {
            SerialLogger* serialLogger = reinterpret_cast<SerialLogger*>(Global_Variables::serialLoggerInstanceHolder); // Retrieve the serial logger instance from the global variable

            // Initialize I2C communication with specified SDA and SCL pins and the defined clock speed
            if (!(Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, I2C_CLOCK_SPEED)))
            {
                //-> I2C initialization fails

                // Log the I2C initialization failure and exit the function
                serialLogger->setSetupLastMessage(SerialLogger::ErrorMessageId::I2C_INITIALIZATION_FAILS);

                return false; // Exit initialize() if I2C initialization fails
            }
            vTaskDelay(pdMS_TO_TICKS(DELAY_TIME_I2C_IN_INITIALIZE)); // Delay for a short time to allow I2C pins to stabilize

            // Create an instance of Adafruit_BME280 for BME280 sensor handling
            bme280SensorInstance = new Adafruit_BME280();
            // Check if the instance was created successfully
            if (!bme280SensorInstance)
            {
                //-> Memory allocation fails

                // Send the error message to the serial monitor
                serialLogger->setSetupLastMessage(SerialLogger::ErrorMessageId::BME280_I2C_ACCESS_CREATION_FAILS);

                return false; // Exit initialize() if memory allocation fails
            }

            // Initialize the BME280 sensor with the specified I2C address
            if (!(bme280SensorInstance->begin(BME280_I2C_ADDRESS, &Wire))) {
                //-> BME280 access initialization fails
                
                // Release the sensor instance if initialization fails
                if (bme280SensorInstance != nullptr) {
                    delete bme280SensorInstance; // Release instance, ignore failure
                    bme280SensorInstance  = nullptr;
                }

                // Send the error message to the serial monitor
                serialLogger->setSetupLastMessage(SerialLogger::ErrorMessageId::BME280_I2C_ACCESS_INITIALIZE_FAILS);

                return false; // Exit initialize() if initialization fails
            }

            return true; // Exit initialize() after setting up the BME280 sensor
        }
};

// Class for BME280 sensor handling via SPI communication inherits from Bme280Access
class Bme280viaSPI : public Bme280Access
{
    private:
        // Definitions of constants for SPI communication
        inline static constexpr const uint32_t SPI_CLOCK_SPEED = 500 * 1000 /* 500 kHz */; // SPI clock speed for BME280 communication
        inline static constexpr const uint8_t PIN_SPI_SCK  = 10 /* GPIO10 */; // Pin number for SPI SCK
        inline static constexpr const uint8_t PIN_SPI_MOSI = 11 /* GPIO11 */; // Pin number for SPI MOSI
        inline static constexpr const uint8_t PIN_SPI_MISO = 12 /* GPIO12 */; // Pin number for SPI MISO
        inline static constexpr const uint8_t PIN_SPI_CS   = 13 /* GPIO13 */; // Chip select pin for BME280 sensor
        inline static constexpr uint32_t DELAY_TIME_SPI_PINS_STABILIZE = 200; // Delay time in milliseconds for SPI　pins initialization

    public:
        // Initialize the BME280 sensor vis SPI communication
        bool initialize() override
        {
            SerialLogger* serialLogger = reinterpret_cast<SerialLogger*>(Global_Variables::serialLoggerInstanceHolder); // Retrieve the serial logger instance from the global variable

            // Initialize SPI communication with specified pins
            if (!SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SPI_CS)) {
                //-> SPI initialization fails

                // Send the error message to the serial monitor
                serialLogger->setSetupLastMessage(SerialLogger::ErrorMessageId::SPI_INITIALIZATION_FAILS);

                return false; // Exit initialize() if SPI initialization fails
            }
            vTaskDelay(pdMS_TO_TICKS(DELAY_TIME_SPI_PINS_STABILIZE)); // Delay for a short time to allow pins to stabilize

            // SPI communication setup
            SPI.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE0)); // Begin SPI transaction with specified settings
            digitalWrite(PIN_SPI_CS, HIGH); // Set chip select pin to HIGH (inactive state)
            SPI.transfer(0x00); // Send a dummy byte to ensure proper SPI communication
            digitalWrite(PIN_SPI_CS, LOW); // Set chip select pin to LOW (active state)
            SPI.endTransaction(); // End SPI transaction

            // Initialize the BME280 sensor with the specified SPI settings
            bme280SensorInstance = new Adafruit_BME280(PIN_SPI_CS, &SPI);
            // Check if the instance was created successfully
            if (!bme280SensorInstance)
            {
                //-> Memory allocation fails

                // Send the error message to the serial monitor
                serialLogger->setSetupLastMessage(SerialLogger::ErrorMessageId::BME280_SPI_ACCESS_CREATION_FAILS);

                return false; // Exit initialize() if memory allocation fails
            }

            // Initialize the BME280 sensor with the specified SPI settings
            if (!(bme280SensorInstance->begin())) {
                //-> BME280 initialization fails

                // Release the sensor instance if initialization fails
                if (bme280SensorInstance != nullptr) {
                    delete bme280SensorInstance; // Release instance, ignore failure
                    bme280SensorInstance  = nullptr;
                }

                // Send the error message to the serial monitor
                serialLogger->setSetupLastMessage(SerialLogger::ErrorMessageId::BME280_SPI_ACCESS_INITIALIZE_FAILS);
                
                return false; // Exit initialize() if BME280 initialization fails
            }
            
            return true; // Exit initialize() after setting up the BME280 sensor
        }
};

// Class for BME280 sensor handling via I2C or SPI communication with mutex for thread safety
class Bme280 {
    private:
        // Bme280Access instance for sensor access
        Bme280Access *bme280AccessInstance = nullptr;
        // Definitions of mutex for thread safety
        inline static SemaphoreHandle_t bme280Mutex; // Mutex for BME280 access to ensure thread safety

    public:
        // Constructor
        Bme280()
        {
            bme280AccessInstance = nullptr; // Initialize bme280AccessInstance to nullptr
            bme280Mutex = nullptr; // Initialize bme280Mutex to nullptr
        };
    
        // Destructor
        ~Bme280() {
            //  Release the BME280 access instance if it was created
            if (bme280Mutex != nullptr) {
                vSemaphoreDelete(bme280Mutex);
                bme280Mutex = nullptr;
            }
        }

        // Initialize the BME280 sensor
        bool initialize(Bme280Access *bme280Access)
        {
            namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
            SerialLogger* serialLogger = reinterpret_cast<SerialLogger*>(Global_Variables::serialLoggerInstanceHolder); // Retrieve the serial logger instance from the global variable

            // Check if bme280Access is nullptr
            if (bme280Access == nullptr) {
                //-> bme280Access is nullptr

                // Send the error message to the serial monitor
                serialLogger->setSetupLastMessage(SerialLogger::ErrorMessageId::BME280_ACCESS_INITIALIZE_FAILS);

                bme280AccessInstance = nullptr; // Reset the bme280AccessInstance to nullptr

                return false; // Exit initialize() if bme280Access is nullptr
            }

            // Create a mutex for BME280 access
            bme280Mutex = xSemaphoreCreateMutex();
            if (bme280Mutex == nullptr)
            {
                //-> Mutex creation fails

                // Send the error message to the serial monitor
                serialLogger->setSetupLastMessage(SerialLogger::ErrorMessageId::BME280_MUTEX_CREATION_FAILS);

                bme280AccessInstance = nullptr; // Reset the bme280AccessInstance to nullptr
                
                return false; // Exit initialize() if mutex creation fails
            }

            // Wait for the mutex to be available
            if (xSemaphoreTake(bme280Mutex, pdMS_TO_TICKS(delayTimes::BME280_MUTEX)) != pdTRUE)
            {
                //-> Mutex acquisition fails

                // Send the error message to the serial monitor
                vTaskDelay(pdMS_TO_TICKS(delayTimes::BME280_MUTEX_RETRY)); // Delay for a short time before retrying
            }

            // Initialize the BME280 sensor access handle
            bool bme280AccessInitializedFlag = bme280Access->initialize();
            if (!bme280Access->isInitialized()) {
                //-> BME280 access initialization fails

                // Release the mutex before returning
                xSemaphoreGive(bme280Mutex);
                vSemaphoreDelete(bme280Mutex); // Release the mutex
                bme280Mutex = nullptr; // Reset the pointer to nullptr

                // Release the sensor instance
                delete bme280Access;
                bme280Access = nullptr;

                bme280AccessInstance = nullptr; // Reset the bme280AccessInstance to nullptr
    
                // Send the error message to the serial monitor
                serialLogger->setSetupLastMessage(SerialLogger::ErrorMessageId::BME280_ACCESS_INITIALIZE_FAILS);
                
                return false; // Exit initialize() if BME280 access initialization fails
            }

            // Release the mutex for BME280 access
            xSemaphoreGive(bme280Mutex);

            bme280AccessInstance = bme280Access; // Set the bme280AccessInstance to the provided bme280Access

            return true; // Exit initialize() after setting up the BME280 sensor
        };

        bool isInitialized(){
            return (bme280Mutex != nullptr && bme280AccessInstance != nullptr); // Check if bme280Mutex and bme280AccessInstance are not nullptr
        };

        // update the state of the BME280 sensor based on the ledStateFlag
        bool updateState(){
            namespace globalVars = Global_Variables; // Use the namespace for global variables
            namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
            SerialLogger* serialLogger = reinterpret_cast<SerialLogger*>(globalVars::serialLoggerInstanceHolder); // Retrieve the serial logger instance from the global variable

            // Check if the BME280 access is initialized
            if (!isInitialized()) {
                //-> BME280 access is not initialized

                // Send the error message to the serial monitor
                serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::BME280_ACCESS_INITIALIZE_FAILS);

                return false; // Exit updateState() if BME280 access is not initialized
            }

            // Acquire the mutex for BME280 access
            if (xSemaphoreTake(bme280Mutex, pdMS_TO_TICKS(delayTimes::BME280_MUTEX)) != pdTRUE)
            {
                //-> Mutex acquisition fails

                vTaskDelay(pdMS_TO_TICKS(delayTimes::BME280_MUTEX_RETRY)); // Delay for a short time before retrying
            }

            // Read sensor values from BME280
            Bme280Access::SensorData sensorData;
            bme280AccessInstance->getSensorData(sensorData);
            float temperature = sensorData.temperature;
            float humidity    = sensorData.humidity;
            float pressure    = sensorData.pressure / 100.0F;

            // Release the mutex for BME280 access
            xSemaphoreGive(bme280Mutex);

            if (globalVars::sensorVarsMutex == nullptr)
            {
                //-> Mutex retrieval fails

                // Send the error message to the serial monitor
                serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::SENSOR_VARS_MUTEX_CREATION_FAILS);

                return false; // Exit updateState() if mutex retrieval fails
            }

            // Acquire the mutex for sensor vars access
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