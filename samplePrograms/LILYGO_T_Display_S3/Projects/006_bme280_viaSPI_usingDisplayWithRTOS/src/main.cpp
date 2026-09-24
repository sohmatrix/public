// This code is sample program for the T-Display S3 and Arduino_GFX_Library and BME280(via SPI) using RTOS

// Include necessary components for C/C++ environment
#include <new>         // for error handling
#include <string>      // using STL string class for easier string manipulation
// Include necessary libraries for Arduino and Arduino_GFX_Library
#include <Arduino.h>   // for Arduino framework

// Global variables and constants
#include "common.hpp"       // Include the header file for program constants
#include "serialLogger.hpp" // Include the header file for serial logger
#include "display.hpp"      // Include the header file for display handling
#include "Bme280viaSPI.hpp"  // Include the header file for external LED handling

// Structure to hold parameters for the display task
struct DisplayTaskParameters
{
    Display* displayInstance; // Pointer to the display instance
    const char* taskName; // Name of the display task
};

struct Bme280TaskParameters
{
    Bme280viaSPI* bme280Instance; // Pointer to the BME280 instance
    const char* taskName; // Name of the BME280 task
};

// Display task function
void displayTask(void *pvParameters)
{
    namespace globalVars = Global_Variables; // Use the namespace for global variables
    namespace delayTimes = Delay_Times; // Use the namespace for delay times

    // Cast the parameter to Display pointer
    DisplayTaskParameters* taskParameters = reinterpret_cast<DisplayTaskParameters*>(pvParameters); // Cast the parameter to DisplayTaskParameters pointer
    Display* displayInstance = taskParameters->displayInstance; // Extract the display instance from the task parameters
    const char* taskName = taskParameters->taskName; // Extract the task name from the task parameters

    // Main loop for display task
    while (true)
    {
        // Display "Hello World" message with counter on the screen
        bool displaySuccessFlag = displayInstance->displayHelloWorldWithCounter();
        if (!displaySuccessFlag)
        {
            //-> Display output fails

            // Retrieve the serial logger instance from the global variable
            SerialLogger* serialLogger = reinterpret_cast<SerialLogger*>(globalVars::serialLoggerInstanceHolder);

            // Send the error message to the serial monitor
            serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::DISPLAY_INITIALIZATION_FAILS);
        }

        vTaskDelay(pdMS_TO_TICKS(delayTimes::DISPLAY_LOOP)); // Wait for the specified time before the next loop iteration
    }
}

// External LED task function
void bme280Task(void *pvParameters)
{
    namespace globalVars = Global_Variables; // Use the namespace for global variables
    namespace delayTimes = Delay_Times; // Use the namespace for delay times

    // Cast the parameter to Bme280 pointer
    Bme280TaskParameters* taskParameters = reinterpret_cast<Bme280TaskParameters*>(pvParameters); // Cast the parameter to Bme280TaskParameters pointer
    Bme280viaSPI* bme280Instance = taskParameters->bme280Instance; // Extract the BME280 instance from the task parameters
    const char* taskName = taskParameters->taskName; // Extract the task name from the task parameters

    // Main loop for external LED task
    while (true)
    {
        // Update the state of the external LED based on the ledStateFlag
        bool updateSuccessFlag = bme280Instance->updateState();
        if (!updateSuccessFlag)
        {
            //-> BME280 update fails

            // Retrieve the serial logger instance from the global variable
            SerialLogger* serialLogger = reinterpret_cast<SerialLogger*>(globalVars::serialLoggerInstanceHolder);

            // Send the error message to the serial monitor
            serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::BME280_INITIALIZATION_FAILS);
        }
        vTaskDelay(pdMS_TO_TICKS(delayTimes::BME280_LOOP)); // Wait for the specified time before the next loop iteration
    }
}

