// === SOURCE-HEADER:BEGIN ===
/**
 * @file main.cpp
 * @author sohmatrix
 * @brief Sample program for the T-Display S3 to send "Hello World" messages via USB CDC Serial communication
 * @branch develop
 * @updated 2026-09-24
 */
// === SOURCE-HEADER:END === fbf18a7c783cb32a325dcc5bca75379443f8a715ef775356ae2b9352f3b539a0

// This code is sample program for the T-Display S3 via USB CDC Serial communication

// Include necessary components for C/C++ environment
#include <string> // using std::string for string manipulation
// Include necessary libraries for Arduino
#include <Arduino.h>

// Global constants
inline static constexpr const unsigned long DEBUG_SERIAL_BAUD = 115200;  // default baud rate for serial communication
inline static constexpr const uint32_t INITIALIZE_SERIAL_DELAY = 100; // initial delay in milliseconds for serial port initialization
inline static constexpr const uint32_t loopDelayTime = 1000 / 1; // delay time in milliseconds between messages
inline static constexpr const char* messageHeader = "Hello World via Serial: "; // message header for serial output
inline static constexpr const size_t MESSAGE_COUNTER_INITIAL = 0; // initial value for messageCounter
inline static constexpr const size_t MESSAGE_COUNT_MAX = 1000; // maximum value for messageCounter

// Global variable
inline static size_t messageCounter = MESSAGE_COUNTER_INITIAL; // counter for the number of messages sent

// initialization
void setup()
{
    // initialize serial communication at default baud rate
    Serial.begin(DEBUG_SERIAL_BAUD);
    delay(INITIALIZE_SERIAL_DELAY); // wait for serial port to initialize

    // send start message
    std::string startMessage = messageHeader;
    startMessage += "started.";
    Serial.println(startMessage.c_str());

    // no delay needed here
}

// main loop ... without finish condition, this will run forever
void loop()
{
    // Increment messageCount and create a message
    messageCounter = (messageCounter + 1) % (MESSAGE_COUNT_MAX); // Increment and reset messageCount if it exceeds MESSAGE_COUNT_MAX
    std::string message = messageHeader + std::to_string(messageCounter);

    // send message with counter
    Serial.println(message.c_str());

    // delay for a while before sending the next message
    delay(loopDelayTime);
}
