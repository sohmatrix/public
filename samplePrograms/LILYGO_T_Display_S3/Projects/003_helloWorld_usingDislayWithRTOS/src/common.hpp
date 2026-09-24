// === SOURCE-HEADER:BEGIN ===
/**
 * @file common.hpp
 * @author sohmatrix
 * @brief This header file contains common constants, delay times, and global variables for the program.
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === f8612fa07c140608da3e0a2463abe4cc08fcafbb2efe779406ddbbabe4077a4b

// common.hpp
#pragma once

// Include necessary standard libraries
#include <new> // for dynamic memory management (new and delete operators)
#include <atomic> // for atomic operations
#include <string> // for using std::string in STL
#include <vector> // for using std::vector in STL
// Include necessary libraries for Arduino
#include <Arduino.h> // for Arduino framework
// Include necessary libraries for FreeRTOS
#include <freertos/FreeRTOS.h> // for FreeRTOS framework
#include <freertos/task.h> // for FreeRTOS task management
#include <freertos/semphr.h> // for FreeRTOS semaphore management
// Include necessary headers for the project
#include <serialLogger.hpp> // for serial logger functionality, global access to the logger instance

// Common Constant definitions for the program
namespace Constants_Common
{
    // Definitions of constants for the program
    inline constexpr const char* TITLE_SYSTEM_HEADER = "LILYGO T-Display S3 ("; // Header for the title to be displayed on the screen
    inline constexpr const char* TITLE_SYSTEM_FOOTER = ")"; // Footer for the title to be displayed on the screen
    inline constexpr const char* TITLE_PROCESS_AVERAGE_PROCESS_TIME_HEADER = "Process time: "; // Header for the display average process time message
    inline constexpr const char* TITLE_PROCESS_AVERAGE_PROCESS_TIME_FOOTER = "(msec)"; // Footer for the display average process time
    inline constexpr const char* MESSAGE_HEADER = "Hello World: "; // Header for the message to be displayed on the screen
    inline constexpr const char* MESSAGE_FOOTER = " | Last Setup Message: "; // Footer for the message to be displayed on the screen
    inline constexpr const char* MESSAGE_FOOTER_END = " <<<"; // End of the message footer
    inline constexpr uint32_t MESSAGE_COUNTER_MAX = 1000; // Maximum value for messageCounter before it resets
    inline constexpr uint32_t PROCESS_TIME_COUNTER_MAX = 1000; // Maximum value for process time counter before it resets
    // Serial communication settings
    inline constexpr unsigned long SERIAL_BAUD_RATE = 115200; // Baud rate for serial communication
};

// Definitions of delay times for various operations in the program
namespace Constants_DelayTimes
{
    // Main loop delay times in milliseconds for various operations
    inline constexpr const uint32_t MAIN_LOOP = 1000; // Delay time in milliseconds for the loop

    // Serial Device delay times in milliseconds for various operations
    inline constexpr const uint32_t SERIAL_INITIALIZE = 200; // Delay time in milliseconds for serial output initialization
    inline constexpr const uint32_t SERIAL_MUTEX = 100; // Delay time in milliseconds for serial mutex acquisition
    inline constexpr const uint32_t SERIAL_MUTEX_RETRY = 100; // Delay time in milliseconds for serial mutex retry

    // Display loop delay time in milliseconds for various operations
    inline constexpr const uint32_t DISPLAY_LOOP = 300; // Delay time in milliseconds for display for the diplay loop

    // Display device delay times in milliseconds for various operations
    inline constexpr const uint32_t DISPLAY_INITIALIZE = 200; // Delay time in milliseconds for display initialization
    inline constexpr const uint32_t DISPLAY_MUTEX = 100; // Delay time in milliseconds for display mutex acquisition
    inline constexpr const uint32_t DISPLAY_MUTEX_RETRY = 100; // Delay time in milliseconds for display mutex retry

    // Message counter variables delay times in milliseconds for various operations
    inline constexpr const uint32_t MESSAGE_COUNTER_MUTEX = 100; // Delay time in milliseconds for message counter mutex acquisition
    inline constexpr const  uint32_t MESSAGE_COUNTER_MUTEX_RETRY = 100; // Delay time in milliseconds for message counter mutex retry

    // Time info item mutex acquisition delay times in milliseconds for various operations
    inline constexpr const uint32_t TIME_INFO_ITEM_MUTEX = 100; // Delay time in milliseconds for time info item mutex acquisition
    inline constexpr const uint32_t TIME_INFO_ITEM_MUTEX_RETRY = 100; // Delay time in milliseconds for time info item mutex retry
};

// Definitions of global variables for the program
namespace Global_Variables
{
    // System-related global variables for the program
    // - Mac address for the device
    inline char macAddress[4 /* hex header */ + 6 /* byte */ * 2 /* hex representation */ + 1 /* null terminator */] = {0 /* dummy initialize value */}; // Array to store the MAC address of the device

    // Instances for the program
    inline void* serialLoggerInstanceHolder = nullptr; // serialLogger instance for serial logging
    inline void* timeInfoItemsInstanceHolder = nullptr; // timeInfo items instance for time info handling
    inline void* messageCounterInstanceHolder = nullptr; // messageCounter instance for message counter handling
    inline void* displayInstanceHolder = nullptr; // display instance for display handling
    inline void* displayMessageTaskInstanceHolder = nullptr; // display message task instance for handling display messages
};
