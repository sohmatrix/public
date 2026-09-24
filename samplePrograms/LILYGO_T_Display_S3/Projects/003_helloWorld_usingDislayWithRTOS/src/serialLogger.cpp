// === SOURCE-HEADER:BEGIN ===
/**
 * @file serialLogger.cpp
 * @author sohmatrix
 * @brief This source file contains the implementation of the serial logger class for the program.
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === 552263010e1fabf999c62b41de76f9fd0af7a2fc7dce4c9e61e8d4438771090f

// Include necessary headers for the project
#include "common.hpp" // Include common definitions and serial logger declarations

// methods for serial logger class

// Initialization for serial logger
void SerialLogger::initialize()
{
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace dateTimes = Constants_DelayTimes; // Use the namespace for delay times

    setupLastErrorMessageId = ErrorMessageId::UNKNOWN_ERROR; // Initialize the last message ID for setup

    // Initialize serial communication with the specified baud rate
    Serial.begin(commons::SERIAL_BAUD_RATE);
    delay(dateTimes::SERIAL_INITIALIZE); // Delay for serial output initialization

    return; // No return value for void function
}

// Send error message to serial logger
void SerialLogger::sendErrorMessage(ErrorMessageId errorMessageId)
{
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace dateTimes = Constants_DelayTimes; // Use the namespace for delay times

    // Create an error message
    std::string message =  commons::MESSAGE_HEADER; // Memory usage increases dynamically at runtime (in heap memory)
    message += errorMessageBody[static_cast<size_t>(errorMessageId)];
    message += commons::MESSAGE_FOOTER;
    message += errorMessageBody[static_cast<size_t>(setupLastErrorMessageId)];
    message += commons::MESSAGE_FOOTER_END;

    // Send the error message to the serial monitor
    Serial.println(message.c_str());

    return;   // No return value for void function
}

// Send message to serial monitor
void SerialLogger::sendMessage(const char* messageString)
{
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace dateTimes = Constants_DelayTimes; // Use the namespace for delay times

    // Create an error message
    std::string message =  commons::MESSAGE_HEADER; // Memory usage increases dynamically at runtime (in heap memory)
    message += messageString;
    message += commons::MESSAGE_FOOTER;
    message += errorMessageBody[static_cast<size_t>(setupLastErrorMessageId)];
    message += commons::MESSAGE_FOOTER_END;


    // Send the message to the serial logger
    Serial.println(message.c_str());

    return; // No return value for void function
}

// Set the last message for setup
void SerialLogger::setupLastErrorMessage(ErrorMessageId errorMessageId)
{
    setupLastErrorMessageId = errorMessageId; // Store the last message ID for setup

    return; // No return value for void function
}
