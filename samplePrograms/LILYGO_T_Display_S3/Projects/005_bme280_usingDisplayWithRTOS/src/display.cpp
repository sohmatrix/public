// display.cpp

// Include necessary components for C/C++ environment
#include <new>         // for error handling
#include <string>      // using STL string class for easier string manipulation
// Include necessary libraries for Arduino and Arduino_GFX_Library
#include <Arduino.h>   // for Arduino framework
// Include necessary header files for program constants, serial logger, and display class
#include "common.hpp"       // Include the header file for program constants
#include "serialLogger.hpp" // Include the header file for serial logger
#include "display.hpp"      // Include the header file for display class
#include "TDisplayS3.hpp"   // Include the header file for T-Display S3 constants

// methods for display class

void Display::clearInstances()
{
    // Delete the display instance if it was created
    if (displayHandleHolder != nullptr) {
        GFXLibraryWrapper::deleteDisplayHandle((GFXLibraryWrapper::DisplayHandle_t*) displayHandleHolder); // Release instance, ignore failure
        displayHandleHolder = nullptr; // Reset the pointer to nullptr
    }

    // Delete the bus instance if it was created
    if (busHandleHolder != nullptr) {
        GFXLibraryWrapper::deleteBusHandle((GFXLibraryWrapper::BusHandle_t*) busHandleHolder); // Release instance, ignore failure
        busHandleHolder = nullptr; // Reset the pointer to nullptr
    }

    // Delete the mutex for display access if it was created
    if (displayMutex != nullptr) {
        vSemaphoreDelete(displayMutex); // Delete the mutex if it was created
        displayMutex = nullptr; // Reset the mutex pointer to nullptr
    }

    return; // Return from the function
}

// Constructor
Display::Display()
{
    // Initialize instance handlers to nullptr
    busHandleHolder = nullptr;
    displayHandleHolder = nullptr;
}

// Destructor
Display::~Display() {
    // Clear the display and bus instances to avoid memory leak
    clearInstances();
}

// Initialize the display
bool Display::initialize()
{
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    namespace globalVars = Global_Variables; // Use the namespace for global variables

    // Retrieve the serial logger instance from the global variable
    SerialLogger* serialLogger = reinterpret_cast<SerialLogger*>(globalVars::serialLoggerInstanceHolder);

    clearInstances(); // Clear the display and bus instances to avoid memory leak 
    
    // Create a mutex for display access
    displayMutex = xSemaphoreCreateMutex();
    if (displayMutex == nullptr)
    {
        //-> Mutex creation fails

        // Send the error message to the serial monitor
        serialLogger->setSetupLastMessage(SerialLogger::ErrorMessageId::DISPLAY_MUTEX_CREATION_FAILS);

        return false; // Return false to indicate initialization failure
    }

    GFXLibraryWrapper::BusHandle_t* busHandle = nullptr; // Initialize bus handle to nullptr
    GFXLibraryWrapper::DisplayHandle_t* displayHandle = nullptr; // Initialize display handle to nullptr

    // Wait for the mutex to be available
    while (xSemaphoreTake(displayMutex, pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX)) != pdTRUE)
    {
        //-> Mutex acquisition fails

        vTaskDelay(pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX_RETRY)); // Delay for a short time before retrying
    }

    GFXLibraryWrapper::powerUp();

    // Create a data bus instance and display instance of Arduino_GFX_Library
    busHandle = GFXLibraryWrapper::createBusHandle();
    if (!busHandle)
    {
        //-> Bus handle creation fails

        // Send the error message to the serial monitor
        serialLogger->setSetupLastMessage(SerialLogger::ErrorMessageId::DISPLAY_BUS_CREATION_FAILS);

        return false; // Return false to indicate initialization failure
    }

    // Create a display instance of Arduino_GFX_Library
    displayHandle = GFXLibraryWrapper::createDisplayHandle(busHandle);
    if (!displayHandle)
    {
        //-> Display handle creation fails

        // Send the error message to the serial monitor
        serialLogger->setSetupLastMessage(SerialLogger::ErrorMessageId::DISPLAY_DISPLAY_CREATION_FAILS);

        // Clean up the bus instance to avoid memory leak
        clearInstances();

        return false; // Return false to indicate initialization failure
    }

    // Initialize the display
    if (!displayHandle->begin())
    {
        //-> Display initialization fails

        // Send the error message to the serial monitor
        serialLogger->setSetupLastMessage(SerialLogger::ErrorMessageId::DISPLAY_BEGIN_FAILS);

        // Clean up the display and bus instances to avoid memory leak
        clearInstances();

        return false; // Return false to indicate initialization failure
    }

    // Release the mutex
    xSemaphoreGive(displayMutex);

    vTaskDelay(pdMS_TO_TICKS(delayTimes::DISPLAY_INITIALIZE)); // wait for a short time before returning

    //　Store the bus and display instances in global variables for later use
    busHandleHolder = (void*) busHandle; // Store the bus instance in global variables
    displayHandleHolder = (void*) displayHandle; // Store the display instance in global variables

    // Clear the display with black color
    displayHandle->fillScreen(GFXLibraryWrapper::COLOR_BLACK);

    return true; // Return true to indicate successful initialization
}

