#pragma once
#include <torch/torch.h>
#include <vector>
#include <constant.h>
#include <cmath>
#include "global_logger.h"


torch::Tensor construct_w(const spectrum_dataset& spectrum);
torch::Tensor get_complex_transmission(const spectrum_dataset& s1, const spectrum_dataset& r1);