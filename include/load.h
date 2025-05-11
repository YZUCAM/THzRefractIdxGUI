/*
data structure for backend process are three level structure, map -> struct -> vector,tensor

struct 
spectrum_dataset 
{   std::vector<double> times;
    std::vector<double> Tm;
    std::vector<double> freqs;
    std::vector<double> freqsTHz;
    torch::Tensor fty;
    std::vector<double> fty_abs;
}

unordered map
unordered_map<std::string, spectrum_dataset>
{
    spectrum name: spectrum_dataset
}

passed by reference, to increase the overall performance and reduce lagging
*/ 

#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string>
#include <vector>
#include <csv.h>
#include "fft.h"
// #include "global_logger.h"


struct spectrum_dataset 
{
    std::vector<float> times;
    std::vector<float> Tm;
    std::vector<float> freqs;
    std::vector<float> freqsTHz;
    torch::Tensor fty;
    std::vector<float> fty_abs;
};

struct complex_transmission_dataset
{
    torch::Tensor Tm_sub;
    torch::Tensor Tm_sam;
    torch::Tensor Tm_sam_sub;
    std::vector<float> Tm_sub_abs;
    std::vector<float> Tm_sam_abs;
    std::vector<float> Tm_sam_sub_abs;
};

struct roi_dataset
{
    torch::Tensor roi_Tm_sub;      // for chip
    torch::Tensor roi_Tm_sam;
    torch::Tensor roi_Tm_sam_sub;
    torch::Tensor roi_w;
    std::vector<float> roi_Tm_sub_abs;
    std::vector<float> roi_Tm_sam_abs;
    std::vector<float> roi_Tm_sam_sub_abs;
    std::vector<float> roi_freqsTHz;
    torch::Tensor L;    // sam
    torch::Tensor L2;   // sub
};

struct cal_parameters
{
    torch::Tensor n1;
    torch::Tensor k1;
    torch::Tensor n2;
    torch::Tensor k2;
    torch::Tensor n3;
    torch::Tensor k3;
    torch::Tensor L;           //thickness
    // torch::Tensor targetSpectrum1;
    // torch::Tensor targetSpectrum2;
    // torch::Tensor phase_measured;
    bool n_grad;
    bool L_grad;
};

struct global_phase_delay
{
    torch::Tensor gpd_sub;
    torch::Tensor gpd_sam;
    torch::Tensor gpd_sam_sub;
};

struct phase_dataset
{   
    std::vector<float> measured_phase0_display;
    std::vector<float> measured_phase1_display;
    std::vector<float> measured_phase2_display;
    std::vector<float> roi_measured_phase0_display;
    std::vector<float> roi_measured_phase1_display;
    std::vector<float> roi_measured_phase2_display;
    torch::Tensor roi_measured_phase0;  // substrate
    torch::Tensor roi_measured_phase1;  // sample
    torch::Tensor roi_measured_phase2;  // sample+substrate
    float controlled_phase_delay = 0;
};

struct complex_refractive_index
{
    std::vector<float> n2;
    std::vector<float> k2;
    // float thickness;
};

struct fitting_dataset
{
    std::vector<float> T_cal_sam;
    std::vector<float> T_cal_sub; 
    std::vector<float> T_cal_sam_sub;
    std::vector<float> Phi_cal_sub;
    std::vector<float> Phi_cal_sam;
    std::vector<float> Phi_cal_sam_sub;
};

void read_csv_columns(const std::string& filename, std::vector<float>& c1,
    std::vector<float>& c2);

void load_spectrum(const std::string& filename, spectrum_dataset& data);

void load_dataset_spectrum(const std::string& filename, spectrum_dataset& data1, 
    spectrum_dataset& data2, spectrum_dataset& data3);


