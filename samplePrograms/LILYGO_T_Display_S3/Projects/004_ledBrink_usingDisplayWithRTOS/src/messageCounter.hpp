// === SOURCE-HEADER:BEGIN ===
/**
 * @file messageCounter.hpp
 * @author sohmatrix
 * @brief Class for managing the message counter
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === 02104ac76b105e0f8b75f6ac38c4622f85dd0ee53c577776804beaf3dbe271e7

#pragma once

#include "common.hpp" // for global variables and constants related to message counter

// Class MessageCounter for managing the message counter
class MessageCounter
{
public: // Public interface for the message counter class
    using Counter_t = uint32_t; // Type alias for the message counter maximum value

    // Initialize the message counter and mutex
    bool initialize();

    // Check if the message counter has been initialized successfully
    bool isInitialized() const;

    // Clear the message counter to reset its value
    bool clear();
 
    // Increment the message counter
    bool increment();

    // Get the current value of the message counter
    Counter_t getValue() const;
};
