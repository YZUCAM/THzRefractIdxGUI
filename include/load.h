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
    torch::Tensor Tm0;
    torch::Tensor Tm1;
    torch::Tensor Tm2;
    std::vector<float> Tm0_abs;
    std::vector<float> Tm1_abs;
    std::vector<float> Tm2_abs;
};

struct roi_dataset
{
    torch::Tensor roi_Tm0;      // for chip
    torch::Tensor roi_Tm1;
    torch::Tensor roi_Tm2;
    torch::Tensor roi_w;
    std::vector<float> roi_Tm0_abs;
    std::vector<float> roi_Tm1_abs;
    std::vector<float> roi_Tm2_abs;
    std::vector<float> roi_freqsTHz;
    torch::Tensor L;
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
    bool FP;
};

struct phase_dataset
{
    torch::Tensor roi_measured_phase1;
    torch::Tensor roi_measured_phase2;
    float controlled_phase_delay = 0;
};

struct complex_refractive_index
{
    std::vector<float> n2;
    std::vector<float> k2;
    // float thickness;
};

struct thickness_finder
{
    std::vector<float> thickarry;
    std::vector<float> thick_error; 
};

void read_csv_columns(const std::string& filename, std::vector<float>& c1,
    std::vector<float>& c2);

void load_spectrum(const std::string& filename, spectrum_dataset& data);

void load_dataset_spectrum(const std::string& filename, spectrum_dataset& data1, 
    spectrum_dataset& data2, spectrum_dataset& data3, spectrum_dataset& data4);


