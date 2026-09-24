// === SOURCE-HEADER:BEGIN ===
/**
 * @file messageCounterTask.cpp
 * @author sohmatrix
 * @brief Task for managing the message counter using FreeRTOS
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === b89bd041bd181e84b60349a2c0f92b3ab0802e1c4b54c9c15f6864ea40496b79

// Include necessary headers for the message counter task implementation
#include "common.hpp" // for global variables and constants related to display message task
#include "timeInfo.hpp" // for time info instance access
#include "messageCounter.hpp" // for message counter instance access
#include "messageCounterTask.hpp" // for message counter task class definition

// Anonymous namespace for internal structure and variables
namespace {
    // Internal structure for holding message counter task parameters
    struct MessageCounterTaskParameters_t
    {
        void* messageCounterTaskInstanceHolder; // Pointer to the message counter task instance for the task
        const char* taskName; // Name of the message counter task for the task instance
        void* messageCounterInstanceHolder; // Pointer to the message counter instance for the task
        void* timeInfoItemHolder; // Pointer to the Time info item for the message counter task
    };

    // Internal constants for the message counter task
    inline static constexpr const char* MESSAGE_COUNTER_TASK_NAME = "MessageCounterTask"; // Task name for debugging
    inline static constexpr const uint32_t MESSAGE_COUNTER_TASK_STACK_SIZE = 4096; // Stack size in words for the message counter task
    inline static constexpr const uint32_t MESSAGE_COUNTER_TASK_PRIORITY = 1; // Task priority for the message counter task
}

// Member functions for the MessageCounterTask class

//-> Methods called externally and executed in the main task context
// Constructor
MessageCounterTask::MessageCounterTask()
{
    messageCounterTaskInstanceHolder = nullptr;
    taskName = MESSAGE_COUNTER_TASK_NAME;
    messageCounterInstanceHolder = nullptr;
    timeInfoItemHolder = nullptr;
};

// Destructor
MessageCounterTask::~MessageCounterTask()
{
    if (messageCounterTaskHandle)
    {
        //-> message counter task exists

        vTaskDelete(messageCounterTaskHandle); // Delete the message counter task if it exists
        messageCounterTaskHandle = nullptr;    // Reset the task handle
    }
    messageCounterTaskInstanceHolder = nullptr; // Reset the task instance holder
    messageCounterInstanceHolder = nullptr;     // Reset the message counter instance holder
    timeInfoItemHolder = nullptr;               // Reset the last heartbeat time holder
};

// Initialize the message counter task
bool MessageCounterTask::initialize(void *messageCounterInstanceHolder, void *timeInfoItemHolder)
{
    // namespace aliases for easier access to global variables and constants
    namespace globalVars = Global_Variables;     // Use the namespace for global variables
    namespace commons = Constants_Common;        // Use the namespace for program constants
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    // Retrieve necessary instances from the instance holders
    MessageCounter *messageCounterInstance = reinterpret_cast<MessageCounter *>(messageCounterInstanceHolder);
    TimeInfo::TimeInfoItem_t *timeInfoItem = reinterpret_cast<TimeInfo::TimeInfoItem_t *>(timeInfoItemHolder);

    // Check if the message counter instance is valid before proceeding
    if (!messageCounterInstance)
    {
        //-> Message counter instance is not provided

        // Send the error message to the serial monitor
        logger.setupLastErrorMessage(ErrorMessageId::MESSAGE_COUNTER_CREATION_FAILS);

        return false; // Exit initialization if the message counter instance is not provided
    }
    if (!(messageCounterInstance->isInitialized()))
    {
        //-> Message counter initialization fails

        // Send the error message to the serial monitor
        logger.setupLastErrorMessage(ErrorMessageId::MESSAGE_COUNTER_INITIALIZATION_FAILS);

        return false; // Exit initialization if the message counter instance is invalid
    }

    if (!timeInfoItem)
    {
        //-> Last heartbeat time is not provided

        // Send the error message to the serial monitor
        logger.setupLastErrorMessage(ErrorMessageId::TIME_INFO_CREATION_FAILS);

        return false; // Exit initialization if the last heartbeat time is not provided
    }

    // Create a message counter task parameter structure that includes this instance and necessary information
    MessageCounterTaskParameters_t *messageCounterTaskParameters =
        new MessageCounterTaskParameters_t{this, MESSAGE_COUNTER_TASK_NAME, messageCounterInstance, timeInfoItem};
    // Create a new message counter task with the parameters specified for this instance, similar to creating a new thread in other operating systems.
    BaseType_t taskCreationResult =
        xTaskCreate(MessageCounterTask::messageCounterTaskMain /* the entry point function for new FreeRTOS task */,
                    MESSAGE_COUNTER_TASK_NAME, MESSAGE_COUNTER_TASK_STACK_SIZE,
                    (void *)messageCounterTaskParameters /* Task parameter for the new message counter task as argument */, MESSAGE_COUNTER_TASK_PRIORITY,
                    (TaskHandle_t *)&(messageCounterTaskHandle) /* Task handle will be stored here */);
    //-> If successful, the new task starts running concurrently from the FreeRTOS task entry point function, while this execution continues as the main task.
    // Check if the new message counter task creation was successful
    if (taskCreationResult != pdPASS)
    {
        //-> Message counter task creation fails

        // Send the error message to the serial monitor
        logger.setupLastErrorMessage(ErrorMessageId::MESSAGE_COUNTER_TASK_CREATION_FAILS);

        // Clear the message counter task handle since the task creation failed
        messageCounterTaskHandle = nullptr;

        return false; // Exit initialization if message counter task creation fails
    }

    return true; // Message counter task initialization successful
}

