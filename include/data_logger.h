#pragma once
#include "imgui.h"
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <deque>


class DataLogger 
{
public:
    enum LogLevel 
    {
        INFO,
        ERROR
    };

    // Constructor (no file path, only in-memory buffer)
    DataLogger(size_t maxLogSize = 1024 * 5);  // 5 KB default
    ~DataLogger() = default;

    void Log(LogLevel level, const std::string& message);
    void Log(const std::string& message); 
    void ClearLog();
    std::deque<std::string> GetLogBuffer() const;  // In-memory log buffer for display

private:
    void MaintainLogSize();

    std::deque<std::string> logBuffer_;  // In-memory log buffer
    size_t maxLogSize_;      // Maximum size for the in-memory buffer
};