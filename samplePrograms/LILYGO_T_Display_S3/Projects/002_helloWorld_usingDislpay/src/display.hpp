// === SOURCE-HEADER:BEGIN ===
/**
 * @file display.hpp
 * @author sohmatrix
 * @brief Display handling definitions for the program
 * @branch develop
 * @updated 2026-09-24
 */
// === SOURCE-HEADER:END === 293e93ed8df1c2c2c1a26eca42ec1f94e31367b494855acfe941c99d7f95bf87

// display.hpp
#pragma once

// Include necessary headers for the project
#include "common.hpp" // Include the header file for program constants

// Class for display handling
class Display
{
private: // private member variables
    // Definitions of display instance and mutex for thread safety
    inline static void *busHandleHolder;          // Data bus handle for the display
    inline static void *displayHandleHolder;      // Display handle for the ST7789 LED of T-Display S3

private: // private member functions
    // Clear the display and bus instances to avoid memory leak
    void clearInstances();

public: // public member functions
    // Constructor
    Display();

    // Destructor
    ~Display();

    // Initialize the display
    bool initialize();

    // Check if the display is initialized
    bool isInitialized();

    // Clear the display
    bool displayClear();

    // Display titlemessage on the screen
    bool displayTitle(const char *system, const char *process);

    // Display message on the screen
    bool displayMessage(const char *message);
};
