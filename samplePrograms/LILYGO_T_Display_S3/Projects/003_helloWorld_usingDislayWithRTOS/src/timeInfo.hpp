// === SOURCE-HEADER:BEGIN ===
/**
 * @file timeInfo.hpp
 * @author sohmatrix
 * @brief Time information structures and utilities for the program
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === 930d8f17d5a05598efc507b153d97b0a149e9fc74558b198e4ba383d6ca88d3f

#pragma once

#include "common.hpp" // for global variables and constants related to time info items

// TimeInfo structures, constants, and global variables for the program
namespace TimeInfo
{
    // Structure for heartbeat data, containing timeout and last time values
    using Id_t = size_t; // Type definition for the ID of the active check item
    using IdChar_t = char; // Type definition for the name character of the active check item
    using ProcessingTime_t = int64_t; // Type definition for processing time in microseconds
    struct ProcessingTimeInfoItem_t
    {
        IdChar_t idChar; // Name character for the active check item
        ProcessingTime_t averageProcessingTime; // Average processing time in microseconds for the time info item
    };

    // Structure for time info item, containing ID, name character, timeout, last time, and processing time values
    class TimeInfoItem_t
    {
    private:
        Id_t id; // ID for the active check item
        IdChar_t idChar; // Name character for the active check item
        ProcessingTime_t processingStartTime; // Process start time in microseconds for the heartbeat
        size_t processTimeCounter; // Counter for the number of process times recorded
        ProcessingTime_t processingTimes[Constants_Common::PROCESS_TIME_COUNTER_MAX]; // Process end time in microseconds for the heartbeat
        SemaphoreHandle_t timeInfoItemMutex; // Mutex for protecting access to the time info item

    private:
        // Reset the processing times for the time info item
        void resetProcessingTimes()
        {
            processTimeCounter = 0;
            for (uint32_t i = 0; i < Constants_Common::PROCESS_TIME_COUNTER_MAX; ++i)
            {
                processingTimes[i] = 0 /* <-dummy value */;
            }
        }

        // Get the processing time for the time info item
        ProcessingTime_t getAverageProcessingTime() const
        {
            ProcessingTime_t totalProcessingTime = 0;
            for (size_t i = 0; i < Constants_Common::PROCESS_TIME_COUNTER_MAX; ++i)
            {
                totalProcessingTime += processingTimes[i];
            }
            ProcessingTime_t averageProcessingTime = totalProcessingTime / Constants_Common::PROCESS_TIME_COUNTER_MAX;

            return averageProcessingTime;
        };

    public:
        // Constructor for the active check item, initializing the timeout and last time values
        TimeInfoItem_t(Id_t i, IdChar_t ic) : id(i), idChar(ic)
        {
            processingStartTime = 0 /* <-dummy value */;
            resetProcessingTimes();
            timeInfoItemMutex = nullptr;
        };

        // Destructor for the active check item, deleting the mutex
        ~TimeInfoItem_t()
        {
            if (timeInfoItemMutex)
            {
                //-> time info item mutex exists

                // delete the time info item mutex
                vSemaphoreDelete(timeInfoItemMutex);
                timeInfoItemMutex = nullptr;
            }
        };

        // Initialize the time info item, creating the mutex and resetting the processing times
        bool initialize()
        {
            processingStartTime = 0 /* <-dummy value */;
            resetProcessingTimes();
            timeInfoItemMutex = xSemaphoreCreateMutex();
            if (timeInfoItemMutex == nullptr)
            {
                return false; // Failed to create the mutex
            }

            return true; // Successfully initialized
        };


        // Update the process start time for the heartbeat
        void updateProcessingStartTime()
        {
            // namespace aliases for easier access to global variables and constants
            namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times

            if (!timeInfoItemMutex)
            {
                //-> Time info item mutex is not available

                // Send an error message indicating that the time info item mutex is not available
                logger.sendErrorMessage(ErrorMessageId::TIME_INFO_MUTEX_CREATION_FAILS);

                return; // Mutex is not available, exit the function
            }

            // Wait for the mutex to be available
            while (xSemaphoreTake(timeInfoItemMutex, pdMS_TO_TICKS(delayTimes::TIME_INFO_ITEM_MUTEX)) != pdTRUE)
            {
                //-> Mutex acquisition fails

                vTaskDelay(pdMS_TO_TICKS(delayTimes::TIME_INFO_ITEM_MUTEX_RETRY)); // Delay for a short time before retrying
            }

            // Update the processing start time for the heartbeat using mutex for thread safety
            processingStartTime = esp_timer_get_time(); // Store the current time as the processing start time for the heartbeat
            
            xSemaphoreGive(timeInfoItemMutex); // Release the mutex
        };

        // Update the process end time for the time info item
        void updateProcessingEndTime()
        {
            // namespace aliases for easier access to global variables and constants
            namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times

            if (!timeInfoItemMutex)
            {
                //-> Time info item mutex is not available

                // Send an error message indicating that the time info item mutex is not available
                logger.sendErrorMessage(ErrorMessageId::TIME_INFO_MUTEX_CREATION_FAILS);

                return; // Mutex is not available, exit the function
            }

            // Wait for the mutex to be available
            while (xSemaphoreTake(timeInfoItemMutex, pdMS_TO_TICKS(delayTimes::TIME_INFO_ITEM_MUTEX)) != pdTRUE)
            {
                //-> Mutex acquisition fails

                vTaskDelay(pdMS_TO_TICKS(delayTimes::TIME_INFO_ITEM_MUTEX_RETRY)); // Delay for a short time before retrying
            }

            // Setup the processing time information based on the start time using mutex for thread safety
            ProcessingTime_t currentProcessingTime = esp_timer_get_time() - processingStartTime; // Get the current processing time
            if ((processTimeCounter == 0) && (processingTimes[0]  <= 0 /* <-dummy value */)) 
            {
                //-> first update time for processing times

                // Initialize all processing times with the current processing time for calculating the average later
                for (size_t i = 0; i < Constants_Common::PROCESS_TIME_COUNTER_MAX; ++i) // Iterate over all processing time slots
                {
                    processingTimes[i] = currentProcessingTime;
                }
            }
            else
            {
                //-> subsequent update time for processing times

                processingTimes[processTimeCounter] = currentProcessingTime; // Update the processing time at the current index
            }

            processTimeCounter = (processTimeCounter + 1) % Constants_Common::PROCESS_TIME_COUNTER_MAX; // Update the process time counter in a circular manner
            processingStartTime = 0 /* <-dummy value */; // Reset the processing start time
            
            xSemaphoreGive(timeInfoItemMutex); // Release the mutex
        };

        bool getProcessingTimeInfoItem(ProcessingTimeInfoItem_t& processingTimeInfoItem)
        {
            // namespace aliases for easier access to global variables and constants
            namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times

            if (!timeInfoItemMutex)
            {
                //-> Time info item mutex is not available

                // Send an error message indicating that the time info item mutex is not available
                logger.sendErrorMessage(ErrorMessageId::TIME_INFO_MUTEX_CREATION_FAILS);

                return false; // Mutex is not available, exit the function
            }

            // Wait for the mutex to be available
            while (xSemaphoreTake(timeInfoItemMutex, pdMS_TO_TICKS(delayTimes::TIME_INFO_ITEM_MUTEX)) != pdTRUE)
            {
                //-> Mutex acquisition fails

                vTaskDelay(pdMS_TO_TICKS(delayTimes::TIME_INFO_ITEM_MUTEX_RETRY)); // Delay for a short time before retrying
            }

            // Store the processing time information using mutex to ensure thread safety
            processingTimeInfoItem.idChar = idChar;
            processingTimeInfoItem.averageProcessingTime = getAverageProcessingTime();

            xSemaphoreGive(timeInfoItemMutex); // Release the mutex

            return (true);
        };
    };

    // Definitions of global variables for the TimeInfoItem_t
    inline constexpr const size_t MESSAGE_COUNTER_TASK_ID = 0; // timeInfoItem counter for the message counter task
    inline constexpr const char MESSAGE_COUNTER_TASK_ID_CHAR = 'M'; // Name character for the message counter task
    inline constexpr const size_t DISPLAY_MESSAGE_TASK_ID = 1; // timeInfoItem counter for the display message task
    inline constexpr const char DISPLAY_MESSAGE_TASK_ID_CHAR = 'D'; // Name character for the display message task
   
      inline TimeInfoItem_t timeInfoItems[] = { // Array of time info items for current tasks status
        {MESSAGE_COUNTER_TASK_ID, MESSAGE_COUNTER_TASK_ID_CHAR},
        {DISPLAY_MESSAGE_TASK_ID, DISPLAY_MESSAGE_TASK_ID_CHAR},

    };
    inline constexpr const size_t NUM_OF_ACTIVE_CHECK_ITEMS = sizeof(timeInfoItems) / sizeof(timeInfoItems[0]); // Number of active check items
   
    // Reset all heartbeat and process times for all active check items
    inline bool initializeAllTimes()
    {
        for (size_t i = 0; i < NUM_OF_ACTIVE_CHECK_ITEMS; ++i) // Iterate over all active check items
        {
            if (!timeInfoItems[i].initialize()) // Initialize the current time info item and check for success
            {
                //-> Initialization of the current time info item failed

                return false; // Return false immediately
            }
        }
        return true; // Return true if all initializations succeed
    }

    // Get the processing times for all active check items
    inline bool getAverageProcessingTimes(std::vector<ProcessingTimeInfoItem_t> &processingTimeInfoVec)
    {
        processingTimeInfoVec.clear(); // Clear the vector to store the processing time info items
        for (size_t i = 0; i < NUM_OF_ACTIVE_CHECK_ITEMS; ++i) // Iterate over all active check items
        {
            ProcessingTimeInfoItem_t processingTimeinfoItem;
            if (!timeInfoItems[i].getProcessingTimeInfoItem(processingTimeinfoItem)) // Retrieve the processing time info item for the current time info item and check for success
            {
                //-> Retrieval of the processing time info item failed

                return false; // Return false immediately if retrieval of any processing time info item fails
            }
            processingTimeInfoVec.push_back(processingTimeinfoItem); // Add the retrieved processing time info item to the vector
        }

        return true; // Return true if all processing time info items are successfully retrieved
    }
};
