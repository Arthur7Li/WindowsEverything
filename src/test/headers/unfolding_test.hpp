#pragma once

#include <conversion.hpp>
#include <falgo.hpp>
#include <parse.hpp>
#include <shooting_vectors.hpp>
#include <unfolding.hpp>
#include <utils.hpp>

// abdul 27/07/2026 [flatten generated provenance into a restricted-input fixture without changing its deterministic order]
static std::vector<LeftRight> flatten_left_rights(const CurvesLR& curves) {
    std::vector<LeftRight> result;
    for (const auto& entry : curves.first) {
        result.insert(result.end(), entry.second.begin(), entry.second.end());
    }
    for (const auto& entry : curves.second) {
        result.insert(result.end(), entry.second.begin(), entry.second.end());
    }
    return result;
}

BOOST_AUTO_TEST_CASE(test_restricted_curve_merge_keeps_duplicate_keys_across_workers) {
    // abdul 27/07/2026 [split duplicate equation keys across worker chunks to reproduce map insertion data loss]
    const auto code_sequence = parse_code_sequence("1 3 3");
    const auto code_angles = code_sequence.angles(XYZ::X, XYZ::Y);
    const auto code_angles_pi = falgo::transform(code_angles, xyz_to_xypi);
    const Unfolding unfolding{code_sequence.numbers(), code_angles};
    const auto shooting_vector = shooting_vector_open(code_sequence, code_angles_pi);
    const auto all_curves = unfolding.generate_curves_lr(
        shooting_vector.first, shooting_vector.second);

    auto restricted = flatten_left_rights(all_curves);
    BOOST_REQUIRE(!restricted.empty());
    const auto duplicate_copy = restricted;
    restricted.insert(restricted.end(), duplicate_copy.begin(), duplicate_copy.end());

    // abdul 27/07/2026 [restore the process worker limit even when a parity assertion later fails]
    class WorkerRestore final {
      private:
        unsigned int original;

      public:
        WorkerRestore() : original{billiards_worker_count()} {}
        ~WorkerRestore() {
            billiards_set_worker_count(original);
        }
    } restore;

    billiards_set_worker_count(1);
    const auto single_worker = unfolding.generate_curves_lr(
        shooting_vector.first, shooting_vector.second, restricted);

    billiards_set_worker_count(2);
    const auto two_workers = unfolding.generate_curves_lr(
        shooting_vector.first, shooting_vector.second, restricted);

    billiards_set_worker_count(4);
    const auto four_workers = unfolding.generate_curves_lr(
        shooting_vector.first, shooting_vector.second, restricted);

    BOOST_CHECK(single_worker.first == two_workers.first);
    BOOST_CHECK(single_worker.second == two_workers.second);
    BOOST_CHECK(single_worker.first == four_workers.first);
    BOOST_CHECK(single_worker.second == four_workers.second);
}
