#pragma once

#include <cstdint>
#include <vector>

#include "task/include/task.hpp"
#include "titaev_m_sortirovka_betchera/common/include/common.hpp"

namespace titaev_m_sortirovka_betchera {

class TitaevSortirovkaBetcheraSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit TitaevSortirovkaBetcheraSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

  void ConvertToKeys(std::vector<uint64_t> &keys);
  void RadixSort(std::vector<uint64_t> &keys);
  void ConvertFromKeys(const std::vector<uint64_t> &keys);
  void BatcherSort();
};

}  // namespace titaev_m_sortirovka_betchera
