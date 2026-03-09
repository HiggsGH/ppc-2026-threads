#include "redkina_a_integral_simpson_seq/omp/include/ops_omp.hpp"

#include <omp.h>

#include <cmath>
#include <cstddef>
#include <functional>
#include <vector>

#include "redkina_a_integral_simpson_seq/common/include/common.hpp"

namespace redkina_a_integral_simpson_seq {

namespace {

// Вес Симпсона для индекса i при разбиении n
inline double GetWeight(int i, int n) {
  if (i == 0 || i == n) {
    return 1.0;
  }
  return (i % 2 == 1) ? 4.0 : 2.0;
}

// Рекурсивное вычисление суммы для размерностей, начиная с dim_start
double RecursiveSum(int dim_start, int dim_end, const double *a, const double *h, const int *n,
                    const std::vector<double> &point_prefix,
                    const std::function<double(const std::vector<double> &)> &func) {
  if (dim_start == dim_end) {
    // Базовый случай: все координаты определены
    return func(point_prefix);
  }
  double sum = 0.0;
  int current_dim = dim_start;
  for (int idx = 0; idx <= n[current_dim]; ++idx) {
    double x = a[current_dim] + idx * h[current_dim];
    double w = GetWeight(idx, n[current_dim]);
    std::vector<double> point = point_prefix;
    point.push_back(x);
    sum += w * RecursiveSum(dim_start + 1, dim_end, a, h, n, point, func);
  }
  return sum;
}

}  // namespace

RedkinaAIntegralSimpsonOMP::RedkinaAIntegralSimpsonOMP(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool RedkinaAIntegralSimpsonOMP::ValidationImpl() {
  const auto &in = GetInput();
  size_t dim = in.a.size();

  if (dim == 0 || in.b.size() != dim || in.n.size() != dim) {
    return false;
  }
  for (size_t i = 0; i < dim; ++i) {
    if (in.a[i] >= in.b[i]) {
      return false;
    }
    if (in.n[i] <= 0 || in.n[i] % 2 != 0) {
      return false;
    }
  }
  return static_cast<bool>(in.func);
}

bool RedkinaAIntegralSimpsonOMP::PreProcessingImpl() {
  const auto &in = GetInput();
  func_ = in.func;
  a_ = in.a;
  b_ = in.b;
  n_ = in.n;
  result_ = 0.0;
  return true;
}

bool RedkinaAIntegralSimpsonOMP::RunImpl() {
  const size_t dim = a_.size();

  // Локальные копии для использования в параллельном регионе
  std::vector<double> a = a_;
  std::vector<double> b = b_;
  std::vector<int> n = n_;
  std::vector<double> h(dim);
  for (size_t i = 0; i < dim; ++i) {
    h[i] = (b[i] - a[i]) / static_cast<double>(n[i]);
  }

  // Произведение шагов
  double h_prod = 1.0;
  for (size_t i = 0; i < dim; ++i) {
    h_prod *= h[i];
  }

  // Сумма по всем узлам
  double total_sum = 0.0;

  if (dim == 0) {
    result_ = 0.0;
    return true;
  }

  // Параллельный цикл по первому измерению (как у Овсянникова)
#pragma omp parallel for reduction(+ : total_sum) schedule(static)
  for (int i0 = 0; i0 <= n[0]; ++i0) {
    double x0 = a[0] + i0 * h[0];
    double w0 = GetWeight(i0, n[0]);
    std::vector<double> point(1, x0);
    // Рекурсивный обход остальных измерений
    double inner_sum = RecursiveSum(1, static_cast<int>(dim), a.data(), h.data(), n.data(), point, func_);
    total_sum += w0 * inner_sum;
  }

  // Знаменатель 3^dim
  double denominator = 1.0;
  for (size_t i = 0; i < dim; ++i) {
    denominator *= 3.0;
  }

  result_ = (h_prod / denominator) * total_sum;
  return true;
}

bool RedkinaAIntegralSimpsonOMP::PostProcessingImpl() {
  GetOutput() = result_;
  return true;
}

}  // namespace redkina_a_integral_simpson_seq
