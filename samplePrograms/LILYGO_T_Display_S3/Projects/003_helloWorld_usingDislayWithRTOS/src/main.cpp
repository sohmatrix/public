// === SOURCE-HEADER:BEGIN ===
/**
 * @file main.cpp
 * @author sohmatrix
 * @brief Main program for T-Display S3 with Arduino_GFX_Library using RTOS
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === 5399f4cbf9152161ddfd7616018a913a8380ad8306b3c5a06c29b37990bd4efc

// Include necessary headers for the project
#include "common.hpp"       // Include the header file for program constants
#include "timeInfo.hpp" // Include the header file for time info handling
#include "messageCounter.hpp" // Include the header file for message counter
#include "display.hpp"      // Include the header file for display handling
#include "displayMessageTask.hpp" // Include the header file for display message task

// Clean up global instances and handles before program exit
void cleanupGlobalInstancesAndHandles()
{
    // namespace aliases for easier access to global variables and constants
    namespace globalVars = Global_Variables; // Use the namespace for global variables

    // Check if the display message task instance is not nullptr before deleting the task
    if (globalVars::displayMessageTaskInstanceHolder != nullptr)
    {
        //-> Display task handle is not nullptr, delete the display task

        vTaskDelete(reinterpret_cast<TaskHandle_t>(globalVars::displayMessageTaskInstanceHolder)); // Delete the display task
        globalVars::displayMessageTaskInstanceHolder = nullptr; // Reset the display task handle in the global variable
    }

    // Check if the display instance is not nullptr before deleting the instance
    if (globalVars::displayInstanceHolder != nullptr)
    {
        //-> Display instance is not nullptr, delete the display instance

        delete reinterpret_cast<Display*>(globalVars::displayInstanceHolder); // Delete the display instance
        globalVars::displayInstanceHolder = nullptr; // Reset the display instance in the global variable
    }

    // Check if the message counter instance is not nullptr before deleting the instance
    if (globalVars::messageCounterInstanceHolder != nullptr)
    {
        //-> Message counter instance is not nullptr, delete the message counter instance

        delete reinterpret_cast<MessageCounter*>(globalVars::messageCounterInstanceHolder); // Delete the message counter instance
        globalVars::messageCounterInstanceHolder = nullptr; // Reset the message counter instance in the global variable
    }
}

// Setup function for Arduino
void setup()
{
    // namespace aliases for easier access to global variables and constants
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    namespace globalVars = Global_Variables; // Use the namespace for global variables

    // Initialize serial logger instance
    logger.initialize();
    // Store the serial instance in the global variable
    globalVars::serialLoggerInstanceHolder = (void*)&logger;

    // Retrieve the MAC address of the device and store it in the global variable
    snprintf(globalVars::macAddress, sizeof(globalVars::macAddress), "%016llx", ESP.getEfuseMac());

    bool breakLoopFlag = false; // Flag to control breaking out of the loop
    do // Single iteration loop for error handling
    {
        // Reset all heartbeat and process times for all active check items
        if (!TimeInfo::initializeAllTimes())
        {
            //-> Initialization of all times fails

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::TIME_INFO_CREATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Break out of the do-while loop
        }
        // Store the time info items instance in the global variable
        globalVars::timeInfoItemsInstanceHolder = (void*)&TimeInfo::timeInfoItems; 

        // Create an instance of the message counter
        MessageCounter *messageCounter = new MessageCounter();
        if (!(messageCounter))
        {
            //-> Mutex creation fails

            // Send the error message to the serial monitor
            logger.setupLastErrorMessage(ErrorMessageId::MESSAGE_COUNTER_MUTEX_CREATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Break out of the do-while loop
        }
        // Check if the message counter instance is successfully initialized
        if (!(messageCounter->initialize()))
        {
            //-> Message counter initialization fails

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::MESSAGE_COUNTER_INITIALIZATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Break out of the do-while loop
        }
        // Store the message counter instance in the global variable for later use
        globalVars::messageCounterInstanceHolder = (void *)messageCounter;

        // Create display instance
        Display *displayInstance = new Display();
        if (!displayInstance)
        {
            //-> Display instance creation fails

            // Send the error message to the serial monitor
            logger.setupLastErrorMessage(ErrorMessageId::DISPLAY_CREATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Break out of the do-while loop
        }

        // Initialize the display instance
        if (!(displayInstance->initialize()))
        {
            //-> Display initialization fails

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::DISPLAY_INITIALIZATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Break out of the do-while loop
        }
        // Store the display instance in the global variable for later use
        globalVars::displayInstanceHolder = (void *)displayInstance;

        // Create display message task instance
        DisplayMessageTask *displayMessageTask = new DisplayMessageTask();
        if (!displayMessageTask)
        {
            //-> Display message task instance creation fails

            // Send the error message to the serial monitor
            logger.setupLastErrorMessage(ErrorMessageId::DISPLAY_MESSAGE_TASK_CREATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Break out of the do-while loop
        }
        // Initialize the display message task instance
        if (!(displayMessageTask->initialize((void *)displayInstance, (void *)messageCounter,
            (void *)&(TimeInfo::timeInfoItems[TimeInfo::DISPLAY_MESSAGE_TASK_ID]))))
        {
            //-> Display message task initialization fails

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::DISPLAY_MESSAGE_TASK_INITIALIZATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Break out of the do-while loop
        }
        // Store the display message task instance in the global variable for later use
        globalVars::displayMessageTaskInstanceHolder = (void *)displayMessageTask;

    } while (false);
    // Single iteration loop epilogue for error handling
    if (breakLoopFlag)
    {
        //-> An error occurred during do-while loop above
        cleanupGlobalInstancesAndHandles(); // Clean up global instances and handles before program exit
    }
    else
    {
        //-> No error occurred during do-while loop above
    }

    // No delay for a noticeable time before returning from setup()
}

// Main loop function for Arduino ... without finish condition, this will run forever
void loop()
{
    // namespace aliases for easier access to global variables and constants
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    namespace globalVars = Global_Variables; // Use the namespace for global variables
    // Retrieve the instances from the global variable
    MessageCounter* messageCounter = reinterpret_cast<MessageCounter*>(globalVars::messageCounterInstanceHolder); // Retrieve the message counter instance from the global variable
    TimeInfo::TimeInfoItem_t* timeInfoitem = &(TimeInfo::timeInfoItems[TimeInfo::MESSAGE_COUNTER_TASK_ID]); // Retrieve the time info item for the message counter task from the global variable

    do
    {
        if (timeInfoitem)
        {
            //-> Time info item for the message counter task is available

            timeInfoitem->updateProcessingStartTime(); // Update the processing start time for the message counter task
        }
        else
        {
            //-> Time info item for the message counter task is not initialized

            logger.sendErrorMessage(ErrorMessageId::TIME_INFO_CREATION_FAILS);
            break; // Exit do-while loop due to uninitialized time info item
        }

        if (!messageCounter)
        {
            //-> Message counter instance is not initialized

            logger.sendErrorMessage(ErrorMessageId::MESSAGE_COUNTER_INITIALIZATION_FAILS);
            break; // Exit do-while loop due to uninitialized message counter
        }
        // Increment the message counter if it is initialized
        if (!messageCounter->increment())
        {
            //-> Increment failed
            logger.sendErrorMessage(ErrorMessageId::MESSAGE_COUNTER_INCREMENT_FAILS);
            break; // Exit do-while loop due to message counter increment failure
        }

        if (timeInfoitem)
        {
            //-> Time info item for the message counter task is available

            timeInfoitem->updateProcessingEndTime(); // Update the processing end time for the message counter task
        }
        else
        {
            //-> Time info item for the message counter task is not initialized

            logger.sendErrorMessage(ErrorMessageId::TIME_INFO_CREATION_FAILS);
            break; // Exit do-while loop due to uninitialized time info item
        }

    } while (false);

    delay(delayTimes::MAIN_LOOP); // Wait for the specified time before the next loop iteration
}
