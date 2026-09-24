// === SOURCE-HEADER:BEGIN ===
/**
 * @file ExternalLed.hpp
 * @author sohmatrix
 * @brief Class for managing an external LED
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === 6c517fe0ed8787616e3a5ca5f911ad0177db8f4ed82ccf8f1058ac7aa243f7d5

#pragma once

#include "common.hpp" // Include the header file for program constants

// Class for external LED handling, this instance must be singleton and ensures thread-safe access through a mutex.
class ExternalLed
{
    public:    
        // Destructor
        ~ExternalLed();

        // Initialize the external LED
        bool initialize();

        // Check if the external LED is initialized
        bool isInitialized();

        // Heartbeat function to toggle the state of the external LED
        bool heartbeat();
};
