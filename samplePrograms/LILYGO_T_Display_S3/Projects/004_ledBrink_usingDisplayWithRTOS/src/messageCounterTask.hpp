// === SOURCE-HEADER:BEGIN ===
/**
 * @file messageCounterTask.hpp
 * @author sohmatrix
 * @brief Task for managing the message counter using FreeRTOS
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === 164232517af10173b5070a1e2a28a7d5a1d8239d173c9d179d750a60fbd4a7a9

#pragma once

// Include necessary headers for the message counter task implementation
#include "common.hpp" // for global variables and constants related to display message task


// Message counter task class
class MessageCounterTask {
public:
    // Task handle for the message counter task
    TaskHandle_t messageCounterTaskHandle; // Task handle for the message counter task

private:
    // Private member variables for the message counter task instance
    void* messageCounterTaskInstanceHolder; // Pointer to the message counter task instance holder for the task
    const char* taskName; // Name of the message counter task for the task instance
    void* messageCounterInstanceHolder; // Pointer to the message counter instance holder for the task
    void* timeInfoItemHolder; // Pointer to the Time info item holder for the message counter task

public:
    //-> Methods called externally and executed in the main task context
    // Constructor
    MessageCounterTask();

    // Destructor
    ~MessageCounterTask();

    // Initialize the message counter task
    bool initialize(void *messageCounterInstanceHolder, void *timeInfoItemHolder);

    // Check whether initialization was successful, i.e., whether the message counter task was created successfully.
    bool isInitialized() const;

    //-> Methods called within the instance running as the message counter task created using FreeRTOS
    // Message counter task main function as the entry point for the FreeRTOS task
    inline static void messageCounterTaskMain(void *pvParameters); // <- C-style callback function that is defined type-wise as the entry point for the FreeRTOS task
 
    //-> Methods called internally and executed in the message counter task context
    // Setup function called by the message counter task, just same as the setup function in main task function
    void setup();
 
    // Loop function called by the message counter task, just same as the loop function in main task function
    void loop();
};
