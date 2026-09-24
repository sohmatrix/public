// === SOURCE-HEADER:BEGIN ===
/**
 * @file displayMessageTask.cpp
 * @author sohmatrix
 * @brief Task for managing display messages
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === 9fd151650312433d6fb70f58a82171e9e4f50d4260dba72009ded10fa8e8d73c

// Include necessary headers for the project
#include "common.hpp" // for global variables and constants related to display message task
#include "timeInfo.hpp" // for time info instance access
#include "messageCounter.hpp" // for message counter instance access
#include "display.hpp" // for display instance access
#include "displayMessageTask.hpp" // for display message task class definition

// Anonymous namespace for internal structure and variables
namespace {
    // Internal structure for holding display message task parameters
    struct DisplayMessageTaskParameters_t
    {
        void* displayMessageTaskInstanceHolder; // Pointer to the display message task instance for the task
        const char* taskName; // Name of the display message task for the task instance
        void* displayInstanceHolder; // Pointer to the display instance for the task
        void* messageCounterInstanceHolder; // Pointer to the message counter instance for the task
        void* timeInfoItemHolder; // Pointer to the active check item for the display message task
    };

    // Internal constants for the display message task
    inline static constexpr const char* DISPLAY_MESSAGE_TASK_NAME = "DisplayTask"; // Task name for debugging
    inline static constexpr const uint32_t DISPLAY_MESSAGE_TASK_STACK_SIZE = 4096; // Stack size in words for the display message task
    inline static constexpr const uint32_t DISPLAY_MESSAGE_TASK_PRIORITY = 1; // Task priority for the display message task
}

// Member functions for the DisplayMessageTask class

//-> Methods called externally and executed in the main task context
// Constructor
DisplayMessageTask::DisplayMessageTask()
{
    displayTaskHandle = nullptr;
    displayInstanceHolder = nullptr;
    messageCounterInstanceHolder = nullptr;
    timeInfoItemHolder = nullptr;
    taskName = nullptr;
}

// Destructor
DisplayMessageTask::~DisplayMessageTask()
{
    if (displayTaskHandle)
    {
        vTaskDelete(displayTaskHandle); // Delete the display task if it exists
        displayTaskHandle = nullptr;    // Reset the task handle
    }
}

// Initialize the display message task
bool DisplayMessageTask::initialize(void* displayInstanceHolder, void* messageCounterInstanceHolder, void* timeInfoItemHolder)
{
    // namespace aliases for easier access to global variables and constants
    namespace globalVars = Global_Variables;     // Use the namespace for global variables
    namespace commons = Constants_Common;        // Use the namespace for program constants
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    // Retrieve necessary instances from the instance holders
    Display *displayInstance = reinterpret_cast<Display *>(displayInstanceHolder);
    MessageCounter *messageCounterInstance = reinterpret_cast<MessageCounter *>(messageCounterInstanceHolder);
    TimeInfo::TimeInfoItem_t* timeInfoItem = reinterpret_cast<TimeInfo::TimeInfoItem_t*>(timeInfoItemHolder);

    // Check if the display instance is valid before proceeding
    if (!displayInstance || !(displayInstance->isInitialized()))
    {
        //-> Display initialization fails

        // Send the error message to the serial monitor
        logger.sendErrorMessage(ErrorMessageId::DISPLAY_INITIALIZATION_FAILS);

        return false; // Exit initialization if the display instance is invalid
    }
    // Check if the message counter instance is valid before proceeding
    if (!messageCounterInstance || !(messageCounterInstance->isInitialized()))
    {
        //-> Message counter initialization fails

        // Send the error message to the serial monitor
        logger.sendErrorMessage(ErrorMessageId::MESSAGE_COUNTER_INITIALIZATION_FAILS);

        return false; // Exit initialization if the message counter instance is invalid
    }

    // Create a display message task parameter structure that includes this instance and necessary information
    DisplayMessageTaskParameters_t *displayTaskParameters =
        new DisplayMessageTaskParameters_t{this, DISPLAY_MESSAGE_TASK_NAME, displayInstance, messageCounterInstance, timeInfoItem};
    // Create a new display message task with the parameters specified for this instance, similar to creating a new thread in other operating systems.
    BaseType_t taskCreationResult =
        xTaskCreate(DisplayMessageTask::displayMessageTaskMain /* the entry point function for new FreeRTOS task */,
                    DISPLAY_MESSAGE_TASK_NAME, DISPLAY_MESSAGE_TASK_STACK_SIZE,
                    (void *)displayTaskParameters /* Task parameter for the new display message task as argument */, DISPLAY_MESSAGE_TASK_PRIORITY,
                    (TaskHandle_t *)&(displayTaskHandle) /* Task handle will be stored here */);
    //-> If successful, the new task starts running concurrently from the FreeRTOS task entry point function, while this execution continues as the main task.
    // Check if the new display message task creation was successful
    if (taskCreationResult != pdPASS)
    {
        //-> Display message task creation fails

        // Send the error message to the serial monitor
        logger.setupLastErrorMessage(ErrorMessageId::DISPLAY_TASK_CREATION_FAILS);

        // Clear the display task handle since the task creation failed
        displayTaskHandle = nullptr;

        return false; // Exit initialization if display message task creation fails
    }

    return true; // Display task initialization successful
}

