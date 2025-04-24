#include "imgui_redirect_log.h"


ImGuiStreamBuffer::ImGuiStreamBuffer(char* buffer, size_t buffer_size, size_t max_log_size)
        : buffer_(buffer), buffer_size_(buffer_size), max_log_size_(max_log_size) 
{
    setp(buffer, buffer + buffer_size); // Set the buffer for the stream
}

ImGuiStreamBuffer::int_type ImGuiStreamBuffer::overflow(int_type ch) 
{
    if (ch != EOF) {
        // Convert the char to a string and append to the buffer
        if (pptr() < epptr()) {
            *pptr() = static_cast<char>(ch);
            pbump(1);
        }

        // Check if buffer is full or new line character
        if (ch == '\n' || pptr() == epptr()) 
        {
            logBuffer_ += buffer_;
            logBuffer_ += "\n";

            // Check if logBuffer_ exceeds the maximum size, and truncate the beginning
            if (logBuffer_.size() > max_log_size_) 
            {
                size_t excess = logBuffer_.size() - max_log_size_;
                logBuffer_ = logBuffer_.substr(excess);  // Keep only the latest part of the log
            }


            memset(buffer_, 0, buffer_size_);
            setp(buffer_, buffer_ + buffer_size_);  // Reset the buffer
        }
    }
    return ch;
}

std::string ImGuiStreamBuffer::GetLog() const 
{ 
    return logBuffer_; 
}

void ImGuiStreamBuffer::ClearLog() 
{
    logBuffer_.clear(); 
}