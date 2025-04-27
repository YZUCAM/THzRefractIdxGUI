#include "core_functions.h"

// contains complex transmission data of ROI
roi_dataset ROI_data;

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

void set_ROI_dataset(complex_transmission_dataset& source, roi_dataset& df, std::string from, std::string to, std::vector<double> freqsTHz)
{
    // need input window selection
    int idx_from = std::stoi(from);
    int idx_to = std::stoi(to);

    // clear roi_dataset container data
    df.roi_freqsTHz.clear();
    df.roi_Tm1_abs.clear();
    df.roi_Tm2_abs.clear();
    // tensor part will be automatically rewrite so no need handle it manually

    if (idx_from >= 0 && idx_to <= source.Tm1.size(0) && idx_from <= idx_to) 
    {
        // freqsTHZ need reshape
        std::copy(freqsTHz.begin() + idx_from, freqsTHz.begin() + idx_to, std::back_inserter(df.roi_freqsTHz));
        // tensor data
        df.roi_Tm1 = source.Tm1.slice(0, idx_from, idx_to).clone();
        df.roi_Tm1 = source.Tm1.slice(0, idx_from, idx_to).clone();
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