#include "core_functions.h"

// for fast computation, considering to change all datatype to float than double?

/*
direct use ref_spectum will cause empty struct. Use spectum_container["ref"] instead. 
Reason need find out.
*/

// contains complex transmission data of ROI
roi_dataset ROI_data;

cal_parameters cal_param;

phase_dataset phase_info;

complex_refractive_index cri;


// convert tensor data to vector data for visulization
void tensor2vector_cri(const torch::Tensor& t, std::vector<float>& v)
{
    v.resize(t.size(0));
    std::memcpy(v.data(), t.data_ptr<float>(), t.numel() * sizeof(float));
}


// convert everything to tensor for vector and matrix calculation maybe simple.
torch::Tensor construct_w(const spectrum_dataset& spectrum)
{
    torch::Tensor f = torch::tensor(spectrum.freqs);
    torch::Tensor w = f * 2 * M_PI;
    return w;
}


torch::Tensor get_complex_transmission(const spectrum_dataset& s1, const spectrum_dataset& r1)
{
    torch::Tensor ret = s1.fty/r1.fty;
    return ret;
}

// this function need pass spectrum_container["ref"], ROI_data, c_t_dataset. Directly use global and mod variable.
// this function has been optimized.
void set_ROI_dataset(std::string from, std::string to)
{
    // need input window selection
    if (from == "" || to == "")
    {
        logger.Log(DataLogger::ERROR, "Please input valid number on ROI from and ROI to.");
    }
    else
    {
        int idx_from = std::stoi(from);
        int idx_to = std::stoi(to);
    
        // clear roi_dataset container data
        ROI_data.roi_freqsTHz.clear();
        ROI_data.roi_Tm1_abs.clear();
        ROI_data.roi_Tm2_abs.clear();
        // tensor part will be automatically rewrite so no need handle it manually
    
        torch::Tensor w = construct_w(spectrum_container["ref"]);
    
        if (idx_from >= 0 && idx_to <= c_t_dataset.Tm1.size(0) && idx_from <= idx_to) 
        {
            // tensor data
            ROI_data.roi_Tm1 = c_t_dataset.Tm1.slice(0, idx_from, idx_to).clone();
            ROI_data.roi_w = w.slice(0, idx_from, idx_to).clone();
    
            // freqsTHZ need reshape
            std::copy(spectrum_container["ref"].freqsTHz.begin() + idx_from, spectrum_container["ref"].freqsTHz.begin() + idx_to, std::back_inserter(ROI_data.roi_freqsTHz));
        }
        if (idx_from >= 0 && idx_to <= c_t_dataset.Tm2.size(0) && idx_from <= idx_to) 
        {
            ROI_data.roi_Tm2 = c_t_dataset.Tm2.slice(0, idx_from, idx_to).clone();
        }
        if (idx_from >= 0 && idx_to <= c_t_dataset.Tm1_abs.size() && idx_from <= idx_to)
        {
            std::copy(c_t_dataset.Tm1_abs.begin() + idx_from, c_t_dataset.Tm1_abs.begin() + idx_to, std::back_inserter(ROI_data.roi_Tm1_abs));
        }
        if (idx_from >= 0 && idx_to <= c_t_dataset.Tm2_abs.size() && idx_from <= idx_to)   
        {
            std::copy(c_t_dataset.Tm2_abs.begin() + idx_from, c_t_dataset.Tm2_abs.begin() + idx_to, std::back_inserter(ROI_data.roi_Tm2_abs));
        }
    }
    
}

// ---------------key functions--------------------- //

// manual implement unwrap function (NEED TOBE TEST) (PASS)
torch::Tensor unwrap(const torch::Tensor& phase, float discontinuity) 
{
    auto diff = phase.slice(0, 1) - phase.slice(0, 0, -1);
    diff = torch::cat({torch::zeros({1}, diff.options()), diff});  // same size
    auto correction = torch::zeros_like(phase);

    for (int64_t i = 1; i < phase.size(0); ++i) 
    {
        if (diff[i].item<float>() > discontinuity) 
        {
            correction[i] = correction[i-1] - 2 * M_PI;
        } else if (diff[i].item<float>() < -discontinuity) 
        {
            correction[i] = correction[i-1] + 2 * M_PI;
        } else 
        {
            correction[i] = correction[i-1];
        }
    }

    return phase + correction;
}

