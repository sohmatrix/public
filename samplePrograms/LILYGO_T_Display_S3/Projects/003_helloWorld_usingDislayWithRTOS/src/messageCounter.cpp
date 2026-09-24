// === SOURCE-HEADER:BEGIN ===
/**
 * @file messageCounter.cpp
 * @author sohmatrix
 * @brief Class for managing the message counter
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === 08e1b4d2e30bac3e3c2ef82eac3d613bcb2c16306d28589007c7553d0bdbd5de

// Include necessary headers for the message counter implementation
#include "common.hpp" // for global variables and constants related to message counter
#include "messageCounter.hpp" // for message counter class definition

// Aliases for the message counter type to simplify code usage
using Counter_t = MessageCounter::Counter_t;

namespace { // Anonymous namespace for internal constants and variables
    // Constants for the message counter class
    inline static constexpr const Counter_t MESSAGE_COUNTER_INVALID = static_cast<Counter_t>(0); // Invalid value for the message counter
    inline static constexpr const Counter_t MESSAGE_COUNTER_INITIAL = static_cast<Counter_t>(1); // Initial value for the message counter
    inline static constexpr const Counter_t MESSAGE_COUNTER_MAX = 1000; // Maximum value for messageCounter before it resets

    // Message counter variable that is protected by atomic operations to ensure thread safety
    std::atomic<Counter_t> counter{MESSAGE_COUNTER_INVALID};
}

// Initialize the message counter and mutex
bool MessageCounter::initialize()
{
    counter.store(MESSAGE_COUNTER_INITIAL); // Initialize message counter to 1, atomic operation ensures thread safety

    return true;
}

// Check if the message counter has been initialized successfully
bool MessageCounter::isInitialized() const
{
    return (counter.load() != MESSAGE_COUNTER_INVALID); // Return true if the message counter has been initialized, atomic operation ensures thread safety
}

// Clear the message counter to reset its value
bool MessageCounter::clear()
{
    counter.store(MESSAGE_COUNTER_INITIAL); // Clear the message counter, atomic operation ensures thread safety

    return true;
}

// Increment the message counter
bool MessageCounter::increment()
{
    // Increment the message counter and wrap it around if it exceeds the maximum value
    uint32_t currentMessageCounter = counter.load(); // Load the current value of the message counter
    uint32_t nextMessageCounter = MESSAGE_COUNTER_INVALID; // Declare the next message counter value outside the loop to be used in compare_exchange_weak
    do
    {
        nextMessageCounter = (currentMessageCounter % MESSAGE_COUNTER_MAX) + 1; // Calculate the next value of the message counter

        // The currentMessageCounter variable is updated with the latest value of message counter by compare_exchange_weak if the exchange fails
    } while (!counter.compare_exchange_weak(currentMessageCounter, nextMessageCounter)); // Retry until the message counter is successfully incremented, atomic operation ensures thread safety 

    return true; // Successfully incremented the message counter
}
// Get the current value of the message counter
Counter_t MessageCounter::getValue() const
{
    return(counter.load()); // Return the current value of the message counter, atomic operation ensures thread safety and prevents data races
}
