// TDisplayS3.hpp
#pragma once

#include <Arduino.h>             // for Arduino framework
#include <Arduino_GFX_Library.h> // for Arduino_GFX_Library for T-Display S3

#define BusHandle_t Arduino_ESP32PAR8 // Define the bus handle type for ESP32 parallel 8-bit
#define DisplayHandle_t Arduino_ST7789 // Define the display handle type for ST7789 LED of T-Display S3

namespace Constants_GFXWrapper_TDisplayS3
{
    // Definitions of pin numbers and display parameters for T-Display S3
    inline constexpr uint32_t DELAY_TIME_PIN_INITIALIZE = 200; // Delay time in milliseconds for pin initialization
    inline constexpr uint8_t  PIN_LCD_POWER = 15; // Power pin number for LCD
    inline constexpr uint8_t  PIN_LCD_BACK_LIGHT = 38;    // Backlight pin number for LCD

    // Definitions for Arduino_GFX_Library for T-Display S3 and ESP32 parallel 8-bit bus
    inline constexpr uint8_t  ESP32_8PAR_BUS_DC = 7;  // GPIO07 -> Bus Data/Command
    inline constexpr uint8_t  ESP32_8PAR_BUS_CS = 6;  // GPIO06 -> Bus Chip Select
    inline constexpr uint8_t  ESP32_8PAR_BUS_WR = 8;  // GPIO08 -> Bus Write
    inline constexpr uint8_t  ESP32_8PAR_BUS_RD = 9;  // GPIO09 -> Bus Read
    inline constexpr uint8_t  ESP32_8PAR_BUS_D0 = 39; // GPIO39 -> Bus D0
    inline constexpr uint8_t  ESP32_8PAR_BUS_D1 = 40; // GPIO40 -> Bus D1
    inline constexpr uint8_t  ESP32_8PAR_BUS_D2 = 41; // GPIO41 -> Bus D2
    inline constexpr uint8_t  ESP32_8PAR_BUS_D3 = 42; // GPIO42 -> Bus D3
    inline constexpr uint8_t  ESP32_8PAR_BUS_D4 = 45; // GPIO45 -> Bus D4
    inline constexpr uint8_t  ESP32_8PAR_BUS_D5 = 46; // GPIO46 -> Bus D5
    inline constexpr uint8_t  ESP32_8PAR_BUS_D6 = 47; // GPIO47 -> Bus D6
    inline constexpr uint8_t  ESP32_8PAR_BUS_D7 = 48; // GPIO48 -> Bus D7

    // Definitions for ST7789 LED of T-Display S3
    inline constexpr uint8_t  ST7789_LED_RST           = 5;    // Reset pin number for ST7789 LED
    inline constexpr uint8_t  ST7789_LED_DIRECTION     = 1;    // Direction: Horizontal
    inline constexpr bool     ST7789_LED_IPS           = true; // IPS,
    inline constexpr uint16_t ST7789_LED_PANEL_WIDTH   = 170;  // Panel Width
    inline constexpr uint16_t ST7789_LED_PANEL_HEIGHT  = 320;  // Panel Height
    inline constexpr uint8_t  ST7789_LED_LEFT_OFFSET   = 35;   // Left Offset
    inline constexpr uint8_t  ST7789_LED_TOP_OFFSET    = 0;    // Top Offset
    inline constexpr uint8_t  ST7789_LED_RIGHT_OFFSET  = 35;   // Right Offset
    inline constexpr uint8_t  ST7789_LED_BOTTOM_OFFSET = 0;    // Bottom Offset

    // Color codes for Arduino_GFX_Library
    inline constexpr uint16_t ST7789_COLOR_BLACK = 0x0000; // Color code for black
    inline constexpr uint16_t ST7789_COLOR_WHITE = 0xFFFF; // Color code for white
}