// Check whether initialization was successful, i.e., whether the display message task was created successfully.
bool DisplayMessageTask::isInitialized() const
{
    return (displayTaskHandle != nullptr);
}

// Display message task main function as the entry point for the FreeRTOS task
// - This function is static, so then it shared among all instances of the class, but the task parameter (argument parameter) has specific information for each task instance.
// - This function is used only as the entry point for the FreeRTOS task, and it is called first, and only once, when the task starts.
void DisplayMessageTask::displayMessageTaskMain(void *pvParameters) // <- C-style callback function that is defined type-wise as the entry point for the FreeRTOS task
{
    // namespace aliases for easier access to global variables and constants
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    namespace globalVars = Global_Variables;     // Use the namespace for global variables

    // Extract the task parameters from the argument passed to the FreeRTOS task, task instance specific information is contained within these parameters
    DisplayMessageTaskParameters_t *displayTaskParameters = reinterpret_cast<DisplayMessageTaskParameters_t *>(pvParameters); // Cast the parameter to DisplayMessageTask pointer
    // Extract the display message task instance from the task parameters to access member functions and variables of the current task instance
    DisplayMessageTask *selfInstance = reinterpret_cast<DisplayMessageTask *>(displayTaskParameters->displayMessageTaskInstanceHolder); // Extract the display message task instance from the task instance

    //-> selfInstance now is accessible and can be used to call member functions and access member variables of the current display message task instance

    // Assign the extracted instances to nessecary member variables of the current display message task instance
    selfInstance->displayInstanceHolder = reinterpret_cast<void *>(displayTaskParameters->displayInstanceHolder); // Assign the display instance to the current display message task instance
    selfInstance->messageCounterInstanceHolder = reinterpret_cast<void *>(displayTaskParameters->messageCounterInstanceHolder); // Assign the message counter instance to the current display message task instance
    selfInstance->taskName = displayTaskParameters->taskName; // Assign the task name to the current display message task instance
    selfInstance->timeInfoItemHolder = reinterpret_cast<void *>(displayTaskParameters->timeInfoItemHolder); // Assign the time info item holder to the current display message task instance

    //-> Now the current display message task instance is fully initialized and just behaves as new task, then executing its setup and loop functions that is same as ones in main functions

    // Call the setup and loop functions in this instance of the display message task
    selfInstance->setup(); // Call the setup function for the display message task instance
    while (true) // Infinite loop to repeatedly call the loop function for the display message task instance
    {
        selfInstance->loop();
    }
    return; // This line will never be reached due to the infinite loop above
}

//-> Methods called internally and executed in the display message task context
// Setup function called by the display message task, just same as the setup function in main task function
void DisplayMessageTask::setup()
{
    // namespace aliases for easier access to global variables and delay times
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    namespace commons = Constants_Common;        // Use the namespace for program constants
    namespace globalVars = Global_Variables;     // Use the namespace for global variables
    // Retrieve the necessary instances from the instance holders
    Display *displayInstance = reinterpret_cast<Display *>(displayInstanceHolder);
    TimeInfo::TimeInfoItem_t* timeInfoItem = reinterpret_cast<TimeInfo::TimeInfoItem_t*>(timeInfoItemHolder);

    bool breakLoopFlag = false; // Flag to control breaking out of the loop
    do // Single iteration loop for error handling
    {
        if (!displayInstance)
        {
            //-> Display instance is not available

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::DISPLAY_CREATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Exit the do-while loop due to display instance not being available
        }

        if (!(reinterpret_cast<Display *>(displayInstanceHolder)->isInitialized()))
        {
            //-> Display is not initialized

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::DISPLAY_INITIALIZATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Exit the do-while loop due to display not being initialized
        }

        // Clear the display before starting the loop
        displayInstance->displayClear();

    } while (false);
    // Check if the loop was broken due to an error and handle it accordingly
    if (!breakLoopFlag)
    {
        //-> Display setup succeeded
    }
    else
    {
        //-> Display setup failed
    }

    // No delay for a noticeable time before returning from setup()
}

