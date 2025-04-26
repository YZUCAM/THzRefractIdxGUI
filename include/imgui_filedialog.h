// some custom widget are listed here.

#pragma once

#include "ImGuiFileDialog.h"
#include <iostream>
#include <string>
#include "data_logger.h"
#include "global_logger.h"




void drawFileDialogGui(); 

void CircleIndicator(bool status, const char* label = nullptr);