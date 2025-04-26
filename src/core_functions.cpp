#include "core_functions.h"


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