// Clear global instances and handles to avoid memory leaks
void cleanupGlobalInstanceAndHandles()
{
    namespace globalVars = Global_Variables; // Use the namespace for global variables

    // Clear display task handle
    if (globalVars::displayTaskHandleHolder != nullptr)
    {
        TaskHandle_t displayTaskHandle = reinterpret_cast<TaskHandle_t>(globalVars::displayTaskHandleHolder);
        vTaskDelete(displayTaskHandle); // Delete the display task
        displayTaskHandle = nullptr;
        globalVars::displayTaskHandleHolder = nullptr; // Reset the pointer to nullptr
    }

    // Clear BME280 task handle
    if (globalVars::bme280TaskHandleHolder != nullptr)
    {
        TaskHandle_t bme280TaskHandle = reinterpret_cast<TaskHandle_t>(globalVars::bme280TaskHandleHolder);
        vTaskDelete(bme280TaskHandle); // Delete the BME280 task
        bme280TaskHandle = nullptr;
        globalVars::bme280TaskHandleHolder = nullptr; // Reset the pointer to nullptr
    }

    // Clear display instance
    if (globalVars::displayInstanceHolder != nullptr)
    {
        Display* displayInstance = reinterpret_cast<Display*>(globalVars::displayInstanceHolder);
        delete displayInstance; // Release instance, ignore failure
        displayInstance = nullptr;
        globalVars::displayInstanceHolder = nullptr; // Reset the pointer to nullptr
    }

    // Clear BME280 instance
    if (globalVars::bme280InstanceHolder != nullptr)
    {
        Bme280viaSPI* bme280Instance = reinterpret_cast<Bme280viaSPI*>(globalVars::bme280InstanceHolder);
        delete bme280Instance; // Release instance, ignore failure
        bme280Instance = nullptr;
        globalVars::bme280InstanceHolder = nullptr; // Reset the pointer to nullptr
    }

    // Clear sensor vars mutex
    if (globalVars::sensorVarsMutex != nullptr)
    {
        vSemaphoreDelete(globalVars::sensorVarsMutex); // Delete the mutex
        globalVars::sensorVarsMutex = nullptr; // Reset the pointer to nullptr
    }

    // Clear serial logger instance
    if (globalVars::serialLoggerInstanceHolder != nullptr)
    {
        SerialLogger* serialLogger = reinterpret_cast<SerialLogger*>(globalVars::serialLoggerInstanceHolder);
        delete serialLogger; // Release instance, ignore failure
        serialLogger = nullptr;
        globalVars::serialLoggerInstanceHolder = nullptr; // Reset the pointer to nullptr
    }
}

