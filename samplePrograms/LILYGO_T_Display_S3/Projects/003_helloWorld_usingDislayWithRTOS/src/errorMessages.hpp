// === SOURCE-HEADER:BEGIN ===
/**
 * @file errorMessages.hpp
 * @author sohmatrix
 * @brief This header file contains the definitions of error message IDs and their corresponding message bodies.
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === 5d6c60dc9978c499f298b748abc489324257e64874b3dfcd216d9870775af783

#pragma once

// Definitions of error message contents
enum class ErrorMessageId
{
    UNKNOWN_ERROR = 0,
    SERIAL_INITIALIZATION_FAILS,
    SERIAL_MUTEX_CREATION_FAILS,
    MESSAGE_COUNTER_MUTEX_CREATION_FAILS,
    MESSAGE_COUNTER_INITIALIZATION_FAILS,
    MESSAGE_COUNTER_INCREMENT_FAILS,
    DISPLAY_MUTEX_CREATION_FAILS,
    DISPLAY_BUS_CREATION_FAILS,
    DISPLAY_DISPLAY_CREATION_FAILS,
    DISPLAY_CREATION_FAILS,
    DISPLAY_BEGIN_FAILS,
    DISPLAY_INITIALIZATION_FAILS,
    DISPLAY_TITLE_FAILS,
    DISPLAY_MESSAGE_FAILS,
    DISPLAY_TASK_CREATION_FAILS,
    DISPLAY_MESSAGE_TASK_CREATION_FAILS,
    DISPLAY_MESSAGE_TASK_INITIALIZATION_FAILS,
    TIME_INFO_CREATION_FAILS,
    TIME_INFO_MUTEX_CREATION_FAILS,
    // Add more error message IDs as needed
};
/* Keep the number and order of entries consistent with the enum definitions */
inline static constexpr const char *errorMessageBody[] = {
    "Unknown error!",
    "Serial initialization failed!",
    "Serial mutex creation failed!",
    "Message counter mutex creation failed!",
    "Message counter initialization failed!",
    "Message counter increment failed!",
    "Display mutex creation failed!",
    "Display bus creation failed!",
    "Display display creation failed!",
    "Display creation failed!",
    "Display begin failed!",
    "Display initialization failed!",
    "Display title failed!",
    "Display message failed!",
    "Display task creation failed!",
    "Display message task creation failed!",
    "Display message task initialization failed!",
    "Time info creation failed!",
    "Time info mutex creation failed!",
    // Add more error message bodies as needed
};
