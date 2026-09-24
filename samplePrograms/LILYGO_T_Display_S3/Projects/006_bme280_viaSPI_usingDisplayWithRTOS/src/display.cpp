// display.cpp

// Include necessary components for C/C++ environment
#include <new>         // for error handling
#include <string>      // using STL string class for easier string manipulation
// Include necessary libraries for Arduino and Arduino_GFX_Library
#include <Arduino.h>   // for Arduino framework

#include "common.hpp"       // Include the header file for program constants
#include "serialLogger.hpp" // Include the header file for serial logger
#include "display.hpp"      // Include the header file for display class
#include "TDisplayS3.hpp"   // Include the header file for T-Display S3 constants

// methods for display class

// Constructor
Display::Display()
{
    // Initialize instance handlers to nullptr
    busHandleHolder = nullptr;
    displayHandleHolder = nullptr;
}

// Destructor
Display::~Display() {
    // Delete the display instance if it was created
    if (displayHandleHolder != nullptr) {
        GFXLibraryWrapper::deleteDisplayHandle((DisplayHandle_t*) displayHandleHolder); // Release instance, ignore failure
        displayHandleHolder = nullptr; // Reset the pointer to nullptr
    }

    // Delete the bus instance if it was created
    if (busHandleHolder != nullptr) {
        GFXLibraryWrapper::deleteBusHandle((BusHandle_t*) busHandleHolder); // Release instance, ignore failure
        busHandleHolder = nullptr; // Reset the pointer to nullptr
    }

    // Delete the mutex for display access if it was created
    if (displayMutex != nullptr) {
        vSemaphoreDelete(displayMutex); // Delete the mutex if it was created
        displayMutex = nullptr; // Reset the mutex pointer to nullptr
    }
}

// Initialize the display
bool Display::initialize()
{
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace delayTimes = Delay_Times; // Use the namespace for delay times
    namespace globalVars = Global_Variables; // Use the namespace for global variables

    // Retrieve the serial logger instance from the global variable
    SerialLogger* serialLogger = reinterpret_cast<SerialLogger*>(globalVars::serialLoggerInstanceHolder);
    
    // Create a mutex for display access
    displayMutex = xSemaphoreCreateMutex();
    if (displayMutex == nullptr)
    {
       //-> Mutex creation fails

        // Send the error message to the serial monitor
        serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::DISPLAY_MUTEX_CREATION_FAILS);
        return false;
    }

    // Wait for the mutex to be available
    while (xSemaphoreTake(displayMutex, pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX)) != pdTRUE)
    {
        //-> Mutex acquisition fails
        vTaskDelay(pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX_RETRY)); // Delay for a short time before retrying
    }

    BusHandle_t* busHandle = nullptr; // Initialize bus handle to nullptr
    DisplayHandle_t* displayHandle = nullptr; // Initialize display handle to nullptr

    GFXLibraryWrapper::powerUp();

    // Create a data bus instance and display instance of Arduino_GFX_Library
    busHandle = GFXLibraryWrapper::createBusHandle();
    if (busHandle)
    {
        //-> Bus handle creation success

        // Create a display instance of Arduino_GFX_Library
        displayHandle = GFXLibraryWrapper::createDisplayHandle(busHandle);
    }

    // Initialize the display
    bool displayInitializeSuccessFlag = ((busHandle && displayHandle) && displayHandle->begin());
    if (displayInitializeSuccessFlag)
    {
        //-> Display initialization success

        // Clear the display with black color
        displayHandle->fillScreen(GFXLibraryWrapper::COLOR_BLACK);
    }
    else
    {
        //-> Display initialization fails

        // Check if both display and bus handles were created successfully
        if (displayHandle && busHandle) {
            //-> Display and bus handles creation success, but initialization(begin function) fails

            // Send the error message to the serial monitor
            serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::DISPLAY_BEGIN_FAILS);
        }

        // Clear Arduino_GFX_Library's instances
        if (displayHandle) {
            //-> Display handle creation success, but initialization fails

            GFXLibraryWrapper::deleteDisplayHandle(displayHandle); // Release instance, ignore failure
            displayHandle = nullptr; // Reset the pointer to nullptr
        }
        else {
            //-> Display handle creation fails

            // Send the error message to the serial monitor
            serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::DISPLAY_DISPLAY_CREATION_FAILS);
        }
        if (busHandle) {
            //-> Bus handle creation success, but display initialization fails
            GFXLibraryWrapper::deleteBusHandle(busHandle); // Release instance, ignore failure
            busHandle = nullptr; // Reset the pointer to nullptr
        }
        else
        {
            //-> Bus handle creation fails

            // Send the error message to the serial monitor
            serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::DISPLAY_BUS_CREATION_FAILS);
        }
    }

    // Release the mutex
    xSemaphoreGive(displayMutex);

    //　Store the bus and display instances in global variables for later use
    busHandleHolder = (void*) busHandle; // Store the bus instance in global variables
    displayHandleHolder = (void*) displayHandle; // Store the display instance in global variables

    vTaskDelay(pdMS_TO_TICKS(delayTimes::DISPLAY_INITIALIZE)); // wait for a short time before returning

    return displayInitializeSuccessFlag;
}

