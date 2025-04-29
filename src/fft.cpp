#include <fft.h>

torch::Tensor fft(std::vector<float> v1)
{
    torch::Tensor input = torch::tensor(v1, torch::kFloat);
    torch::Tensor rfft_result = torch::fft::rfft(input).conj();

    return rfft_result;
}

std::vector<float> linspace(float start, float end, size_t num)
{
    std::vector<float> result;
    result.reserve(num);

    if (num == 0) {return result;}
    if (num == 1) 
    {
        result.push_back(start);
        return result;
    }

    float step = (end - start) / (num - 1);
    for (size_t i = 0; i < num; ++i) 
    {
        result.push_back(start + i * step);
    }

    return result;
}

std::vector<float> construct_freqs(std::vector<float> t)
{
    int N = t.size();
    int n = N/2 + 1;

    float t_min = *std::min_element(t.begin(), t.end());
    float t_max = *std::max_element(t.begin(), t.end());
    float freq_end = N / (t_max - t_min);
    

    std::vector<float> freqs = linspace(0.0, freq_end, N);
    std::vector<float> wfreqs(freqs.begin(), freqs.begin() + n);

    return wfreqs;
}

std::vector<float> pos2time(std::vector<float> t)
{
    std::vector<float> times(t.size());
    // std::transform(t.begin(), t.end(), times.begin(), [](double x){return x * 2e-3 / C;});  // position here is mm 

    std::transform(t.begin(), t.end(), times.begin(), [](float x){return x * 1e-12;});  // temp use for Kun's data, the time is ps here need to convert to s
    
    return times;
}
