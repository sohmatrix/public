// display.hpp
#pragma once
// Include Arduino framework for Arduino development
#include <Arduino.h> // for Arduino framework
// Include necessary header files for program constants
#include "common.hpp"

// Class for display handling
class Display
{
    private:
        // Definitions of display instance and mutex for thread safety
        inline static void* busHandleHolder; // Data bus handle for the display
        inline static void* displayHandleHolder; // Display handle for the ST7789 LED of T-Display S3
        inline static SemaphoreHandle_t displayMutex; // Mutex for display access to ensure thread safety

    public:
        // Definitions of constants for display handling
        inline static constexpr const uint8_t MESSAGE_DEFAULT_TEXT_SIZE = 3; // Text size for messages

    private:
        // Clear the display and bus instances to avoid memory leak
        void clearInstances();

    public:
        // Constructor
        Display();

        // Destructor
        ~Display();

        // Initialize the display
        bool initialize();

        // Check if the display is initialized
        bool isInitialized();

        // Display a message with sensor values on the screen
        bool displayMessageWithSensorValues();
};
