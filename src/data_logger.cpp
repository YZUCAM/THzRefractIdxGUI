#include "data_logger.h"

DataLogger::DataLogger(size_t maxLogSize)
    : maxLogSize_(maxLogSize) {
    // Initialize the log buffer
}

void DataLogger::Log(LogLevel level, const std::string& message) 
{
    std::ostringstream logStream;

    // // Add timestamp to the log
    // auto now = std::time(nullptr);
    // auto tm = *std::localtime(&now);
    // logStream << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " ";

    // Add log level
    if (level == INFO) {
        logStream << "[INFO] ";
    } else if (level == ERROR) {
        logStream << "[ERROR] ";
    }

    // Add the actual message
    logStream << message << std::endl;

    // Update in-memory log buffer
    logBuffer_.push_back(logStream.str());

    // Maintain log size (if necessary)
    MaintainLogSize();
}

void DataLogger::MaintainLogSize() 
{
    if (logBuffer_.size() > maxLogSize_) {
        // Trim the log buffer to the last maxLogSize_ bytes
        logBuffer_.pop_front();
    }
}

void DataLogger::ClearLog() 
{
    logBuffer_.clear();
}

std::deque<std::string> DataLogger::GetLogBuffer() const 
{
    return logBuffer_;
}