// Setup function for Arduino
void setup()
{
    namespace globalVars = Global_Variables; // Use the namespace for global variables
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace delayTimes = Delay_Times; // Use the namespace for delay times

    // Initialize serial logger instance
    SerialLogger* serialLogger = new SerialLogger(); 
    serialLogger->initialize();
    // Store the serial logger instance in the global variable for later use
    globalVars::serialLoggerInstanceHolder = (void*) serialLogger;

    // Create a mutex for sensor vars access
    SemaphoreHandle_t sensorVarsMutex = xSemaphoreCreateMutex();
    if (sensorVarsMutex == nullptr)
    {
        //-> Mutex creation fails

        // Send the error message to the serial monitor
        serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::SENSOR_VARS_MUTEX_CREATION_FAILS);

        return; // Exit setup() if mutex creation fails
    }
    // Store the sensor vars mutex in the global variable for later use
    globalVars::sensorVarsMutex = sensorVarsMutex;

    // Wait for the mutex to be available
    while (xSemaphoreTake(globalVars::sensorVarsMutex, pdMS_TO_TICKS(delayTimes::SENSOR_VARS_MUTEX)) != pdTRUE)
    {
        //-> Mutex acquisition fails

        vTaskDelay(pdMS_TO_TICKS(delayTimes::SENSOR_VARS_MUTEX_RETRY)); // Delay for a short time before retrying
    }

    // Release the mutex
    xSemaphoreGive(globalVars::sensorVarsMutex);

    // Initialize BME280 instance
    Bme280viaSPI* bme280Instance = new Bme280viaSPI();
    bool bme280InitializeSuccessFlag = bme280Instance->initialize();
    if (!(bme280Instance && bme280Instance->isInitialized()))
    {
        //-> BME280 initialization fails

        cleanupGlobalInstanceAndHandles(); // Clear global instances and handles to avoid memory leaks

        // Send the error message to the serial monitor
        serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::BME280_INITIALIZATION_FAILS);

        return; // Exit setup() if BME280 initialization fails
    }
    // Store the BME280 instance in the global variable for later use
    globalVars::bme280InstanceHolder = (void*) bme280Instance;

    // Initialize display instance
    Display* displayInstance = new Display();
    bool displayInitializeSuccessFlag = displayInstance->initialize();
    if (!(displayInstance && displayInstance->isInitialized()))
    {
        //-> Display initialization fails

        cleanupGlobalInstanceAndHandles(); // Clear global instances and handles to avoid memory leaks

        // Send the error message to the serial monitor
        serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::DISPLAY_INITIALIZATION_FAILS);

        return; // Exit setup() if display initialization fails
    }
    // Store the display instance in the global variable for later use
    globalVars::displayInstanceHolder = (void*) displayInstance;

    // Create a task for display handling
    DisplayTaskParameters *displayTaskParameters = new DisplayTaskParameters{displayInstance, commons::DISPLAY_TASK_NAME};
    TaskHandle_t displayTaskHandle = nullptr; // Task handle for the display task
    BaseType_t taskCreationResult = xTaskCreate(
        displayTask /* task function */, commons::DISPLAY_TASK_NAME, commons::DISPLAY_TASK_STACK_SIZE,
        (void*) displayTaskParameters /* Task input parameter */, commons::DISPLAY_TASK_PRIORITY,
        (TaskHandle_t*) &(displayTaskHandle) /* Task handle */);
    // Check if the task creation was successful
    if (taskCreationResult != pdPASS)   
    {
        //-> Display task creation fails

        cleanupGlobalInstanceAndHandles(); // Clear global instances and handles to avoid memory leaks

        // Send the error message to the serial monitor
        serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::DISPLAY_TASK_CREATION_FAILS);

        return; // Exit setup() if display task creation fails
    }
    // Store the display task handle in the global variable for later use
    globalVars::displayTaskHandleHolder = (void*) displayTaskHandle;

    // Create a task for BM280 handling
    Bme280TaskParameters *bme280TaskParameters = new Bme280TaskParameters{bme280Instance, commons::BME280_TASK_NAME};
    TaskHandle_t bme280TaskHandle = nullptr; // Task handle for the BME280 task
    taskCreationResult = xTaskCreate(
        bme280Task /* task function */, commons::BME280_TASK_NAME, commons::BME280_TASK_STACK_SIZE,
        (void*) bme280TaskParameters /* Task input parameter */, commons::BME280_TASK_PRIORITY,
        (TaskHandle_t*) &(bme280TaskHandle) /* Task handle */);
    // Check if the task creation was successful
    if (taskCreationResult != pdPASS)
    {
        //-> BME280 task creation fails

        cleanupGlobalInstanceAndHandles(); // Clear global instances and handles to avoid memory leaks

        // Send the error message to the serial monitor
        serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::BME280_TASK_CREATION_FAILS);

        return; // Exit setup() if BME280 task creation fails
    }
    // Store the BME280 task handle in the global variable for later use
    globalVars::bme280TaskHandleHolder = (void*) bme280TaskHandle;

    // No wait for a noticeable time before returning from setup()
}

// Main loop function for Arduino ... without finish condition, this will run forever
void loop()
{
    namespace globalVars = Global_Variables; // Use the namespace for global variables
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace delayTimes = Delay_Times; // Use the namespace for delay times

    // Retrieve the instances from the global variable
    SerialLogger* serialLogger = reinterpret_cast<SerialLogger*>(globalVars::serialLoggerInstanceHolder); // Retrieve the serial logger instance from the global variable
    SemaphoreHandle_t sensorVarsMutex = globalVars::sensorVarsMutex; // Retrieve the BME280 mutex from the global variable

    if (sensorVarsMutex == nullptr)
    {
        //-> Mutex retrieval fails

        // Send the error message to the serial monitor
        serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::BME280_MUTEX_CREATION_FAILS);

        return; // Exit loop() if mutex retrieval fails
    }

    // Wait for the mutex to be available
    while (xSemaphoreTake(globalVars::sensorVarsMutex, pdMS_TO_TICKS(delayTimes::BME280_MUTEX)) != pdTRUE)
    {
        //-> Mutex acquisition fails

        vTaskDelay(pdMS_TO_TICKS(delayTimes::BME280_MUTEX_RETRY)); // Delay for a short time before retrying
    }

    float sensorTemperature = globalVars::sensorTemperature; // Retrieve the sensorTemperature from the global variable
    float sensorHumidity = globalVars::sensorHumidity; // Retrieve the sensorHumidity from the global variable
    float sensorPressure = globalVars::sensorPressure; // Retrieve the sensorPressure from the global variable

    // Release the mutex
    xSemaphoreGive(globalVars::sensorVarsMutex);

    delay(delayTimes::LOOP); // Wait for the specified time before the next loop iteration
}
