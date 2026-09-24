// common.hpp
#pragma once

#include <Arduino.h> // for Arduino framework

// Common Constant definitions for the program
namespace Constants_Common
{
    // Definitions of constants for the program
    inline constexpr const char* messageHeader = "BME280: "; // Header for the message to be displayed on the screen
    inline constexpr int MESSAGE_COUNTER_MAX = 1000; // Maximum value for messageCounter before it resets
    // Definitions of constants for serial communication
    inline constexpr unsigned long SERIAL_BAUD_RATE = 115200; // Baud rate for serial communication
    // Definitions of constants for display task creation
    inline constexpr const char* DISPLAY_TASK_NAME = "DisplayTask"; // Task name for debugging
    inline constexpr uint32_t DISPLAY_TASK_STACK_SIZE = 4096; // Stack size in words for the display task
    inline constexpr uint32_t DISPLAY_TASK_PRIORITY = 1; // Task priority for the display task
    // Definitions of constants for external LED task creation
    inline constexpr const char* BME280_TASK_NAME = "BME280Task"; // Task name for debugging
    inline constexpr uint32_t BME280_TASK_STACK_SIZE = 2048; // Stack size in words for the BME280 task
    inline constexpr uint32_t BME280_TASK_PRIORITY = 1; // Task priority for the BME280 task
};

// Definitions of delay times for various operations in the program
namespace Delay_Times
{
    // Main loop delay times in milliseconds for various operations
    inline constexpr uint32_t LOOP = 1000; // Delay time in milliseconds for the loop
    // Display device delay times in milliseconds for various operations
    inline constexpr uint32_t DISPLAY_LOOP = 300; // Delay time in milliseconds for display for the diplay loop
    // BME280 device delay times in milliseconds for various operations
    inline constexpr uint32_t BME280_LOOP = 200; // Delay time in milliseconds for the BME280 loop

    // Serial Device delay times in milliseconds for various operations
    inline constexpr uint32_t SERIAL_INITIALIZE = 200; // Delay time in milliseconds for serial output initialization
    inline constexpr uint32_t SERIAL_MUTEX = 100; // Delay time in milliseconds for serial mutex acquisition
    inline constexpr uint32_t SERIAL_MUTEX_RETRY = 100; // Delay time in milliseconds for serial mutex retry

    // Display device delay times in milliseconds for various operations
    inline constexpr uint32_t DISPLAY_INITIALIZE = 200; // Delay time in milliseconds for display initialization
    inline constexpr uint32_t DISPLAY_MUTEX = 100; // Delay time in milliseconds for display mutex acquisition
    inline constexpr uint32_t DISPLAY_MUTEX_RETRY = 100; // Delay time in milliseconds for display mutex retry

    // BME280 device delay times in milliseconds for various operations
    inline constexpr uint32_t BME280_MUTEX = 100; // Delay time in milliseconds for BME280 mutex acquisition
    inline constexpr uint32_t BME280_MUTEX_RETRY = 100; // Delay time in milliseconds for BME280 mutex retry

    // Message sensor variables delay times in milliseconds for various operations
    inline constexpr uint32_t SENSOR_VARS_MUTEX = 100; // Delay time in milliseconds for sensor vars mutex acquisition
    inline constexpr uint32_t SENSOR_VARS_MUTEX_RETRY = 100; // Delay time in milliseconds for sensor vars mutex retry
};

// Definitions of global variables for the program
namespace Global_Variables
{
    // Instances for the program
    inline void* serialLoggerInstanceHolder = nullptr; // serialLogger instance for serial logging
    inline void* displayInstanceHolder = nullptr; // display instance for display handling
    inline void* displayTaskHandleHolder = nullptr; // Task handle for the display task, initialized to nullptr
    inline void* bme280InstanceHolder = nullptr; // BME280 instance for BME280 handling
    inline void* bme280TaskHandleHolder = nullptr; // Task handle for the BME280

    // State variables and its mutex for the program
    inline float sensorTemperature = 0.0F; // Temperature value from the sensor
    inline float sensorHumidity    = 0.0F; // Humidity value from the sensor
    inline float sensorPressure    = 0.0F; // Pressure value from the sensor
    inline SemaphoreHandle_t sensorVarsMutex = nullptr; // Mutex for sensor variables access to ensure thread safety
};
