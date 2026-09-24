// === SOURCE-HEADER:BEGIN ===
/**
 * @file main.cpp
 * @author sohmatrix
 * @brief Main program for the T-Display S3 with Arduino_GFX_Library
 * @branch develop
 * @updated 2026-09-24
 */
// === SOURCE-HEADER:END === 18da3098d25b97806c3528f24cb303f39090b37e2dfba7bf9e3b2ad256a8a151

// This code is sample program for the T-Display S3 with Arduino_GFX_Library

// Global variables and constants
#include "common.hpp"       // Include the header file for program constants
#include "display.hpp"      // Include the header file for display handling

// Setup function for Arduino
void setup()
{
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    namespace globalVars = Global_Variables; // Use the namespace for global variables

    // Initialize serial logger instance
    logger.initialize();
    // Store the serial logger instance in the global variable for later use
    globalVars::serialLoggerInstanceHolder = (void*) &logger;

    // Retrieve the MAC address of the device and store it in the global variable
    snprintf(globalVars::macAddress, sizeof(globalVars::macAddress), "%016llx", ESP.getEfuseMac());

    // Initialize messageCounter
    globalVars::messageCounter = 0;

    // Initialize process times array
    for (uint32_t i = 0; i < commons::PROCESS_TIME_COUNTER_MAX; ++i)
    {
        globalVars::processTimes[i] = 0 /* dummy initialize value */; // ;
    }
    // Initialize process time counter
    globalVars::processTimeCounter = 0;

    // Create display instance
    Display* displayInstance = new Display();
    if (!displayInstance)
    {
        logger.sendErrorMessage(ErrorMessageId::DISPLAY_CREATION_FAILS);
        return;
    }
    // Initialize the display
    displayInstance->initialize();
    if (!(displayInstance->isInitialized()))
    {
        //-> Display initialization fails

        // Clean up the display instance to avoid memory leak
        delete displayInstance;
        displayInstance = nullptr;

        // Send the error message to the serial monitor
        logger.sendErrorMessage(ErrorMessageId::DISPLAY_INITIALIZATION_FAILS);

        return; // Exit setup() if display initialization fails
    }
    // Store the display instance in the global variable for later use
    globalVars::displayInstanceHolder = (void*) displayInstance;

    // No wait for a noticeable time before returning from setup()
}

// Main loop function for Arduino ... without finish condition, this will run forever
void loop()
{
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    namespace globalVars = Global_Variables; // Use the namespace for global variables

    // Record the start time of the loop iteration
    uint64_t loopStartTime = esp_timer_get_time(); // Get the start time in microseconds

    // Retrieve the instances from the global variable
    Display* displayInstance = reinterpret_cast<Display*>(globalVars::displayInstanceHolder); // Retrieve the display instance from the global variable

    // Increment messageCounter
    globalVars::messageCounter %= commons::MESSAGE_COUNTER_MAX; // Reset messageCounter if it exceeds MESSAGE_COUNTER_MAX
    ++(globalVars::messageCounter); // Increment messageCounter
 
    // Create a message
    std::string message = commons::MESSAGE_HEADER; // Memory usage increases dynamically at runtime (in heap memory)
    message += std::to_string(globalVars::messageCounter);

    // Display　message on the screen
    if (!displayInstance->displayMessage(message.c_str()))
    {
        //-> Display output fails

        // Send the error message to the serial monitor
        logger.sendErrorMessage(ErrorMessageId::DISPLAY_MESSAGE_FAILS);
    }

    // Record the end time of the loop iteration and store the process time
    uint64_t loopEndTime = esp_timer_get_time(); // Get the end time in microseconds
    uint64_t processTime = loopEndTime - loopStartTime; // Process time in microseconds

    // Check if this is the first loop iteration and the first process time is still the dummy value
    if (!((globalVars::processTimeCounter == 0) && (globalVars::processTimes[0] <= 0 /* <-dummy value */)))
    {
        //-> This is not the first loop iteration or the first process time has already been initialized

        // Update the process time for the current loop iteration
        globalVars::processTimeCounter %= commons::PROCESS_TIME_COUNTER_MAX; // Ensure the processTimeCounter wraps around correctly
        globalVars::processTimes[globalVars::processTimeCounter] = processTime;
        ++(globalVars::processTimeCounter);
    }
    else
    {
        //-> This is the first loop iteration and the first process time is still the dummy value

        // Initialize all process times with the first process time
        for (uint32_t i = 0; i < commons::PROCESS_TIME_COUNTER_MAX; ++i)
        {
            globalVars::processTimes[i] = processTime; // Initialize all process times with the first process time
        }
    }

    // Calculate the average process time
    uint64_t totalProcessTime = 0;
    for (uint32_t i = 0; i < commons::PROCESS_TIME_COUNTER_MAX; ++i)
    {
        totalProcessTime += globalVars::processTimes[i];
    }
    uint64_t averageProcessTime = totalProcessTime / commons::PROCESS_TIME_COUNTER_MAX; // Average process time in microseconds
    uint32_t averageProcessTimeMs = averageProcessTime / 1000; // Average process time in milliseconds
    uint32_t averageProcessTimeUs = (averageProcessTime % 1000) / 10; // Average process time in hundredths of a millisecond

    // Create the system and process time strings to display the system information
    std::string systemString = commons::TITLE_SYSTEM_HEADER;
    systemString += globalVars::macAddress; // Append the MAC address to the system string
    systemString += commons::TITLE_SYSTEM_FOOTER; // Append the footer to the system string
    std::string procesString = commons::TITLE_PROCESS_AVERAGE_PROCESS_TIME_HEADER;
    procesString += std::to_string(averageProcessTimeMs) + "." + std::to_string(averageProcessTimeUs);
    procesString += commons::TITLE_PROCESS_AVERAGE_PROCESS_TIME_FOOTER;
    
    // Display　message on the screen
    if (!displayInstance->displayTitle(systemString.c_str(), procesString.c_str()))
    {
        //-> Display output fails

        // Send the error message to the serial monitor
        logger.sendErrorMessage(ErrorMessageId::DISPLAY_TITLE_FAILS);
    }

    delay(delayTimes::LOOP); // Wait for the specified time before the next loop iteration

    // Note: The loop() function will run indefinitely, and the program will not reach this point unless there is an error in the display output.
}
