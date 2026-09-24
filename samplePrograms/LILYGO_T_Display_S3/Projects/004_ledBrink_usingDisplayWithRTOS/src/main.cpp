// === SOURCE-HEADER:BEGIN ===
/**
 * @file main.cpp
 * @author sohmatrix
 * @brief Main program for T-Display S3 with Arduino_GFX_Library and LED brink using RTOS
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === 65cc7c49d516555cf7f5902b5be31ca5fc29a121b43a936e57a3d6c592a037bd

// Include necessary headers for the project
#include "common.hpp"       // Include the header file for program constants
#include "timeInfo.hpp" // Include the header file for time info handling
#include "messageCounter.hpp" // Include the header file for message counter
#include "display.hpp"      // Include the header file for display handling
#include "messageCounterTask.hpp" // Include the header file for message counter task
#include "displayMessageTask.hpp" // Include the header file for display message task
#include "externalLed.hpp" // Include the header file for external LED handling

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

    // Check if the heartbeat task instance is not nullptr before deleting the task
    if (globalVars::messageCounterTaskInstanceHolder != nullptr)
    {
        //-> Message counter task handle is not nullptr, delete the message counter task

        vTaskDelete(reinterpret_cast<TaskHandle_t>(globalVars::messageCounterTaskInstanceHolder)); // Delete the message counter task
        globalVars::messageCounterTaskInstanceHolder = nullptr; // Reset the message counter task handle in the global variable
    }

    // Check if the message counter instance is not nullptr before deleting the instance
    if (globalVars::messageCounterInstanceHolder != nullptr)
    {
        //-> Message counter instance is not nullptr, delete the message counter instance

        delete reinterpret_cast<MessageCounter*>(globalVars::messageCounterInstanceHolder); // Delete the message counter instance
        globalVars::messageCounterInstanceHolder = nullptr; // Reset the message counter instance in the global variable
    }

    // Check if the external LED instance is not nullptr before deleting the instance
    if (globalVars::externalLedInstanceHolder != nullptr)
    {
        //-> External LED instance is not nullptr, delete the external LED instance

        delete reinterpret_cast<ExternalLed*>(globalVars::externalLedInstanceHolder); // Delete the external LED instance
        globalVars::externalLedInstanceHolder = nullptr; // Reset the external LED instance in the global variable
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

        // Create the message counter task instance
        MessageCounterTask *messageCounterTask = new MessageCounterTask();
        if (!messageCounterTask)
        {
            //-> Message counter task instance creation fails

            // Send the error message to the serial monitor
            logger.setupLastErrorMessage(ErrorMessageId::MESSAGE_COUNTER_TASK_CREATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Break out of the do-while loop
        }

        // Initialize the heartbeat task instance
        if (!(messageCounterTask->initialize(messageCounter,
            (void *)&(TimeInfo::timeInfoItems[TimeInfo::MESSAGE_COUNTER_TASK_ID]))))
        {
            //-> Message counter task initialization fails

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::MESSAGE_COUNTER_TASK_INITIALIZATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Break out of the do-while loop
        }
        // Store the message counter task instance in the global variable for later use
        globalVars::messageCounterTaskInstanceHolder = (void *)messageCounterTask;

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

        // Initialize the external LED instance
        ExternalLed *externalLed = new ExternalLed();
        if (!externalLed)
        {
            //-> External LED instance creation fails

            // Send the error message to the serial monitor
            logger.setupLastErrorMessage(ErrorMessageId::EXTERNAL_LED_CREATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Break out of the do-while loop
        }
        // Initialize the external LED instance
        if (!externalLed->initialize())
        {
            //-> External LED initialization fails

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::EXTERNAL_LED_INITIALIZATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Break out of the do-while loop
        }
        // Store the external LED instance in the global variable for later use
        globalVars::externalLedInstanceHolder = (void *)externalLed;

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

        TimeInfo::TimeInfoItem_t* timeInfoItem = &TimeInfo::timeInfoItems[TimeInfo::HEATBEAT_CHECK_TASK_ID];
        if (timeInfoItem)
        {
            //-> Time info item is available

            timeInfoItem->updateLastTime(); // Update the last time for the heartbeat check task
        }
        else
        {
            //-> Time info item is not available

            // Send an error message indicating that the time info item creation failed
            logger.sendErrorMessage(ErrorMessageId::TIME_INFO_CREATION_FAILS);
        }
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
    ExternalLed* externalLed = reinterpret_cast<ExternalLed*>(globalVars::externalLedInstanceHolder); // Retrieve the external LED instance from the global variable
    // Retrieve the time info item for the heartbeat check task
    TimeInfo::TimeInfoItem_t* timeInfoItem = &TimeInfo::timeInfoItems[TimeInfo::HEATBEAT_CHECK_TASK_ID];

    if (timeInfoItem)
    {
        //-> Time info item is available

        timeInfoItem->updateProcessingStartTime(); // Update the processing start time for the heartbeat
    }
    else
    {
        //-> Time info item is not available

        // Send an error message indicating that the time info item creation failed
        logger.sendErrorMessage(ErrorMessageId::TIME_INFO_CREATION_FAILS);
    }

    bool breakLoopFlag = false; // Flag to control breaking out of the loop
    do // Single iteration loop for error handling
    {
        // Check if the external LED instance is available before updating its state
        if (!externalLed)
        {
            //-> External LED instance is not available

            // Send an error message indicating that the external LED instance creation failed
            logger.sendErrorMessage(ErrorMessageId::EXTERNAL_LED_CREATION_FAILS);
            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Exit do-while loop due to unavailable external LED instance
        }
        // Check if the external LED instance is initialized before updating its state
        if (!externalLed->isInitialized())
        {
            //-> External LED initialization failed

            // Send an error message indicating that the external LED initialization failed
            logger.sendErrorMessage(ErrorMessageId::EXTERNAL_LED_INITIALIZATION_FAILS);
            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Exit do-while loop due to external LED initialization failure
        }

        // Check task heartbeat for last time update, whether the task is running or not
        if (TimeInfo::areHeartbeatsValid())
        {
            //-> Heartbeat check succeeds for the task

            // Update the state of the external LED (toggle it) to indicate heartbeat success
            if (externalLed->heartbeat())
            {
                //-> External LED state update succeeds

                // Nothing to do for the external LED, just keep it in the last state (on or off) to indicate heartbeat success
            }
            else
            {
                //-> External LED state update fails

                // Send the error message to the serial monitor
                logger.sendErrorMessage(ErrorMessageId::HEARTBEAT_HEARTBEAT_FAILS);
                breakLoopFlag = true; // Set the flag to break out of the loop
                break; // Exit do-while loop due to external LED heartbeat failure
            }
        }
        else
        {
            //-> Heartbeat check fails for the task

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::HEARTBEAT_HEARTBEAT_FAILS);
        }

    } while (false);
    // Check if the loop was broken due to an error and clean up global instances and handles if necessary
    if (!breakLoopFlag)
    {
        //-> No error occurred, continue normal execution

        if (timeInfoItem)
        {
            //-> Time info item is available

            timeInfoItem->updateProcessingEndTime(); // Update the processing end time for the heartbeat during normal execution
            timeInfoItem->updateLastTime(); // Update the last time for the heartbeat during normal execution
        }
        else
        {
            //-> Time info item is not available

            // Send an error message indicating that the time info item creation failed
            logger.sendErrorMessage(ErrorMessageId::TIME_INFO_CREATION_FAILS);
        }
    }
    else
    {
        //-> An error occurred, clean up global instances and handles before exiting

        cleanupGlobalInstancesAndHandles(); // Clean up global instances and handles before program exit
    }

    delay(delayTimes::MAIN_LOOP); // Wait for the specified time before the next loop iteration
}
