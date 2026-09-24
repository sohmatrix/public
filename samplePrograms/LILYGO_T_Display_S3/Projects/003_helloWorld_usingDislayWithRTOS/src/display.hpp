// === SOURCE-HEADER:BEGIN ===
/**
 * @file display.hpp
 * @author sohmatrix
 * @brief Class for managing the display
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === 36d20c172e194ef8a2bbac295bd97a8f8e5c6c7c4c292bd6977a7276b7f4aa68

#pragma once

// Include necessary headers for the project
#include "common.hpp" // Include the header file for program constants

// Class for display handling
class Display // Class for display handling, this instance must be singleton and ensures thread-safe access through a mutex.
{
private: // private member functions
    // Clear the display and bus instances to avoid memory leak
    void clearInstances();

public: // public member functions
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