// get_phase function has been optimized direct access global variable
void get_phase(std::string from, std::string to)
{
    // slice to desire ROI
    int idx_from = std::stoi(from);
    int idx_to = std::stoi(to);

    // NEED UNWRAP FROM 0 FREQUENCE AND THEN CHOP TO DESIGER REGION
    if (c_t_dataset.Tm1.numel() == 0)
    {
        logger.Log(DataLogger::INFO, "No Tm1 data.");
    }
    else
    {
        torch::Tensor angle1 = torch::angle(c_t_dataset.Tm1.to(torch::kComplexFloat));  // convert it to double!
        torch::Tensor phase1 = unwrap(angle1);

        if (idx_from >= 0 && idx_to <= phase1.size(0) && idx_from <= idx_to) 
        {
            phase_info.roi_measured_phase1 = phase1.slice(0, idx_from, idx_to).clone();
        }
    }
    if (c_t_dataset.Tm2.numel() == 0)
    {
        logger.Log(DataLogger::INFO, "No Tm2 data.");
    }
    else
    {
        torch::Tensor angle2 = torch::angle(c_t_dataset.Tm2.to(torch::kComplexFloat));  // convert it to double!
        torch::Tensor phase2 = unwrap(angle2);

        if (idx_from >= 0 && idx_to <= phase2.size(0) && idx_from <= idx_to) 
        {
            phase_info.roi_measured_phase2 = phase2.slice(0, idx_from, idx_to).clone();
        }
    }
}
// need pay attention to the angle info and see if angle is increasing from zero. (PASS)

// Simulation part

std::pair<torch::Tensor, torch::Tensor> tensor_cal_FP(
    const torch::Tensor& n1, const torch::Tensor& k1,
    const torch::Tensor& n2, const torch::Tensor& k2,
    const torch::Tensor& n3, const torch::Tensor& k3,
    const torch::Tensor& w, const torch::Tensor& L)
{
    auto j = torch::complex(torch::tensor(0.0), torch::tensor(1.0));

    auto numerator = (n2 - n1 + j * (k2 - k1)) * (n2 - n3 + j * (k2 - k3));
    auto denominator = (n2 + n1 + j * (k2 + k1)) * (n2 + n3 + j * (k2 + k3));
    auto exp_term = torch::exp(2 * j * (n2 + j * k2) * w * L / C);

    auto fp = 1.0 / (1.0 - numerator / denominator * exp_term);
    auto fp_phase = torch::angle(fp);

    return std::pair<torch::Tensor, torch::Tensor>(fp, fp_phase);
}


std::pair<torch::Tensor, torch::Tensor> tensor_cal_transmission_phase(
    const torch::Tensor& n1, const torch::Tensor& k1,
    const torch::Tensor& n2, const torch::Tensor& k2,
    const torch::Tensor& n3, const torch::Tensor& k3,
    const torch::Tensor& w, torch::Tensor& L,
    bool FP)
{
    auto j = torch::complex(torch::tensor(0.0), torch::tensor(1.0));

    auto coeff = 2 * (n2 + j * k2) * (n1 + n3 + j * (k1 + k3)) /
                 ((n2 + n1 + j * (k2 + k1)) * (n2 + n3 + j * (k2 + k3)));

    torch::Tensor T_cal, phase_by_k;

    if (FP)
    {
        auto [fp_coeff, fp_phase] = tensor_cal_FP(n1, k1, n2, k2, n3, k3, w, L);
        T_cal = coeff * torch::exp(j * (n2 - n1 + j * (k2 - k1)) * w * L / C) * fp_coeff;
        phase_by_k = torch::angle(coeff) + fp_phase;
    }
    else
    {
        T_cal = coeff * torch::exp(j * (n2 - n1 + j * (k2 - k1)) * w * L / C);
        phase_by_k = torch::angle(coeff);
    }

    return std::pair<torch::Tensor, torch::Tensor>(T_cal, phase_by_k);
}


torch::Tensor tensor_cal_euclidean_dist(
    const torch::Tensor& t_cal, const torch::Tensor& t_m,
    const torch::Tensor& phase_measured, const torch::Tensor& phase_compen,
    const torch::Tensor& w, const torch::Tensor& L,
    const torch::Tensor& n2, const torch::Tensor& n1)
{
    auto d = torch::pow((torch::log(torch::abs(t_cal)) - torch::log(torch::abs(t_m))), 2) +
             torch::pow((phase_compen + ((n2 - n1) * w * L / C) - phase_measured), 2);
    return d;
}

// ------------------------------------------------- //


void prepare_network_prams()
{

    // torch::Tensor phase_measured;

    int size = ROI_data.roi_Tm1.size(0);

    cal_param.n1 = torch::ones({size}, torch::kFloat);
    cal_param.k1 = torch::zeros({size}, torch::kFloat);

    cal_param.n2 = torch::ones({size}, torch::kFloat);
    cal_param.k2 = torch::zeros({size}, torch::kFloat) * 0.01;

    cal_param.n3 = torch::ones({size}, torch::kFloat);
    cal_param.k3 = torch::zeros({size}, torch::kFloat);

    cal_param.L = ROI_data.L;
    cal_param.n_grad = true;
    cal_param.L_grad = false;
    cal_param.FP = true;

}

