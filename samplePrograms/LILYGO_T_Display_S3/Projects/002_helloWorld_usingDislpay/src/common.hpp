// === SOURCE-HEADER:BEGIN ===
/**
 * @file common.hpp
 * @author sohmatrix
 * @brief Common definitions and global variables for the program
 * @branch develop
 * @updated 2026-09-24
 */
// === SOURCE-HEADER:END === 1d2fc089347fc5006dcf16b2ccacfc8623e226d71e4825f60e631f9064ceb87c

// common.hpp
#pragma once

// Include necessary components for C/C++ environment
#include <new>         // for error handling
#include <string>      // using STL string class for easier string manipulation
// Include necessary libraries for Arduino and Arduino_GFX_Library
#include <Arduino.h>   // for Arduino framework
// Include necessary headers for the program
#include <serialLogger.hpp>

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
    // serial communication settings
    inline constexpr unsigned long SERIAL_BAUD_RATE = 115200; // Baud rate for serial communication
};

// Definitions of delay times for various operations in the program
namespace Constants_DelayTimes
{
    // Main loop delay times in milliseconds for various operations
    inline constexpr uint32_t LOOP = 1000; // Delay time in milliseconds for the loop

    // Serial Device delay times in milliseconds for various operations
    inline constexpr uint32_t SERIAL_INITIALIZE = 200; // Delay time in milliseconds for serial output initialization
    inline constexpr uint32_t DISPLAY_LOOP = 300; // Delay time in milliseconds for display for the diplay loop

    // Display device delay times in milliseconds for various operations
    inline constexpr uint32_t DISPLAY_INITIALIZE = 200; // Delay time in milliseconds for display initialization
    inline constexpr uint32_t DISPLAY_MUTEX = 100; // Delay time in milliseconds for display mutex acquisition
    inline constexpr uint32_t DISPLAY_MUTEX_RETRY = 100; // Delay time in milliseconds for display mutex retry
};


// Definitions of global variables for the program
namespace Global_Variables
{
    // System-related global variables for the program
    // - Mac address for the device
    inline char macAddress[4 /* hex header */ + 6 /* byte */ * 2 /* hex representation */ + 1 /* null terminator */] = {0 /* dummy initialize value */}; // Array to store the MAC address of the device
    // - Loop Process Time counter for measuring process time before it resets
    inline uint64_t processTimes[Constants_Common::PROCESS_TIME_COUNTER_MAX] = {0 /* dummy initialize value */}; // Array for storing process times before it resets
    inline uint32_t processTimeCounter = -1 /* dummy initialize value */; // Counter for the number of process times stored

    // Instances for the program
    inline void* serialLoggerInstanceHolder = nullptr; // serialLogger instance for serial logging
    inline void* displayInstanceHolder = nullptr; // display instance for display handling

    // State variables for the program
    inline uint32_t messageCounter = -1 /* dummy initialize value */; // Counter for the number of messages displayed


};
