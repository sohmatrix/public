// === SOURCE-HEADER:BEGIN ===
/**
 * @file externalLed.cpp
 * @author sohmatrix
 * @brief Class for managing an external LED
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === d9f8f336f8e66f90b44fcfc0831e9fb8a4f903f24d772d422bfda3265a2e25a1

#include "common.hpp"       // Include the header file for program constants
#include "externalLed.hpp"

// Anonymous namespace for internal linkage of external LED constants and variables
namespace
{
    // Definitions of constants for external LED handling
    inline static constexpr const uint8_t PIN_EXTERNAL_LED = 3 /* GPIO03 */ ; // Pin number for external LED
    inline static constexpr const uint32_t DELAY_TIME_PIN_INITIALIZE = 200; // Delay time in milliseconds for pin initialization

    // Definitions of variables for external LED handling
    inline static SemaphoreHandle_t externalLedMutex = nullptr; // Mutex for external LED access to ensure thread safety
    inline static std::atomic<bool> ledStateFlag(false); // Flag to indicate the state of the external LED (on/off) with atomic operations for thread safety

}

// Destructor
ExternalLed::~ExternalLed()
{
    // Clean up the mutex for external LED access
    if (externalLedMutex)
    {
        //-> Mutex exists

        vSemaphoreDelete(externalLedMutex); // Delete the mutex for external LED access
        externalLedMutex = nullptr;
    }

    ledStateFlag.store(false); // Reset the LED state flag to false (LED off)
}

// Initialize the external LED
bool ExternalLed::initialize()
{
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times

    // Check instance status
    if (externalLedMutex)
    {
        //-> Mutex already exists

        return true; // Exit initialize() if the external LED is already initialized or already created
    }

    // Create a mutex for external LED access
    externalLedMutex = xSemaphoreCreateMutex();
    if (externalLedMutex == nullptr)
    {
        //-> Mutex creation fails

        // Send the error message to the serial monitor
        logger.sendErrorMessage(ErrorMessageId::EXTERNAL_LED_MUTEX_CREATION_FAILS);

        return false; // Exit initialize() if mutex creation fails
    }

    // Wait for the mutex to be available
    if (xSemaphoreTake(externalLedMutex, pdMS_TO_TICKS(delayTimes::LED_MUTEX)) != pdTRUE)
    {
        //-> Mutex acquisition fails

        // Wait for a short time before retrying
        vTaskDelay(pdMS_TO_TICKS(delayTimes::LED_MUTEX_RETRY)); // Delay for a short time before retrying
    }

    // Set the pin mode for the external LED pin
    pinMode(PIN_EXTERNAL_LED, OUTPUT);
    vTaskDelay(pdMS_TO_TICKS(DELAY_TIME_PIN_INITIALIZE)); // wait for a short time before returning

    // Turn off the external LED initially
    digitalWrite(PIN_EXTERNAL_LED, LOW);

    // Release the mutex
    xSemaphoreGive(externalLedMutex);

    ledStateFlag.store(false); // Initialize the ledStateFlag to false (LED off)

    return true; // Exit initialize() after setting up the external LED
};

bool ExternalLed::isInitialized()
{
    return (externalLedMutex); // Check if externalLedMutex is not nullptr
};

// Heartbeat function to toggle the state of the external LED
bool ExternalLed::heartbeat()
{
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    namespace globalVars = Global_Variables;     // Use the namespace for global variables

    // Update the LED state flag atomically using compare_exchange_weak
    bool currentLedStateFlag = ledStateFlag.load();
    bool nextLedStateFlag;
    do
    {
        nextLedStateFlag = !currentLedStateFlag;

        // The currentLedStateFlag variable may be updated with the latest value of ledStateFlag by compare_exchange_weak if the exchange fails
    } while (!ledStateFlag.compare_exchange_weak(currentLedStateFlag, nextLedStateFlag)); // Retry until the LED state is successfully updated atomically

    // Check if the mutex for external LED access is created
    if (!externalLedMutex)
    {
        //-> Mutex retrieval fails

        // Send the error message to the serial monitor
        logger.sendErrorMessage(ErrorMessageId::EXTERNAL_LED_MUTEX_CREATION_FAILS);

        return false; // Exit heartbeat() if mutex retrieval fails
    }

    // Wait for the mutex for external LED access to be available
    if (xSemaphoreTake(externalLedMutex, pdMS_TO_TICKS(delayTimes::LED_MUTEX)) != pdTRUE)
    {
        //-> Mutex acquisition fails

        // Wait for a short time before retrying
        vTaskDelay(pdMS_TO_TICKS(delayTimes::LED_MUTEX_RETRY)); // Delay for a short time before retrying
    }

    // Apply the external LED state based on the ledStateFlag
    if (nextLedStateFlag)
    {
        //-> LED is ON

        // Turn on the external LED
        digitalWrite(PIN_EXTERNAL_LED, HIGH);
    }
    else
    {
        //-> LED is OFF

        // Turn off the external LED
        digitalWrite(PIN_EXTERNAL_LED, LOW);
    }

    // Release the mutex
    xSemaphoreGive(externalLedMutex);

    return true; // Exit heartbeat() after updating the external LED state
}