void update_network_prams(cal_parameters& parameters, torch::Tensor& opt_n, torch::Tensor& opt_k, torch::Tensor new_L)
{
    parameters.n2 = opt_n;
    parameters.k2 = opt_k;
    parameters.L = new_L;
}



ExtractIndexNetwork::ExtractIndexNetwork(cal_parameters& prams, torch::Tensor& ctd, torch::Tensor& pd, torch::Tensor& w)
    : n1(prams.n1), k1(prams.k1), n2(prams.n2), k2(prams.k2),
      n3(prams.n3), k3(prams.k3), L(prams.L), 
      targetSpectrum(ctd), phase_measured(pd), w(w),
      n_grad(prams.n_grad), L_grad(prams.L_grad), FP(prams.FP) {
    // Constructor body can be left empty as initialization happens in the initializer list
    this->n2 = register_parameter("n2", n2, n_grad);
    this->k2 = register_parameter("k2", k2, n_grad);
    this->L = register_parameter("L", L, L_grad);
}

torch::Tensor ExtractIndexNetwork::forward() 
{
    auto [T_cal_ROI, phase_extra_ROI] = tensor_cal_transmission_phase(n1, k1, n2, k2, n3, k3, w, L, FP);

    auto dist1 = tensor_cal_euclidean_dist(
        T_cal_ROI, targetSpectrum, phase_measured, phase_extra_ROI, w, L, n2, n1);

    return torch::mean(dist1) * 1e5;
}


std::pair<std::unordered_map<std::string, std::vector<float>>, std::vector<torch::Tensor>>
train_step(
    ExtractIndexNetwork& model,
    torch::optim::Optimizer& optimizer,
    int max_epochs,
    torch::Device device)
{
    float train_loss = 0.0;
    std::unordered_map<std::string, std::vector<float>> results;
    results["train_loss"] = {};

    model.to(device);

    
    for (int epoch = 0; epoch < max_epochs; ++epoch) 
    {
        torch::Tensor loss = model.forward();
        train_loss = loss.item<float>();

        optimizer.zero_grad();
        loss.backward();
        optimizer.step();

        if (epoch % 500 == 0) {
            std::cout << "Epoch " << (epoch + 1) << "/" << max_epochs  << " | Train loss: " << train_loss << std::endl;
            std::string message = std::to_string(epoch + 1) + "/" + std::to_string(max_epochs) + "|loss: " + std::to_string(train_loss);
            logger.Log(message); 
        }

        progress = (float)(epoch + 1)/max_epochs;
    }
    
    // Save final loss
    results["train_loss"].push_back(train_loss);

    std::cout << "--------------------------------------------" << std::endl;
    logger.Log("-------------------");
    std::cout << "Thickness: " 
              << (model.L.cpu().detach().item<float>() * 1e3)
              << " mm" << std::endl;

    // Return dictionary and important tensors (n2, k2, L)
    std::vector<torch::Tensor> params = {
        model.n2.cpu().detach().clone(),
        model.k2.cpu().detach().clone(),
        model.L.cpu().detach().clone()
    };
    
    return {results, params};
}



// Extraction button call back function
void extraction_freestanding(std::string lr, std::string max_ep, std::string from, std::string to)
{
    isTraining = true;

    float lr_ = std::stod(lr);
    int max_epochs = std::stoi(max_ep);
    torch::Device device(torch::kCPU);

    set_ROI_dataset(from, to);
    get_phase(from, to);
    prepare_network_prams();

    ExtractIndexNetwork extraction_model(cal_param, ROI_data.roi_Tm1, phase_info.roi_measured_phase1, ROI_data.roi_w);
    torch::optim::Adam optimizer(
        { extraction_model.n2, extraction_model.k2 }, torch::optim::AdamOptions(lr_)
    );

    std::pair<std::unordered_map<std::string, std::vector<float>>, std::vector<torch::Tensor>> ret = train_step(extraction_model, optimizer, max_epochs, device);

    torch::Tensor optimal_n2 = ret.second[0];   // n2
    torch::Tensor optimal_k2 = ret.second[1];   // k2
    torch::Tensor optimal_thickness = ret.second[2];   // L

    tensor2vector_cri(optimal_n2, cri.n2);
    tensor2vector_cri(optimal_k2, cri.k2);

    isTraining = false;

    first_load_plot = true;

    // model can run but need to check correctness.
}