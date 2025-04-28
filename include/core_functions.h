#pragma once
#include <torch/torch.h>
#include <vector>
#include <constant.h>
#include <cmath>
#include "global_logger.h"


torch::Tensor construct_w(const spectrum_dataset& spectrum);
torch::Tensor get_complex_transmission(const spectrum_dataset& s1, const spectrum_dataset& r1);

void set_ROI_dataset(complex_transmission_dataset& source, roi_dataset& df, const spectrum_dataset& spectrum, std::string from, std::string to);

torch::Tensor unwrap(const torch::Tensor& phase, double discontinuity = M_PI);

void get_phase(complex_transmission_dataset& source, std::string from, std::string to, phase_dataset& phase_info);

void prepare_network_prams(roi_dataset& roi_data, cal_parameters& parameters, double L);
void update_network_prams(cal_parameters& parameters, torch::Tensor& opt_n, torch::Tensor& opt_k, double new_L);

// Define class for ExtractIndexNetwork
class ExtractIndexNetwork : public torch::nn::Module 
{
    public:
        // Constructor, needs to initialize tensors and parameters
        ExtractIndexNetwork(cal_parameters& prams, torch::Tensor& ctd, torch::Tensor& pd);
    
        // Destructor
        ~ExtractIndexNetwork() = default;
    
        // Forward pass function
        torch::Tensor forward();
    
        // Member variables
        torch::Tensor n1, k1, n2, k2, n3, k3;
        double L;  // thickness
        torch::Tensor targetSpectrum, phase_measured;
        bool n_grad, L_grad, FP;
};
    
// Define shared pointer alias using the TORCH_MODULE macro
// TORCH_MODULE(ExtractIndexNetwork);