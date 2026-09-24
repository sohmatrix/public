// === SOURCE-HEADER:BEGIN ===
/**
 * @file serialLogger.hpp
 * @author sohmatrix
 * @brief Serial logger for error and message reporting
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === 1d379e49cd310454882d8177559d91bdcf101344ea31afaf551003af02edec57

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




