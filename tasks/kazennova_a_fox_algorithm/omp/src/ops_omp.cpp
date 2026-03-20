#include "kazennova_a_fox_algorithm/omp/include/ops_omp.hpp"

#include <atomic>
#include <cstddef>
#include <vector>

#include <omp.h>

#include "kazennova_a_fox_algorithm/common/include/common.hpp"

namespace kazennova_a_fox_algorithm {

KazennovaATestTaskOMP::KazennovaATestTaskOMP(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool KazennovaATestTaskOMP::ValidationImpl() {
  const auto &in = GetInput();

  if (in.A.data.empty() || in.B.data.empty()) {
    return false;
  }

  if (in.A.rows <= 0 || in.A.cols <= 0 || in.B.rows <= 0 || in.B.cols <= 0) {
    return false;
  }

  if (in.A.cols != in.B.rows) {
    return false;
  }

  return true;
}

bool KazennovaATestTaskOMP::PreProcessingImpl() {
  const auto &in = GetInput();

  GetOutput().rows = in.A.rows;
  GetOutput().cols = in.B.cols;
  GetOutput().data.assign(static_cast<size_t>(in.A.rows) * static_cast<size_t>(in.B.cols), 0.0);

  return true;
}

bool KazennovaATestTaskOMP::RunImpl() {
  const auto &in = GetInput();
  auto &out = GetOutput();

  const int m = in.A.rows;
  const int n = in.B.cols;
  const int k = in.A.cols;

  const auto &a = in.A.data;
  const auto &b = in.B.data;
  auto &c = out.data;

  const int num_threads = ppc::util::GetNumThreads();

  #pragma omp parallel for collapse(2) schedule(static) num_threads(num_threads)
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      double sum = 0.0;
      for (int k_idx = 0; k_idx < k; ++k_idx) {
        const size_t a_idx = (static_cast<size_t>(i) * k) + k_idx;
        const size_t b_idx = (static_cast<size_t>(k_idx) * n) + j;
        sum += a[a_idx] * b[b_idx];
      }
      c[(static_cast<size_t>(i) * n) + j] = sum;
    }
  }

  return true;
}

bool KazennovaATestTaskOMP::PostProcessingImpl() {
  return !GetOutput().data.empty();
}

}  // namespace kazennova_a_fox_algorithm