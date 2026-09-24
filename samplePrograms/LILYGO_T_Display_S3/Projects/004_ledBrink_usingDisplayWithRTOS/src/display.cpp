// === SOURCE-HEADER:BEGIN ===
/**
 * @file display.cpp
 * @author sohmatrix
 * @brief Class for managing the display
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === 07939affa0d19867c5ee8786155921e18c36dea88d663ada93cef186895e65e9

// Include necessary header files for the program
#include "common.hpp"       // Include the header file for program constants
#include "display.hpp"      // Include the header file for display class

// Include necessary header files for the GFX library wrapper
#include "TDisplayS3.hpp"   // Include the header file for T-Display S3 constants

// Aliases for the GFX library wrapper and its handle types to simplify code usage
using GFX = GFXLibraryWrapper;
using DisplayHandle_t = GFX::DisplayHandle_t;
using BusHandle_t = GFX::BusHandle_t;
using Color_t = GFX::Color_t;
using Position_t = GFX::Position_t;
using TextScale_t = GFX::TextScale_t;

namespace { // Anonymous namespace for internal constants and variables
    // Definitions of constants for display handling
    // - Constants related to the title area and its text positioning
    inline static constexpr const TextScale_t TITLE_SYSTEM_DEFAULT_TEXT_SCALE = 1; // Default scale of text size for titles
    inline static constexpr const TextScale_t TITLE_PROCESS_DEFAULT_TEXT_SCALE = 1; // Default scale of text size for process titles
    inline static constexpr const Position_t TITLE_AREA_X = 0; // Default X position for the title area
    inline static constexpr const Position_t TITLE_AREA_Y = 0; // Default Y position for the title area
    inline static constexpr const Position_t TITLE_TEXT_OFFSET_X = 4; // Default X offset for the title
    inline static constexpr const Position_t TITLE_TEXT_OFFSET_Y = 4; // Default Y offset for the title
    inline static constexpr const Position_t TITLE_SYSTEM_CURSOR_X = TITLE_AREA_X + TITLE_TEXT_OFFSET_X; // Default X position for the title cursor
    inline static constexpr const Position_t TITLE_SYSTEM_CURSOR_Y = TITLE_AREA_Y + TITLE_TEXT_OFFSET_Y; // Default Y position for the title cursor
    inline static constexpr const Position_t TITLE_PROCESS_CURSOR_X = TITLE_TEXT_OFFSET_X * 2; // Default X position for the title cursor
    inline static constexpr const Position_t TITLE_PROCESS_CURSOR_Y = TITLE_SYSTEM_CURSOR_Y + GFX::FONT_SIZE_HEIGHT * TITLE_SYSTEM_DEFAULT_TEXT_SCALE + TITLE_TEXT_OFFSET_Y; // Default Y position for the title cursor
    inline static constexpr const Position_t TITLE_AREA_WIDTH = GFX::DISPLAY_WIDTH; // Default width for the title area
    inline static constexpr const Position_t TITLE_AREA_HEIGHT = TITLE_PROCESS_CURSOR_Y + GFX::FONT_SIZE_HEIGHT * TITLE_PROCESS_DEFAULT_TEXT_SCALE + TITLE_TEXT_OFFSET_Y; // Default height for the title area
    // - Constants related to the message area and its text positioning
    inline static constexpr const TextScale_t MESSAGE_DEFAULT_TEXT_SCALE = 3; // Default scale of text size for messages
    inline static constexpr const Position_t MESSAGE_AREA_X = 0; // Default X position for the message area
    inline static constexpr const Position_t MESSAGE_AREA_Y = TITLE_AREA_HEIGHT + 1; // Default Y position for the message area
    inline static constexpr const Position_t MESSAGE_TEXT_OFFSET_X = 8; // Default X offset for the message text
    inline static constexpr const Position_t MESSAGE_TEXT_OFFSET_Y = 8; // Default Y offset for the message text
    inline static constexpr const Position_t MESSAGE_CURSOR_X = MESSAGE_AREA_X + MESSAGE_TEXT_OFFSET_X; // Default X position for the message cursor
    inline static constexpr const Position_t MESSAGE_CURSOR_Y = MESSAGE_AREA_Y + MESSAGE_TEXT_OFFSET_Y; // Default Y position for the message cursor
    inline static constexpr const Position_t MESSAGE_AREA_WIDTH = GFX::DISPLAY_WIDTH; // Default width for the message area
    inline static constexpr const Position_t MESSAGE_AREA_HEIGHT = GFX::DISPLAY_HEIGHT; // Default height for the message area

    // Definitions of display instance and mutex for thread safety
    inline static void *busHandleHolder = nullptr;          // Data bus handle for the display
    inline static void *displayHandleHolder = nullptr;      // Display handle for the ST7789 LED of T-Display S3
    inline static SemaphoreHandle_t displayMutex = nullptr; // Mutex for display access to ensure thread safety
}

// Clear the display and bus instances to avoid memory leak
void Display::clearInstances()
{
    // Delete the display instance if it was created
    if (displayHandleHolder) {
        //-> display instance deletion

        // Delete the display instance to free memory
        GFX::deleteDisplayHandle((DisplayHandle_t*) displayHandleHolder); // Release instance, ignore failure
        displayHandleHolder = nullptr; // Reset the pointer to nullptr
    }

    // Delete the bus instance if it was created
    if (busHandleHolder) {
        //-> bus instance deletion

        // Delete the bus instance to free memory
        GFX::deleteBusHandle((BusHandle_t*) busHandleHolder); // Release instance, ignore failure
        busHandleHolder = nullptr; // Reset the pointer to nullptr
    }

        // Delete the mutex for display access if it was created
    if (displayMutex) {
        //-> mutex deletion

        // Delete the mutex to free memory
        vSemaphoreDelete(displayMutex); // Delete the mutex if it was created
        displayMutex = nullptr; // Reset the mutex pointer to nullptr
    }
}

// Destructor
Display::~Display()
{
    // Clear the display and bus instances to avoid memory leak
    clearInstances();
}

// Initialize the display
bool Display::initialize()
{
    // namespaces for program constants, delay times, and global variables  
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    namespace globalVars = Global_Variables; // Use the namespace for global variables

    if (displayHandleHolder || busHandleHolder || displayMutex)
    {
        //-> Existing instances detected

        // Send a message to the serial monitor indicating that existing instances were detected
        logger.setupLastErrorMessage(ErrorMessageId::DISPLAY_INITIALIZATION_FAILS);
        
        return false; // Return false to indicate initialization failure
    }
    
    // Create a mutex for display access
    displayMutex = xSemaphoreCreateMutex();
    if (!displayMutex)
    {
        //-> Mutex creation fails

        // Send the error message to the serial monitor
        logger.setupLastErrorMessage(ErrorMessageId::DISPLAY_MUTEX_CREATION_FAILS);

        return false; // Return false to indicate initialization failure
    }

    BusHandle_t* busHandle = nullptr; // Initialize bus handle to nullptr
    DisplayHandle_t* displayHandle = nullptr; // Initialize display handle to nullptr

    // Wait for the mutex to be available
    while (xSemaphoreTake(displayMutex, pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX)) != pdTRUE)
    {
        //-> Mutex acquisition fails

        vTaskDelay(pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX_RETRY)); // Delay for a short time before retrying
    }

    GFX::powerUp(); // Power up the display

    // Create a data bus instance and display instance of Arduino_GFX_Library
    bool breakLoopFlag = false; // Flag to control breaking out of the loop
    do {
        busHandle = GFX::createBusHandle();
        if (!busHandle)
        {
            //-> Bus handle creation fails

            // Send the error message to the serial monitor
            logger.setupLastErrorMessage(ErrorMessageId::DISPLAY_BUS_CREATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Break out of the do-while loop
        }

        // Create a display instance of Arduino_GFX_Library
        displayHandle = GFX::createDisplayHandle(busHandle);
        if (!displayHandle)
        {
            //-> Display handle creation fails

            // Send the error message to the serial monitor
            logger.setupLastErrorMessage(ErrorMessageId::DISPLAY_DISPLAY_CREATION_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Break out of the do-while loop
        }

        // Initialize the display
        if (!displayHandle->begin())
        {
            //-> Display initialization fails

            // Send the error message to the serial monitor
            logger.setupLastErrorMessage(ErrorMessageId::DISPLAY_BEGIN_FAILS);

            breakLoopFlag = true; // Set the flag to break out of the loop
            break; // Break out of the do-while loop
        }

        vTaskDelay(pdMS_TO_TICKS(delayTimes::DISPLAY_INITIALIZE)); // wait for a short time before returning

    } while (false); // End of single iteration loop for error handling

    // Release the mutex
    xSemaphoreGive(displayMutex);

    // Check if any error occurred during the initialization loop
    if (breakLoopFlag)
    {
        //-> An error occurred during the display initialization loop

        clearInstances(); // Clean up the instances to avoid memory leak

        return false; // Return false to indicate initialization failure
    }

    //　Store the bus and display instances in global variables for later use
    busHandleHolder = (void*) busHandle; // Store the bus instance in global variables
    displayHandleHolder = (void*) displayHandle; // Store the display instance in global variables

    // Clear the display after initialization
    displayClear();

    return true; // Return true to indicate successful initialization
}

// Check if the display is initialized
bool Display::isInitialized()
{
    return (busHandleHolder && displayHandleHolder && displayMutex); // Check if busInstance, displayInstance, and displayMutex are not nullptr
}

bool Display::displayClear()
{
    // namespace alias for global variables
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    // Retrieve the display instance from the global variable
    DisplayHandle_t* displayHandle = (DisplayHandle_t*) displayHandleHolder;

    // Check if the display is initialized
    if (!(isInitialized()))
    {
        //-> Display is not initialized

        // Send the error message to the serial monitor
        logger.sendErrorMessage(ErrorMessageId::DISPLAY_INITIALIZATION_FAILS);

        return false; // Return false if the display is not initialized
    }

    // Wait for the mutex to be available
    while (xSemaphoreTake(displayMutex, pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX)) != pdTRUE)
    {
        //-> Mutex acquisition fails

        vTaskDelay(pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX_RETRY)); // Delay for a short time before retrying
    }

    // Clear the display with black color
    displayHandle->fillScreen(GFXLibraryWrapper::COLOR_BLACK);

    // Release the mutex after clearing the display
    xSemaphoreGive(displayMutex);

    return true; // Return true to indicate successful clearing
}

// Display the title message on the screen
bool Display::displayTitle(const char* system, const char* process)
{
    // Use the namespace for program constants, delay times, and global variables
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    namespace globalVars = Global_Variables; // Use the namespace for global variables
    // Retrieve the display instance from the global variable
    DisplayHandle_t* displayHandle = (DisplayHandle_t*) displayHandleHolder;

    // Check if the display is initialized
    if (!(isInitialized()))
    {
        //-> Display initialization fails

        // Send the error message to the serial monitor
        logger.sendErrorMessage(ErrorMessageId::DISPLAY_INITIALIZATION_FAILS);

        return false; // Return false to indicate failure
    }

    // Wait for the mutex to be available
    while (xSemaphoreTake(displayMutex, pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX)) != pdTRUE)
    {
        //-> Mutex acquisition fails

        vTaskDelay(pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX_RETRY)); // Delay for a short time before retrying
    }

    // Clear the display area with white color
    displayHandle->fillRect(TITLE_AREA_X, TITLE_AREA_Y, TITLE_AREA_WIDTH, TITLE_AREA_HEIGHT, GFX::COLOR_WHITE);

    // Set the cursor position and text size and color for the system text and display it
    displayHandle->setTextColor(GFX::COLOR_BLUE);
    displayHandle->setTextSize(TITLE_SYSTEM_DEFAULT_TEXT_SCALE);
    displayHandle->setCursor(TITLE_SYSTEM_CURSOR_X, TITLE_SYSTEM_CURSOR_Y);
    displayHandle->print(system);

    // Set the cursor position and text size and color for the process text and display it
    displayHandle->setTextColor(GFX::COLOR_RED);
    displayHandle->setTextSize(TITLE_PROCESS_DEFAULT_TEXT_SCALE);
    displayHandle->setCursor(TITLE_PROCESS_CURSOR_X, TITLE_PROCESS_CURSOR_Y);
    displayHandle->print(process);

    // Release the mutex
    xSemaphoreGive(displayMutex);

    return true; // Return true to indicate successful display of the title
}

// Display a message on the screen
bool Display::displayMessage(const char* message)
{
    // Use the namespace for program constants, delay times, and global variables
    namespace commons = Constants_Common; // Use the namespace for program constants
    namespace delayTimes = Constants_DelayTimes; // Use the namespace for delay times
    namespace globalVars = Global_Variables; // Use the namespace for global variables
    // Retrieve the display instance from the global variable
    DisplayHandle_t* displayHandle = (DisplayHandle_t*) displayHandleHolder;

    // Check if the display is initialized
    if (!(isInitialized()))
    {
        //-> Display initialization fails

        // Send the error message to the serial monitor
        logger.sendErrorMessage(ErrorMessageId::DISPLAY_INITIALIZATION_FAILS);

        return false; // Return false to indicate failure
    }

    // Wait for the mutex to be available
    while (xSemaphoreTake(displayMutex, pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX)) != pdTRUE)
    {
        //-> Mutex acquisition fails

        vTaskDelay(pdMS_TO_TICKS(delayTimes::DISPLAY_MUTEX_RETRY)); // Delay for a short time before retrying
    }

    // Generate a random text color for the message
    Color_t textColor = random(GFX::COLOR_BLACK, GFX::COLOR_WHITE); // Random color for text

    // Clear the display with black color
    displayHandle->fillRect(MESSAGE_AREA_X, MESSAGE_AREA_Y, MESSAGE_AREA_WIDTH, MESSAGE_AREA_HEIGHT, GFX::COLOR_BLACK);
 
    // Set the text color, size, and cursor position for the message
    displayHandle->setTextColor(textColor);
    displayHandle->setTextSize(MESSAGE_DEFAULT_TEXT_SCALE);
    displayHandle->setCursor(MESSAGE_CURSOR_X, MESSAGE_CURSOR_Y);

    // -Display the message on the screen
    displayHandle->print(message);

    // Release the mutex
    xSemaphoreGive(displayMutex);

    return true;
}
