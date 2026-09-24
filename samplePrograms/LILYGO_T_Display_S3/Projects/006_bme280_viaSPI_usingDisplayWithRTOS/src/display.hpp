// display.hpp
#pragma once

#include <Arduino.h> // for Arduino framework

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
        inline static constexpr const uint8_t MESSAGE_TEXT_SIZE = 3; // Text size for messages
        inline static constexpr const uint8_t MESSAGE_CURSOR_X = 5; // X coordinate for message cursor
        inline static constexpr const uint8_t MESSAGE_CURSOR_Y = 70; // Y coordinate for message cursor

    public:
        // Constructor
        Display();

        // Destructor
        ~Display();

        // Initialize the display
        bool initialize();

        // Check if the display is initialized
        bool isInitialized();

        // Display "Hello World" message with counter on the screen
        bool displayHelloWorldWithCounter();
};
