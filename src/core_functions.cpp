#include "core_functions.h"

// contains complex transmission data of ROI
roi_dataset ROI_data;

cal_parameters cal_param1;


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

void set_ROI_dataset(complex_transmission_dataset& source, roi_dataset& df, const spectrum_dataset& spectrum, std::string from, std::string to)
{
    // need input window selection
    int idx_from = std::stoi(from);
    int idx_to = std::stoi(to);

    // clear roi_dataset container data
    df.roi_freqsTHz.clear();
    df.roi_Tm1_abs.clear();
    df.roi_Tm2_abs.clear();
    // tensor part will be automatically rewrite so no need handle it manually

    torch::Tensor w = construct_w(spectrum);


    if (idx_from >= 0 && idx_to <= source.Tm1.size(0) && idx_from <= idx_to) 
    {
        // freqsTHZ need reshape
        std::copy(spectrum.freqsTHz.begin() + idx_from, spectrum.freqsTHz.begin() + idx_to, std::back_inserter(df.roi_freqsTHz));
        // tensor data
        df.roi_Tm1 = source.Tm1.slice(0, idx_from, idx_to).clone();
        df.roi_w = w.slice(0, idx_from, idx_to).clone();
    }
    if (idx_from >= 0 && idx_to <= source.Tm2.size(0) && idx_from <= idx_to) 
    {
        df.roi_Tm2 = source.Tm2.slice(0, idx_from, idx_to).clone();
    }
    if (idx_from >= 0 && idx_to <= source.Tm1_abs.size() && idx_from <= idx_to)
    {
        std::copy(source.Tm1_abs.begin() + idx_from, source.Tm1_abs.begin() + idx_to, std::back_inserter(df.roi_Tm1_abs));
    }
    if (idx_from >= 0 && idx_to <= source.Tm2_abs.size() && idx_from <= idx_to)   
    {
        std::copy(source.Tm2_abs.begin() + idx_from, source.Tm2_abs.begin() + idx_to, std::back_inserter(df.roi_Tm2_abs));
    }

}


// ExtractIndexNetwork::ExtractIndexNetwork(cal_parameters& prams)
// {
//     this->n1 = prams.n1;
//     this->k1 = prams.k1;
//     this->n2 = prams.n2;
//     this->k2 = prams.k2;
//     this->n3 = prams.n3;
//     this->k3 = prams.k3;
//     this->L = prams.L;          //thickness
//     this->targetSpectrum = prams.targetSpectrum;
//     this->phase_measured = prams.phase_measured;
//     this->n_grad = prams.n_grad;
//     this->L_grad = prams.L_grad;
//     this->FP = prams.FP;
// }

// torch::Tensor ExtractIndexNetwork::forward() {
//     // Implement forward logic here
//     return torch::Tensor(); // Placeholder for now need to apply our model!
// }


// ---------------key functions--------------------- //

// manual implement unwrap function (NEED TOBE TEST)
torch::Tensor unwrap(const torch::Tensor& phase, double discontinuity) 
{
    auto diff = phase.slice(0, 1) - phase.slice(0, 0, -1);
    diff = torch::cat({torch::zeros({1}, diff.options()), diff});  // same size
    auto correction = torch::zeros_like(phase);

    for (int64_t i = 1; i < phase.size(0); ++i) 
    {
        if (diff[i].item<double>() > discontinuity) 
        {
            correction[i] = correction[i-1] - 2 * M_PI;
        } else if (diff[i].item<double>() < -discontinuity) 
        {
            correction[i] = correction[i-1] + 2 * M_PI;
        } else 
        {
            correction[i] = correction[i-1];
        }
    }

    return phase + correction;
}

torch::Tensor get_phase(complex_transmission_dataset& source, std::string from, std::string to)
{
    // NEED UNWRAP FROM 0 FREQUENCE AND THEN CHOP TO DESIGER REGION
    torch::Tensor angle1 = torch::angle(source.Tm1);
    torch::Tensor angle2 = torch::angle(source.Tm2);
    torch::Tensor phase1 = unwrap(angle1);
    torch::Tensor phase2 = unwrap(angle2);

    // slice to desire ROI
    int idx_from = std::stoi(from);
    int idx_to = std::stoi(to);

    torch::Tensor roi_phase1 = phase1.slice(0, idx_from, idx_to).clone();
    torch::Tensor roi_phase2 = phase2.slice(0, idx_from, idx_to).clone();

    return roi_phase1;
}


// # Anchor the phase
// angle = torch.angle(Tm)
// angle2 = torch.angle(Tm2)
// phase_measured = torch.tensor(np.unwrap(angle.numpy()), dtype=torch.float32)
// phase_measured2 = torch.tensor(np.unwrap(angle2.numpy()), dtype=torch.float32)


// need pay attention to the angle info and see if angle is increasing from zero.

















// ------------------------------------------------- //


void prepare_network_prams(roi_dataset& roi_data, cal_parameters& parameters, double L)
{

    // torch::Tensor phase_measured;

    int size = roi_data.roi_Tm1.size(0);

    parameters.n1 = torch::ones({size}, torch::kDouble);
    parameters.k1 = torch::zeros({size}, torch::kDouble);

    parameters.n2 = torch::ones({size}, torch::kDouble);
    parameters.k2 = torch::zeros({size}, torch::kDouble) * 0.01;

    parameters.n3 = torch::ones({size}, torch::kDouble);
    parameters.k3 = torch::zeros({size}, torch::kDouble);

    parameters.targetSpectrum1 = roi_data.roi_Tm1;
    parameters.targetSpectrum2 = roi_data.roi_Tm2;

    parameters.L = roi_data.L;
    parameters.n_grad = true;
    parameters.L_grad = false;
    parameters.FP = true;
}

ExtractIndexNetwork::ExtractIndexNetwork(cal_parameters& prams)
    : n1(prams.n1), k1(prams.k1), n2(prams.n2), k2(prams.k2),
      n3(prams.n3), k3(prams.k3), L(prams.L), 
      targetSpectrum1(prams.targetSpectrum1), targetSpectrum2(prams.targetSpectrum2), 
      phase_measured(prams.phase_measured),
      n_grad(prams.n_grad), L_grad(prams.L_grad), FP(prams.FP) {
    // Constructor body can be left empty as initialization happens in the initializer list
}

torch::Tensor ExtractIndexNetwork::forward() 
{
    // Implement your model's forward pass here. Example:
    // This is a placeholder example, replace with your actual model logic.
    return torch::matmul(n1, k1);  // Example operation
}