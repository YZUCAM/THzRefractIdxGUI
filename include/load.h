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
#include <string>
#include <vector>
#include <csv.h>
#include <fft.h>


extern std::vector<double> pos;
extern int skip_row;

struct spectrum_dataset 
{
    std::vector<double> times;
    std::vector<double> Tm;
    std::vector<double> freqs;
    std::vector<double> freqsTHz;
    torch::Tensor fty;
    std::vector<double> fty_abs;
};

struct complex_transmission_dataset
{
    torch::Tensor Tm1;
    torch::Tensor Tm2;
    std::vector<double> Tm1_abs;
    std::vector<double> Tm2_abs;
};

struct roi_dataset
{
    torch::Tensor roi_Tm1;
    torch::Tensor roi_Tm2;
    // torch::Tensor roi_w;
    std::vector<double> roi_Tm1_abs;
    std::vector<double> roi_Tm2_abs;
    std::vector<double> roi_freqsTHz;
};

void read_csv_columns(const std::string& filename, std::vector<double>& c1,
    std::vector<double>& c2, int skip_rows = 0);

void load_spectrum(const std::string& filename, spectrum_dataset& data);