// Check whether initialization was successful, i.e., whether the message counter task was created successfully.
bool MessageCounterTask::isInitialized() const
{
    return (messageCounterTaskHandle);
}

// Message counter task main function as the entry point for the FreeRTOS task
// - This function is static, so then it shared among all instances of the class, but the task parameter (argument parameter) has specific information for each task instance.
// - This function is used only as the entry point for the FreeRTOS task, and it is called first, and only once, when the task starts.
void MessageCounterTask::messageCounterTaskMain(void *pvParameters) // <- C-style callback function that is defined type-wise as the entry point for the FreeRTOS task
{
    // namespace aliases for easier access to global variables and constants
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    namespace globalVars = Global_Variables;     // Use the namespace for global variables
    // Extract the task parameters from the argument passed to the FreeRTOS task, task instance specific information is contained within these parameters
    MessageCounterTaskParameters_t *messageCounterTaskParameters = reinterpret_cast<MessageCounterTaskParameters_t *>(pvParameters); // Cast the parameter to MessageCounterTask pointer
    // Extract the message counter task instance from the task parameters to access member functions and variables of the current task instance
    MessageCounterTask *selfInstance = reinterpret_cast<MessageCounterTask *>(messageCounterTaskParameters->messageCounterTaskInstanceHolder); // Extract the message counter task instance from the task instance

    //-> selfInstance now is accessible and can be used to call member functions and access member variables of the current message counter task instance

    // Assign the extracted instances to nessecary member variables of the current message counter task instance
    selfInstance->messageCounterInstanceHolder = reinterpret_cast<void *>(messageCounterTaskParameters->messageCounterInstanceHolder); // Assign the message counter instance to the current message counter task instance
    selfInstance->timeInfoItemHolder = messageCounterTaskParameters->timeInfoItemHolder;                                               // Assign the time info item holder to the current message counter task instance
    selfInstance->taskName = messageCounterTaskParameters->taskName;                                                                   // Assign the task name to the current message counter task instance

    //-> Now the current message counter task instance is fully initialized and just behaves as new task, then executing its setup and loop functions that is same as ones in main functions

    // Call the setup and loop functions in this instance of the message counter task
    selfInstance->setup(); // Call the setup function for the current message counter task instance
    while (true)           // Infinite loop to repeatedly call the loop function for the current message counter task instance
    {
        selfInstance->loop();
    }

    return; // This line will never be reached due to the infinite loop above
}