// Check if the display is initialized
bool Display::isInitialized()
{
    return (displayMutex && busHandleHolder && displayHandleHolder); // Check if displayMutex, busHandleHolder and displayHandleHolder are not nullptr
}

// Display "Hello World" message with counter on the screen
bool Display::displayHelloWorldWithCounter()
{
    namespace globalVars = Global_Variables; // Use the namespace for global variables
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace delayTimes = Delay_Times; // Use the namespace for delay times

    SerialLogger* serialLogger = reinterpret_cast<SerialLogger*>(globalVars::serialLoggerInstanceHolder); // Retrieve the serial logger instance from the global variable

    // Check if the display is initialized
    if (isInitialized())
    {
        //->　Display initialization success

        // Wait for the mutex to be available
        while (xSemaphoreTake(globalVars::sensorVarsMutex, pdMS_TO_TICKS(delayTimes::SENSOR_VARS_MUTEX)) != pdTRUE)
        {
            //-> Mutex acquisition fails

            vTaskDelay(pdMS_TO_TICKS(delayTimes::SENSOR_VARS_MUTEX_RETRY)); // Delay for a short time before retrying
        }

        // Create a message
        std::string message = commons::messageHeader; // Memory usage increases dynamically at runtime (in heap memory)
        message += "Temperature:=" + std::to_string(globalVars::sensorTemperature) + " C, ";
        message += "Humidity:=" + std::to_string(globalVars::sensorHumidity) + " %, ";
        message += "Pressure:=" + std::to_string(globalVars::sensorPressure) + " hPa, ";

        // Release the mutex
        xSemaphoreGive(globalVars::sensorVarsMutex);

        // Retrieve the display instance from the global variable
        DisplayHandle_t* displayHandle = (DisplayHandle_t*) displayHandleHolder;

        // Wait for the mutex to be available
        while (xSemaphoreTake(displayMutex, pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX)) != pdTRUE)
        {
            //-> Mutex acquisition fails
            vTaskDelay(pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX_RETRY)); // Delay for a short time before retrying
        }

        // Clear the display with black color
        displayHandle->fillScreen(GFXLibraryWrapper::COLOR_BLACK);

        // Set text color and text size and cursor position
        uint16_t textColor = random(GFXLibraryWrapper::COLOR_BLACK, GFXLibraryWrapper:: COLOR_WHITE); // Random color for text
        displayHandle->setTextColor(textColor);
        displayHandle->setTextSize(MESSAGE_TEXT_SIZE);
        displayHandle->setCursor(MESSAGE_CURSOR_X, MESSAGE_CURSOR_Y);

        // Display the message on the screen
        displayHandle->print(message.c_str());

        // Release the mutex
        xSemaphoreGive(displayMutex);

        return true;
    }
    else
    {
        //-> Display initialization fails

        // Send the error message to the serial monitor
        serialLogger->sendErrorMessage(SerialLogger::ErrorMessageId::DISPLAY_DISPLAY_FAILS);

        return false;
    }
}
