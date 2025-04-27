#pragma once
#include <torch/torch.h>
#include <vector>
#include <constant.h>
#include <cmath>
#include "global_logger.h"


torch::Tensor construct_w(const spectrum_dataset& spectrum);
torch::Tensor get_complex_transmission(const spectrum_dataset& s1, const spectrum_dataset& r1);

void set_ROI_dataset(complex_transmission_dataset& source, roi_dataset& df, const spectrum_dataset& spectrum, std::string from, std::string to);

// class ExtractIndexNetwork : public torch::nn::Module 
// {
// public:
//     ExtractIndexNetwork(cal_parameters& prams);
//     ~ExtractIndexNetwork() = default;

//     torch::Tensor forward();
    
//     torch::Tensor n1;
//     torch::Tensor k1;
//     torch::Tensor n2;
//     torch::Tensor k2;
//     torch::Tensor n3;
//     torch::Tensor k3;
//     double L;           //thickness
//     torch::Tensor targetSpectrum;
//     torch::Tensor phase_measured;
//     bool n_grad;
//     bool L_grad;
//     bool FP;
// };

// // Define shared pointer alias using the TORCH_MODULE macro
// TORCH_MODULE(ExtractIndexNetwork);

torch::Tensor unwrap(const torch::Tensor& phase, double discontinuity = M_PI);

torch::Tensor get_phase(complex_transmission_dataset& source, std::string from, std::string to);




void prepare_network_prams(roi_dataset& roi_data, cal_parameters& parameters, double L);

// Define class for ExtractIndexNetwork
class ExtractIndexNetwork : public torch::nn::Module 
{
    public:
        // Constructor, needs to initialize tensors and parameters
        ExtractIndexNetwork(cal_parameters& prams);
    
        // Destructor
        ~ExtractIndexNetwork() = default;
    
        // Forward pass function
        torch::Tensor forward();
    
        // Member variables
        torch::Tensor n1, k1, n2, k2, n3, k3;
        double L;  // thickness
        torch::Tensor targetSpectrum1, targetSpectrum2, phase_measured;
        bool n_grad, L_grad, FP;
};
    
// Define shared pointer alias using the TORCH_MODULE macro
// TORCH_MODULE(ExtractIndexNetwork);