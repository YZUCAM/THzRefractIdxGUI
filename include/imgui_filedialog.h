// some custom widget are listed here.

#pragma once

#include "ImGuiFileDialog.h"
#include <iostream>
#include <fstream>
#include <string>
#include "data_logger.h"
#include "global_logger.h"
#include "core_functions.h"



void drawFileDialogGui(); 

void CircleIndicator(bool status, const char* label = nullptr);

void ShowSaveDialog();


void SaveToFile(const std::string& filename, const complex_refractive_index& data, const roi_dataset& data2);