// Loop function called by the display message task, just same as the loop function in main task function
void DisplayMessageTask::loop()
{
    // namespace aliases for easier access to global variables and delay times
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    namespace commons = Constants_Common;        // Use the namespace for program constants
    namespace globalVars = Global_Variables;     // Use the namespace for global variables
    // Retrieve necessary instances from the instance holders
    Display *displayInstance = reinterpret_cast<Display *>(displayInstanceHolder);
    MessageCounter *messageCounterInstance = reinterpret_cast<MessageCounter *>(messageCounterInstanceHolder);
    TimeInfo::TimeInfoItem_t* timeInfoItem = reinterpret_cast<TimeInfo::TimeInfoItem_t*>(timeInfoItemHolder);

    // Record the start time of the loop iteration
    if (timeInfoItem)
    {
        //-> Time info item is available

        timeInfoItem->updateProcessingStartTime(); // Update the start time of processing for the time info item
    }
    else
    {
        //-> Time info item is not available

        //　Send an error message indicating that the time info item creation failed
        logger.sendErrorMessage(ErrorMessageId::TIME_INFO_CREATION_FAILS);
    }

    // Create the system strings to be displayed on the screen
    std::string systemString = commons::TITLE_SYSTEM_HEADER;
    systemString += globalVars::macAddress; // Append the MAC address to the system string
    systemString += commons::TITLE_SYSTEM_FOOTER; // Append the footer to the system string

    // Create the process time strings to be displayed on the screen
    std::vector<TimeInfo::ProcessingTimeInfoItem_t> processingTimeInfoVec;
    TimeInfo::getAverageProcessingTimes(processingTimeInfoVec); // Retrieve the average processing times for all active check items
    std::string processString = commons::TITLE_PROCESS_AVERAGE_PROCESS_TIME_HEADER;
    for (const auto& item : processingTimeInfoVec)
    {
        std::string itemIdChar(1, item.idChar); // Convert the character ID to a string for concatenation
        uint32_t averageProcessTime = item.averageProcessingTime; // Average process time in microseconds
        uint32_t averageProcessTimeMsec = averageProcessTime / 1000; // Milliseconds part of the average process time
        uint32_t averageProcessTimeUsec = (averageProcessTime % 1000); // Microsecond part of the average process time
        processString += itemIdChar + ":" + std::to_string(averageProcessTimeMsec) + "." + std::to_string(averageProcessTimeUsec) + ", ";
    }
    processString.pop_back(); processString.pop_back(); // Remove the trailing comma and space
    processString += commons::TITLE_PROCESS_AVERAGE_PROCESS_TIME_FOOTER;

    // Create the message to be displayed on the screen
    std::string message = commons::MESSAGE_HEADER; // Memory usage increases dynamically at runtime (in heap memory)
    message += std::to_string(messageCounterInstance->getValue()); // Append the message counter value to the message string

    bool breakLoopFlag = false; // Flag to control breaking out of the loop
    do // Single iteration loop for error handling
    {
        if (!(displayInstance->isInitialized()))
        {
            //-> Display is not initialized

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::DISPLAY_INITIALIZATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Exit the do-while loop due to display not being initialized
        }

        // Display　message on the screen
        if (!displayInstance->displayTitle(systemString.c_str(), processString.c_str()))
        {
            //-> Display output fails

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::DISPLAY_TITLE_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Exit the do-while loop due to display output failure
        }

        // Display message with counter on the screen
        if (!(displayInstance->displayMessage(message.c_str())))
        {
            //-> Display output fails

            // Send the error message to the serial monitor
            logger.sendErrorMessage(ErrorMessageId::DISPLAY_MESSAGE_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Exit the do-while loop due to display output failure
        }

    } while (false);
    // Check if the loop was broken due to an error and handle it accordingly
    if (!breakLoopFlag)
    {
        //-> Display message task executed successfully

        if (timeInfoItem)
        {
            //-> time info item is available

            timeInfoItem->updateProcessingEndTime(); // Update the processing end time for the display message task
        }
        else
        {
            //-> time info item is not available

            // Send an error message indicating that the time info item creation failed
            logger.sendErrorMessage(ErrorMessageId::TIME_INFO_CREATION_FAILS);
        }
    }
    else
    {
        //-> Display message task encountered an error
    }

    vTaskDelay(pdMS_TO_TICKS(delayTimes::DISPLAY_LOOP)); // Wait for the specified time before the next loop iteration
}