class GFXLibraryWrapper
{
    public:
        // Definitions of constants for display handling
        static inline constexpr const uint16_t& DISPLAY_WIDTH = Constants_GFXWrapper_TDisplayS3::ST7789_LED_PANEL_WIDTH; // Display width
        static inline constexpr const uint16_t& DISPLAY_HEIGHT = Constants_GFXWrapper_TDisplayS3::ST7789_LED_PANEL_HEIGHT; // Display height
        static inline constexpr const uint8_t& DISPLAY_LEFT_OFFSET = Constants_GFXWrapper_TDisplayS3::ST7789_LED_LEFT_OFFSET; // Display left offset
        static inline constexpr const uint8_t& DISPLAY_TOP_OFFSET = Constants_GFXWrapper_TDisplayS3::ST7789_LED_TOP_OFFSET; // Display top offset
        static inline constexpr const uint8_t& DISPLAY_RIGHT_OFFSET = Constants_GFXWrapper_TDisplayS3::ST7789_LED_RIGHT_OFFSET; // Display right offset
        static inline constexpr const uint8_t& DISPLAY_BOTTOM_OFFSET = Constants_GFXWrapper_TDisplayS3::ST7789_LED_BOTTOM_OFFSET; // Display bottom offset
        static inline constexpr const uint16_t& COLOR_BLACK = Constants_GFXWrapper_TDisplayS3::ST7789_COLOR_BLACK; // Color code for black
        static inline constexpr const uint16_t& COLOR_WHITE = Constants_GFXWrapper_TDisplayS3::ST7789_COLOR_WHITE; // Color code for white

        public:
        // Power up the T-Display S3 by setting up the LCD power pin and backlight pin
        static void powerUp()
        {
            using namespace Constants_GFXWrapper_TDisplayS3; // Use the namespace for T-Display S3

            // LCD power pin and backlight pin setup
            pinMode(PIN_LCD_POWER, OUTPUT);
            pinMode(PIN_LCD_BACK_LIGHT, OUTPUT);
            vTaskDelay(pdMS_TO_TICKS(DELAY_TIME_PIN_INITIALIZE)); // wait for power stabilization

            // LCD backlight on
            digitalWrite(PIN_LCD_BACK_LIGHT, HIGH);
        };

        // Create a data bus instance of Arduino_GFX_Library for T-Display S3
        static BusHandle_t* createBusHandle()
        {
            using namespace Constants_GFXWrapper_TDisplayS3; // Use the namespace for T-Display S3

            // Create a data bus instance of Arduino_GFX_Library for T-Display S3
            BusHandle_t* busHandle = new BusHandle_t(
                ESP32_8PAR_BUS_DC, ESP32_8PAR_BUS_CS, ESP32_8PAR_BUS_WR, ESP32_8PAR_BUS_RD,
                ESP32_8PAR_BUS_D0, ESP32_8PAR_BUS_D1, ESP32_8PAR_BUS_D2, ESP32_8PAR_BUS_D3,
                ESP32_8PAR_BUS_D4, ESP32_8PAR_BUS_D5, ESP32_8PAR_BUS_D6, ESP32_8PAR_BUS_D7);

            // Return the created bus handle
            return busHandle;
        };

        // Delete the data bus instance of Arduino_GFX_Library for T-Display S3
        static void deleteBusHandle(BusHandle_t* busHandle)
        {
            // Check if the bus handle is not nullptr before deleting
            if (busHandle)
            {
                //-> bus handle is not nullptr, delete it

                delete busHandle; // Release instance, ignore failure
            }
        };

        // Create a display instance of Arduino_GFX_Library for ST7789 LED of T-Display S3
        static DisplayHandle_t* createDisplayHandle(BusHandle_t* busHandle)
        {
            using namespace Constants_GFXWrapper_TDisplayS3; // Use the namespace for T-Display S3

            // Create a display instance of Arduino_GFX_Library for ST7789 LED of T-Display S3
            DisplayHandle_t* displayHandle = new DisplayHandle_t(
                busHandle, ST7789_LED_RST, ST7789_LED_DIRECTION, ST7789_LED_IPS,
                ST7789_LED_PANEL_WIDTH, ST7789_LED_PANEL_HEIGHT,
                ST7789_LED_LEFT_OFFSET, ST7789_LED_TOP_OFFSET, ST7789_LED_RIGHT_OFFSET, ST7789_LED_BOTTOM_OFFSET);

            // Return the created display handle
            return displayHandle;
        };

        // Delete the display instance of Arduino_GFX_Library for ST7789 LED of T-Display S3
        static void deleteDisplayHandle(DisplayHandle_t* displayHandle)
        {
            // Check if the display handle is not nullptr before deleting
            if (displayHandle)
            {
                //-> display handle is not nullptr, delete it

                delete displayHandle; // Release instance, ignore failure
            }
        };

};