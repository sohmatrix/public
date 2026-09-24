// === SOURCE-HEADER:BEGIN ===
/**
 * @file serialLogger.hpp
 * @author sohmatrix
 * @brief Serial logger definitions for error and message reporting
 * @branch develop
 * @updated 2026-09-24
 */
// === SOURCE-HEADER:END === 1a4afc009e177628adb37e19f0984ecbddbe73a3c32f311fd6708389a0ba0448

#pragma once

// Include necessary headers for the project
#include "errorMessages.hpp"

// Serial Logger class definition and static instance declaration.
// - A single instance of the Serial Logger class exists statically and is accessible from anywhere. 
// - Intentionally not protected by a mutex or similar synchronization mechanism so that it is immediately accessible and usable at any time.
inline class SerialLogger
{
    private:
        // Last message for setup
        inline static ErrorMessageId setupLastErrorMessageId;

    public:
        // Initialization for serial logger
        static void initialize();

        // Send error message to serial logger
        static void sendErrorMessage(ErrorMessageId errorMessageId);

        // Send message to serial monitor
        static void sendMessage(const char* messageString);

        // Set the last message for setup
        static void setupLastErrorMessage(ErrorMessageId errorMessageId);

} logger; // Create a static instance of SerialLogger named logger




