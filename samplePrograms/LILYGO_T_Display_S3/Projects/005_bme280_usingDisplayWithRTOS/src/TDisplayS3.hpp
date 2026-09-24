// TDisplayS3.hpp
#pragma once
// Include necessary components for C/C++ environment
#include <new>       // for error handling in dynamic memory allocation
// Include necessary libraries for Arduino and Arduino_GFX_Library
#include <Arduino.h>             // for Arduino framework
#include <Arduino_GFX_Library.h> // for Arduino_GFX_Library for T-Display S3

// Namespace for constants related to T-Display S3 and Arduino_GFX_Library
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

    // Pin numbers and display parameters for ST7789 LED of T-Display S3
    inline constexpr uint8_t  ST7789_LED_RST           = 5;    // Reset pin number for ST7789 LED
    inline constexpr uint8_t  ST7789_LED_DIRECTION     = 1;    // Direction: Horizontal
    inline constexpr bool     ST7789_LED_IPS           = true; // IPS,
    inline constexpr uint16_t ST7789_LED_PANEL_HEIGHT  = 170;  // Panel Height   
    inline constexpr uint16_t ST7789_LED_PANEL_WIDTH   = 320;  // Panel Width
    inline constexpr uint8_t  ST7789_LED_LEFT_OFFSET   = 35;   // Left Offset
    inline constexpr uint8_t  ST7789_LED_TOP_OFFSET    = 0;    // Top Offset
    inline constexpr uint8_t  ST7789_LED_RIGHT_OFFSET  = 35;   // Right Offset
    inline constexpr uint8_t  ST7789_LED_BOTTOM_OFFSET = 0;    // Bottom Offset

    // Color codes for ST7789 LED of T-Display S3 in RGB565 format
    inline constexpr uint16_t ST7789_COLOR_BLACK = 0x0000; // Color code for black
    inline constexpr uint16_t ST7789_COLOR_WHITE = 0xFFFF; // Color code for white
    inline constexpr uint16_t ST7789_COLOR_GRAY  = 0x8410; // Color code for gray
    inline constexpr uint16_t ST7789_COLOR_LIGHT_GRAY = 0xC618; // Color code for light gray
    inline constexpr uint16_t ST7789_COLOR_DARK_GRAY = 0x4208; // Color code for dark gray
    inline constexpr uint16_t ST7789_COLOR_MAROON = 0x8000; // Color code for maroon
    inline constexpr uint16_t ST7789_COLOR_RED   = 0xF800; // Color code for red
    inline constexpr uint16_t ST7789_COLOR_GREEN = 0x07E0; // Color code for green
    inline constexpr uint16_t ST7789_COLOR_BLUE  = 0x001F;
    inline constexpr uint16_t ST7789_COLOR_YELLOW = 0xFFE0; // Color code for yellow
    inline constexpr uint16_t ST7789_COLOR_CYAN   = 0x07FF; // Color code for cyan
    inline constexpr uint16_t ST7789_COLOR_MAGENTA = 0xF81F; // Color code for magenta
    inline constexpr uint16_t ST7789_COLOR_ORANGE = 0xFD20; // Color code for orange
}

class GFXLibraryWrapper
{
    public:
        // Type definitions for GFX library wrapper
        typedef Arduino_ESP32PAR8 BusHandle_t; // Define the bus handle type
        typedef Arduino_ST7789 DisplayHandle_t; // Define the display handle type
        typedef uint16_t Size_t; // Define the display size type
        typedef uint8_t Offset_t; // Define the display offset type
        typedef uint16_t Position_t; // Define the display position type
        typedef uint16_t Color_t; // Define the display color type

        // Definitions of constants for display handling
        // - Display Size
        static inline constexpr const Size_t& DISPLAY_WIDTH = Constants_GFXWrapper_TDisplayS3::ST7789_LED_PANEL_WIDTH; // Display width
        static inline constexpr const Size_t& DISPLAY_HEIGHT = Constants_GFXWrapper_TDisplayS3::ST7789_LED_PANEL_HEIGHT; // Display height
        //- Display Color
        static inline constexpr const Color_t& COLOR_BLACK = Constants_GFXWrapper_TDisplayS3::ST7789_COLOR_BLACK; // Color code for black
        static inline constexpr const Color_t& COLOR_WHITE = Constants_GFXWrapper_TDisplayS3::ST7789_COLOR_WHITE; // Color code for white
        static inline constexpr const Color_t& COLOR_GRAY = Constants_GFXWrapper_TDisplayS3::ST7789_COLOR_GRAY; // Color code for gray
        static inline constexpr const Color_t& COLOR_LIGHT_GRAY = Constants_GFXWrapper_TDisplayS3::ST7789_COLOR_LIGHT_GRAY; // Color code for light gray
        static inline constexpr const Color_t& COLOR_DARK_GRAY = Constants_GFXWrapper_TDisplayS3::ST7789_COLOR_DARK_GRAY; // Color code for dark gray
        static inline constexpr const Color_t& COLOR_MAROON = Constants_GFXWrapper_TDisplayS3::ST7789_COLOR_MAROON; // Color code for maroon
        static inline constexpr const Color_t& COLOR_RED = Constants_GFXWrapper_TDisplayS3::ST7789_COLOR_RED; // Color code for red
        static inline constexpr const Color_t& COLOR_GREEN = Constants_GFXWrapper_TDisplayS3::ST7789_COLOR_GREEN; // Color code for green
        static inline constexpr const Color_t& COLOR_BLUE = Constants_GFXWrapper_TDisplayS3::ST7789_COLOR_BLUE; // Color code for blue
        static inline constexpr const Color_t& COLOR_YELLOW = Constants_GFXWrapper_TDisplayS3::ST7789_COLOR_YELLOW; // Color code for yellow
        static inline constexpr const Color_t& COLOR_CYAN = Constants_GFXWrapper_TDisplayS3::ST7789_COLOR_CYAN; // Color code for cyan
        static inline constexpr const Color_t& COLOR_MAGENTA = Constants_GFXWrapper_TDisplayS3::ST7789_COLOR_MAGENTA; // Color code for magenta
        static inline constexpr const Color_t& COLOR_ORANGE = Constants_GFXWrapper_TDisplayS3::ST7789_COLOR_ORANGE; // Color code for orange
        // - Display DefaultFont Size
        static inline constexpr const uint8_t& FONT_SIZE_HEIGHT = 8; // Font height
        static inline constexpr const uint8_t& FONT_SIZE_WIDTH = 6; // Font width

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
                ST7789_LED_PANEL_HEIGHT, ST7789_LED_PANEL_WIDTH,
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