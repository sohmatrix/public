// serialLogger.hpp
#pragma once

#include <Arduino.h> // for Arduino framework

class SerialLogger
{
    public:
        // Definitions of error message contents
        enum class ErrorMessageId {
            UNKNOWN_ERROR = 0,
            SERIAL_INITIALIZATION_FAILS,
            SERIAL_MUTEX_CREATION_FAILS,
            DISPLAY_MUTEX_CREATION_FAILS,
            DISPLAY_BUS_CREATION_FAILS,
            DISPLAY_DISPLAY_CREATION_FAILS,
            DISPLAY_BEGIN_FAILS,
            DISPLAY_INITIALIZATION_FAILS,
            DISPLAY_DISPLAY_FAILS,
            DISPLAY_TASK_CREATION_FAILS,
            BME280_TASK_CREATION_FAILS,
            BME280_INITIALIZATION_FAILS,
            BME280_MUTEX_CREATION_FAILS,
            SENSOR_VARS_MUTEX_CREATION_FAILS,
            // Add more error message IDs as needed
        };
        /* Keep the number and order of entries consistent with the enum definitions */
        inline static constexpr const char* errorMessageBody[] = {
            "Unknown error!",
            "Serial initialization failed!",
            "Serial mutex creation failed!",
            "Display mutex creation failed!",
            "Display bus creation failed!",
            "Display begin failed!",
            "Display initialization failed!",
            "Display display failed!",
            "Display task creation failed!",
            "BME280 task creation failed!",
            "BME280 initialization failed!",
            "BME280 mutex creation failed!",
            "Sensor Vars mutex creation failed!"
            // Add more error message bodies as needed
        };

    public:
        // Initialization for serial logger
        void initialize();

        // Send error message to serial logger
        void sendErrorMessage(ErrorMessageId errorMessageId);

        // Send message to serial monitor
        void sendMessage(char* messageString);
};




