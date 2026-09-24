// serialLogger.cpp

// Include necessary components for C/C++ environment
#include <new>         // for error handling
#include <string>      // using STL string class for easier string manipulation
// Include necessary libraries for Arduino
#include <Arduino.h>   // for Arduino framework

#include "common.hpp"
#include "serialLogger.hpp"

// methods for serial logger class

// Initialization for serial logger
void SerialLogger::initialize()
{
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace dateTimes = Delay_Times;    // Use the namespace for delay times

    // Initialize serial communication with the specified baud rate
    Serial.begin(commons::SERIAL_BAUD_RATE);
    delay(dateTimes::SERIAL_INITIALIZE); // Delay for serial output initialization

    // No return value for void function
}

// Send error message to serial logger
void SerialLogger::sendErrorMessage(ErrorMessageId errorMessageId)
{
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace dateTimes = Delay_Times; // Use the namespace for delay times

    // Create an error message
    std::string message =  commons::messageHeader; // Memory usage increases dynamically at runtime (in heap memory)
    message += errorMessageBody[static_cast<int>(errorMessageId)];

    // Send the error message to the serial monitor
    Serial.println(message.c_str());

    // No return value for void function
}

// Send message to serial monitor
void SerialLogger::sendMessage(char* messageString)
{
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace dateTimes = Delay_Times; // Use the namespace for delay times

    // Create an error message
    std::string message =  commons ::messageHeader; // Memory usage increases dynamically at runtime (in heap memory)
    message += messageString;

    // Send the message to the serial logger
    Serial.println(message.c_str());

    // No return value for void function
}
