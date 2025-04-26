#pragma once
#include <torch/torch.h>
#include <vector>
#include <constant.h>
#include <algorithm>

torch::Tensor fft(std::vector<double> v1);

std::vector<double> linspace(double start, double end, size_t num);

std::vector<double> construct_freqs(std::vector<double> t);

std::vector<double> pos2time(std::vector<double> t);