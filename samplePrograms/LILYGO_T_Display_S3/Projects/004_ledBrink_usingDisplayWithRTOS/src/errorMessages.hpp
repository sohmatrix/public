// === SOURCE-HEADER:BEGIN ===
/**
 * @file errorMessages.hpp
 * @author sohmatrix
 * @brief Definitions of error message IDs and their corresponding message bodies.
 * @branch develop
 * @updated 2026-09-19
 */
// === SOURCE-HEADER:END === 97d62f9a3b768957637fdb8613626ada7c6ce2a753da562ec6ab578fbbb09dcd

#pragma once

// Definitions of error message contents
enum class ErrorMessageId
{
    UNKNOWN_ERROR = 0,
    SERIAL_INITIALIZATION_FAILS,
    SERIAL_MUTEX_CREATION_FAILS,
    MESSAGE_COUNTER_MUTEX_CREATION_FAILS,
    MESSAGE_COUNTER_CREATION_FAILS,
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
    EXTERNAL_LED_CREATION_FAILS,
    EXTERNAL_LED_INITIALIZATION_FAILS,
    EXTERNAL_LED_MUTEX_CREATION_FAILS,
    EXTERNAL_LED_HEARTBEAT_FAILS,
    MESSAGE_COUNTER_TASK_CREATION_FAILS,
    MESSAGE_COUNTER_TASK_INITIALIZATION_FAILS,
    HEARTBEAT_CHECK_FAILS,
    HEARTBEAT_HEARTBEAT_FAILS,
    TIME_INFO_CREATION_FAILS,
    TIME_INFO_MUTEX_NOT_AVAILABLE,
    // Add more error message IDs as needed
};

/* Keep the number and order of entries consistent with the enum definitions */
inline static constexpr const char *errorMessageBody[] = {
    "Unknown error!",
    "Serial initialization failed!",
    "Serial mutex creation failed!",
    "Message counter mutex creation failed!",
    "Message counter creation failed!",
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
    "External LED creation failed!",
    "External LED initialization failed!",
    "External LED mutex creation failed!",
    "External LED heartbeat failed!",
    "Message counter task creation failed!",
    "Message counter task initialization failed!",
    "Heartbeat check failed!",
    "Heartbeat heartbeat failed!",
    "Time info creation failed!",
    "Time info mutex not available!",
    // Add more error message bodies as needed
};
