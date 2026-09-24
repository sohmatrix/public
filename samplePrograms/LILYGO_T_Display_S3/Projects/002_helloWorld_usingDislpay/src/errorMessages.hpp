// === SOURCE-HEADER:BEGIN ===
/**
 * @file errorMessages.hpp
 * @author sohmatrix
 * @brief Error message definitions for the program
 * @branch develop
 * @updated 2026-09-24
 */
// === SOURCE-HEADER:END === 7233f1686e27567de8b76ce7ee5a2ed0a3a15f962e157ce4f6ad901be9fbf235


// Definitions of error message contents
enum class ErrorMessageId
{
    UNKNOWN_ERROR = 0,
    SERIAL_INITIALIZATION_FAILS,
    DISPLAY_BUS_CREATION_FAILS,
    DISPLAY_CREATION_FAILS,
    DISPLAY_DISPLAY_CREATION_FAILS,
    DISPLAY_BEGIN_FAILS,
    DISPLAY_INITIALIZATION_FAILS,
    DISPLAY_DISPLAY_FAILS,
    DISPLAY_TITLE_FAILS,
    DISPLAY_MESSAGE_FAILS,
    DISPLAY_TASK_CREATION_FAILS,
    MESSAGE_COUNTER_MUTEX_CREATION_FAILS
    // Add more error message IDs as needed
};
/* Keep the number and order of entries consistent with the enum definitions */
inline static constexpr const char *errorMessageBody[] = {
    "Unknown error!",
    "Serial initialization failed!",
    "Display bus creation failed!",
    "Display creation failed!",
    "Display display creation failed!",
    "Display begin failed!",
    "Display initialization failed!",
    "Display display failed!",
    "Display title failed!",
    "Display message failed!",
    "Display task creation failed!",
    "Message counter mutex creation failed!"
    // Add more error message bodies as needed
};
