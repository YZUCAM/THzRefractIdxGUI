// store all global variables

#pragma once
#include "data_logger.h"
#include "load.h"
// #include <fft.h>


extern std::atomic<bool> isTraining;
extern std::atomic<float> progress;
extern std::atomic<bool> stopFlag;

extern DataLogger logger;

extern bool know_thickness;
extern bool ref_selected;
extern bool chip_selected;
extern bool sam_selected;
extern bool sam_chip_selected;
extern bool first_load_plot;
extern std::string selected_file_type;
extern std::string point;
extern float induced_phase_delay;

extern std::vector<float> pos;
// extern int skip_row;
extern std::unordered_map<std::string, spectrum_dataset> spectrum_container;
extern spectrum_dataset ref_spectrum;
extern spectrum_dataset sam_spectrum;
extern spectrum_dataset sam_delay_spectrum;

// processed data
extern complex_transmission_dataset c_t_dataset;
extern roi_dataset ROI_data;

extern cal_parameters cal_param;

extern phase_dataset phase_info;

extern complex_refractive_index cri;

extern thickness_finder thickness_info;


// under 1280 x 720 main window size, the each widget size define here:
#define RIGHT_WINDOW_SIZE ImVec2(384, 720)
#define TEXT_SIZE ImVec2(220, 18)
#define BUTTON_SIZE ImVec2(118, 24)
#define RADIO_SIZE ImVec2(88, 24)
#define CIRCLE_INDICATOR_SIZE ImVec2(30, 18)
#define INPUT_SIZE ImVec2(100, 24)

// button color
// #define BUTTONCOLOR_DARK_STYLE ImVec4(0.26, 0.59, 0.98, 0.4)
// #define BUTTONHOVERCOLOR_DARK_STYLE ImVec4(0.26, 0.59, 0.98, 0.4)   // RGB 0.26*255 = 66.3, 150.45, 249.9 




