#pragma once
#include <torch/torch.h>
#include <vector>
#include <constant.h>
#include <algorithm>

torch::Tensor fft(std::vector<float> v1);

std::vector<float> linspace(float start, float end, size_t num);

std::vector<float> construct_freqs(std::vector<float> t);

std::vector<float> pos2time(std::vector<float> t);