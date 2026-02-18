#include <gtest/gtest.h>

#include "batushin_i_incr_contrast_with_lhs/common/include/common.hpp"
#include "batushin_i_incr_contrast_with_lhs/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace batushin_i_incr_contrast_with_lhs {

class BatushinIRunPerfTestThreads : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 200;
  InType input_data_{};

  void SetUp() override {
    input_data_ = kCount_;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return input_data_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(BatushinIRunPerfTestThreads, RunPerfModes) {
  ExecuteTest(GetParam());
}

namespace {

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, BatushinITestTaskSEQ, BatushinITestTaskSEQ, BatushinITestTaskSEQ,
                                BatushinITestTaskSEQ, BatushinITestTaskSEQ>(
        PPC_SETTINGS_batushin_i_incr_contrast_with_lhs);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = BatushinIRunPerfTestThreads::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, BatushinIRunPerfTestThreads, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace batushin_i_incr_contrast_with_lhs
