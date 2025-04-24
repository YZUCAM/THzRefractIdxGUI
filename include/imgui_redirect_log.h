#pragma once
#include "imgui.h"
#include <iostream>
#include <sstream>

class ImGuiStreamBuffer : public std::streambuf 
{
public:
    ImGuiStreamBuffer(char* buffer, size_t buffer_size, size_t max_log_size = 1024);

    int_type overflow(int_type ch) override;    
    std::string GetLog() const;
    void ClearLog();
    
private:
    char* buffer_;
    size_t buffer_size_;
    size_t max_log_size_ = 1024;
    std::string logBuffer_;

};