// Check if the display is initialized
bool Display::isInitialized()
{
    return (displayMutex && busHandleHolder && displayHandleHolder); // Check if displayMutex, busHandleHolder and displayHandleHolder are not nullptr
}

// Display a message with sensor values on the screen
bool Display::displayMessageWithSensorValues()
{
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    namespace globalVars = Global_Variables; // Use the namespace for global variables

    SerialLogger* serialLogger = reinterpret_cast<SerialLogger*>(globalVars::serialLoggerInstanceHolder); // Retrieve the serial logger instance from the global variable

    // Check if the display is initialized
    if (!isInitialized())
    {
        //-> Display is not initialized

        // Send the error message to the serial monitor
        serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::DISPLAY_INITIALIZATION_FAILS);

        return false; // Exit the function if the display is not initialized
    }

    if (globalVars::sensorVarsMutex == nullptr)
    {
        //-> Mutex retrieval fails

        // Send the error message to the serial monitor
        serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::SENSOR_VARS_MUTEX_CREATION_FAILS);

        return false; // Exit the function if mutex retrieval fails
    }

    // Wait for the mutex to be available
    while (xSemaphoreTake(globalVars::sensorVarsMutex, pdMS_TO_TICKS(delayTimes::SENSOR_VARS_MUTEX)) != pdTRUE)
    {
        //-> Mutex acquisition fails
        vTaskDelay(pdMS_TO_TICKS(delayTimes::SENSOR_VARS_MUTEX_RETRY)); // Delay for a short time before retrying
    }

    // Retrieve the sensor values from the global variables
    float sensorTemperature = globalVars::sensorTemperature;
    float sensorHumidity = globalVars::sensorHumidity;
    float sensorPressure = globalVars::sensorPressure;

    // Release the mutex
    xSemaphoreGive(globalVars::sensorVarsMutex);

    // Create a message
    std::string message = commons::messageHeader; // Memory usage increases dynamically at runtime (in heap memory)
    message += "Temperature:=" + std::to_string(sensorTemperature) + " C, ";
    message += "Humidity:=" + std::to_string(sensorHumidity) + " %, ";
    message += "Pressure:=" + std::to_string(sensorPressure) + " hPa, ";

    // Retrieve the display instance from the global variable
    GFXLibraryWrapper::DisplayHandle_t* displayHandle = (GFXLibraryWrapper::DisplayHandle_t*) displayHandleHolder;

    // Wait for the mutex to be available
    while (xSemaphoreTake(displayMutex, pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX)) != pdTRUE)
    {
        //-> Mutex acquisition fails
        vTaskDelay(pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX_RETRY)); // Delay for a short time before retrying
    }

    // Clear the display with black color
    displayHandle->fillScreen(GFXLibraryWrapper::COLOR_BLACK);
    // Display the message on the screen  with the following steps:
    uint16_t textColor = random(GFXLibraryWrapper::COLOR_BLACK, GFXLibraryWrapper::COLOR_WHITE); // Random color for text
    displayHandle->setTextColor(textColor);
    // - Set the text size
    displayHandle->setTextSize(MESSAGE_DEFAULT_TEXT_SIZE);
    // - Set the cursor position
    GFXLibraryWrapper::Position_t MessageCursor_y = (GFXLibraryWrapper::DISPLAY_HEIGHT / 2) - (GFXLibraryWrapper::FONT_SIZE_HEIGHT * MESSAGE_DEFAULT_TEXT_SIZE / 2); // center the text vertically
    GFXLibraryWrapper::Position_t MessageCursor_x = GFXLibraryWrapper::FONT_SIZE_WIDTH * MESSAGE_DEFAULT_TEXT_SIZE / 2; // half of the text size to the left of the screen
    displayHandle->setCursor(MessageCursor_x, MessageCursor_y);

    // - Display the message on the screen
    displayHandle->print(message.c_str());

    // Release the mutex for display access
    xSemaphoreGive(displayMutex);

    return true;

}
