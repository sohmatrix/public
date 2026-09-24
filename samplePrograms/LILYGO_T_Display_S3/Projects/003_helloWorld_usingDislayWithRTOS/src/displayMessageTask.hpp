// === SOURCE-HEADER:BEGIN ===
/**
 * @file displayMessageTask.hpp
 * @author sohmatrix
 * @brief Task for managing display messages
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === 76712e93dce08d547d1d962be43dbc29ee5786f8d5e398dd0d8940089727c735

#pragma once

// Include necessary headers for the project
#include "common.hpp" // for global variables and constants related to display message task

// Class DisplayMessageTask for managing the display message task
class DisplayMessageTask
{
public:
    // Task handle for the display task
    TaskHandle_t displayTaskHandle; // Task handle for the display task

private:
    // Private member variables for the display message task instance
    const char* taskName; // Name of the display task for the task instance
    void* displayInstanceHolder; // Pointer to the display instance holder for the task
    void* messageCounterInstanceHolder; // Pointer to the message counter instance holder for the task
    void* timeInfoItemHolder; // Pointer to the active check item holder for the display message task

public:
    //-> Methods called externally and executed in the main task context
    // Constructor
    DisplayMessageTask();

    // Destructor
    ~DisplayMessageTask();

    // Initialize the display message task
    bool initialize(void* displayInstanceHolder, void* messageCounterInstanceHolder, void* timeInfoItemHolder);

    // Check whether initialization was successful, i.e., whether the display message task was created successfully
    bool isInitialized() const;

private:
    //-> Methods called within the instance running as the display message task created using FreeRTOS
    // Display message task main function as the entry point for the FreeRTOS task
    inline static void displayMessageTaskMain(void *pvParameters); // <- C-style callback function that is defined type-wise as the entry point for the FreeRTOS task

    //-> Methods called internally and executed in the display message task context
    // Setup function called by the display message task main function, just same as the setup function in main task function
    void setup();

    // Loop function called by the display message task main function, just same as the loop function in main task function
    void loop();


};
