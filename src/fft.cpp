#include <fft.h>

torch::Tensor fft(std::vector<double> v1)
{
    torch::Tensor input = torch::tensor(v1, torch::kFloat);
    torch::Tensor rfft_result = torch::fft::rfft(input);

    return rfft_result;
}

std::vector<double> linspace(double start, double end, size_t num)
{
    std::vector<double> result;
    result.reserve(num);

    if (num == 0) {return result;}
    if (num == 1) 
    {
        result.push_back(start);
        return result;
    }

    double step = (end - start) / (num - 1);
    for (size_t i = 0; i < num; ++i) 
    {
        result.push_back(start + i * step);
    }

    return result;
}

std::vector<double> construct_freqs(std::vector<double> t)
{
    int N = t.size();
    int n = N/2 + 1;

    double t_min = *std::min_element(t.begin(), t.end());
    double t_max = *std::max_element(t.begin(), t.end());
    double freq_end = N / (t_max - t_min);
    

    std::vector<double> freqs = linspace(0.0, freq_end, N);
    std::vector<double> wfreqs(freqs.begin(), freqs.begin() + n);

    return wfreqs;
}

std::vector<double> pos2time(std::vector<double> t)
{
    std::vector<double> times(t.size());
    std::transform(t.begin(), t.end(), times.begin(), [](double x){return x * 2e-3 / C;});
    
    return times;
}
