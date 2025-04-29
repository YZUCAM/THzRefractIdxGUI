#pragma once
#include <torch/torch.h>
#include <vector>
#include "constant.h"
#include <cmath>
#include "global_logger.h"


torch::Tensor construct_w(const spectrum_dataset& spectrum);
torch::Tensor get_complex_transmission(const spectrum_dataset& s1, const spectrum_dataset& r1);

void set_ROI_dataset(std::string from, std::string to);

torch::Tensor unwrap(const torch::Tensor& phase, double discontinuity = M_PI);

void get_phase(std::string from, std::string to);

std::pair<torch::Tensor, torch::Tensor> tensor_cal_FP(const torch::Tensor& n1, const torch::Tensor& k1,
    const torch::Tensor& n2, const torch::Tensor& k2, const torch::Tensor& n3, const torch::Tensor& k3, const torch::Tensor& w, const torch::Tensor& L);

std::pair<torch::Tensor, torch::Tensor> tensor_cal_transmission_phase(const torch::Tensor& n1, const torch::Tensor& k1,
    const torch::Tensor& n2, const torch::Tensor& k2, const torch::Tensor& n3, const torch::Tensor& k3, const torch::Tensor& w, 
    const torch::Tensor& L, bool FP);

torch::Tensor tensor_cal_euclidean_dist(const torch::Tensor& t_cal, const torch::Tensor& t_m, const torch::Tensor& phase_measured, 
    const torch::Tensor& phase_compen, const torch::Tensor& w, const torch::Tensor& L, const torch::Tensor& n2, const torch::Tensor& n1);


void prepare_network_prams();
void update_network_prams(cal_parameters& parameters, torch::Tensor& opt_n, torch::Tensor& opt_k, torch::Tensor new_L);

// Define class for ExtractIndexNetwork
class ExtractIndexNetwork : public torch::nn::Module 
{
    public:
        // Constructor, needs to initialize tensors and parameters
        ExtractIndexNetwork(cal_parameters& prams, torch::Tensor& ctd, torch::Tensor& pd, torch::Tensor& w);
    
        // Destructor
        ~ExtractIndexNetwork() = default;
    
        // Forward pass function
        torch::Tensor forward();
    
        // Member variables
        torch::Tensor n1, k1, n2, k2, n3, k3, w;
        torch::Tensor L;  // thickness
        torch::Tensor targetSpectrum, phase_measured;
        bool n_grad, L_grad, FP;
};
    
// Define shared pointer alias using the TORCH_MODULE macro
// TORCH_MODULE(ExtractIndexNetwork);


std::pair<std::unordered_map<std::string, std::vector<double>>, std::vector<torch::Tensor>> 
train_step(ExtractIndexNetwork& model,torch::optim::Optimizer& optimizer, int max_epochs, torch::Device device);


void extraction_freestanding(std::string lr, std::string max_ep, std::string from, std::string to);