//-> Methods called internally and executed in the message counter task context
// Setup function called by the message counter task, just same as the setup function in main task function
void MessageCounterTask::setup()
{
    // Retrieve the necessary instances from the instance holders
    MessageCounter *messageCounterInstance = reinterpret_cast<MessageCounter *>(messageCounterInstanceHolder);
    TimeInfo::TimeInfoItem_t *timeInfoItem = reinterpret_cast<TimeInfo::TimeInfoItem_t *>(timeInfoItemHolder); // Retrieve the time info item from the instance holder

    bool breakLoopFlag = false; // Flag to control breaking out of the loop
    do                          // Single iteration loop for error handling
    {
        if (!messageCounterInstance)
        {
            //-> Message counter instance is not available

            // Send the error message to the serial monitor
            logger.setupLastErrorMessage(ErrorMessageId::MESSAGE_COUNTER_CREATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break;                // Exit the do-while loop due to message counter instance not being available
        }

        if (!(messageCounterInstance->isInitialized()))
        {
            //-> Message counter is not initialized

            // Send the error message to the serial monitor
            logger.setupLastErrorMessage(ErrorMessageId::MESSAGE_COUNTER_INITIALIZATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break;                // Exit the do-while loop due to message counter not being initialized
        }

        // Clear the message counter to reset its value at the start of the task
        messageCounterInstance->clear();

    } while (false);
    // Check if the loop was broken due to an error
    if (!breakLoopFlag)
    {
        //-> No error occurred during the setup process

        if (timeInfoItem)
        {
            //-> Time info item is available

            timeInfoItem->updateLastTime(); // Update the last time for the message counter task
        }
        else
        {
            //-> Time info item is not available

            // Send an error message indicating that the time info item creation failed
            logger.setupLastErrorMessage(ErrorMessageId::TIME_INFO_CREATION_FAILS);
        }
    }
    else
    {
        //-> An error occurred during the setup process
    }

    // No delay for a noticeable time before returning from setup()
}

// Loop function called by the message counter task, just same as the loop function in main task function
void MessageCounterTask::loop()
{
    // namespace aliases for easier access to global variables and delay times
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    namespace commons = Constants_Common;        // Use the namespace for program constants
    // Retrieve necessary instances from the instance holders
    MessageCounter *messageCounterInstance = reinterpret_cast<MessageCounter *>(messageCounterInstanceHolder);
    TimeInfo::TimeInfoItem_t *timeInfoItem = reinterpret_cast<TimeInfo::TimeInfoItem_t *>(timeInfoItemHolder);

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
    do                          // Single iteration loop for error handling
    {
        if (!messageCounterInstance)
        {
            //-> Message counter instance is not available

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::MESSAGE_COUNTER_CREATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break;                // Exit the do-while loop due to message counter instance not being available
        }

        if (!(messageCounterInstance->isInitialized()))
        {
            //-> Message counter is not initialized

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::MESSAGE_COUNTER_INITIALIZATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break;                // Exit the do-while loop due to message counter not being initialized
        }

        // Increment the message counter and check for success
        if (!(messageCounterInstance->increment()))
        {
            //-> Message counter update fails

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::MESSAGE_COUNTER_INCREMENT_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break;                // Exit the do-while loop due to message counter update failure
        }

    } while (false); // End of single iteration loop for error handling
    // Check if the loop was broken due to an error and handle it accordingly
    if (!breakLoopFlag)
    {
        //-> Message counter task executed successfully

        if (timeInfoItem)
        {
            //-> Time info item pointer is valid

            timeInfoItem->updateProcessingEndTime(); // Update the processing end time for the heartbeat
            timeInfoItem->updateLastTime();          // Update the last time for the message counter task heartbeat
        }
        else
        {
            //-> Time info item pointer is invalid

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::TIME_INFO_CREATION_FAILS);
        }
    }
    else
    {
        //-> Message counter task encountered an error
    }

    vTaskDelay(pdMS_TO_TICKS(delayTimes::MESSAGE_COUNTER_LOOP)); // Wait for the specified time before the next loop iteration
}
