#pragma once

#include <boost/optional/optional_io.hpp>

#include <equations.hpp>
#include <parse.hpp>
#include <utils.hpp>

#include <cstdlib>
#include <cstdint>
#include <iomanip>
#include <sstream>

static std::vector<CodeSequence> parse_file(const std::string& path) {

    std::vector<CodeSequence> code_seqs{};

    std::ifstream infile{path};
    if (!infile.is_open()) {
        throw std::runtime_error("missing MRR test fixture: " + path);
    }

    // This reads through all the lines of the file
    std::string line{};
    while (std::getline(infile, line)) {
        // When reading through a file:
        // - get a line
        // - trim the whitespace from both sides
        // - check if it is empty
        // - if not, continue to process
        boost::trim(line);

        if (line.empty()) {
            continue;
        } else if (line.find('s') != std::string::npos) {
            // Contains an s
            continue;
        } else if (line.find('S') != std::string::npos) {
            // Contains an S
        } else {
            const auto code_seq = parse_code_sequence(line);
            code_seqs.emplace_back(code_seq);
        }
    }

    return code_seqs;
}

BOOST_AUTO_TEST_CASE(test_calculate_empty) {

    const auto code_seqs = parse_file("src/test/resources/empty_codes_to_15.txt");
    BOOST_REQUIRE(!code_seqs.empty());

    for (const auto& code_seq : code_seqs) {
        const auto code_type = code_seq.type();
        if (is_stable(code_type)) {
            BOOST_TEST(!calculate_stable(code_seq, code_type));
        } else {
            BOOST_TEST(!calculate_unstable(code_seq, code_type));
        }
    }
}

BOOST_AUTO_TEST_CASE(test_calculate_nonempty) {

    const auto code_seqs = parse_file("src/test/resources/nonempty_codes_to_15.txt");
    BOOST_REQUIRE(!code_seqs.empty());

    for (const auto& code_seq : code_seqs) {
        const auto code_type = code_seq.type();
        if (is_stable(code_type)) {
            BOOST_TEST(calculate_stable(code_seq, code_type));
        } else {
            BOOST_TEST(calculate_unstable(code_seq, code_type));
        }
    }
}

static void check_same_stable_boundary(const Stable& lhs, const Stable& rhs) {
    BOOST_CHECK(lhs.initial_angles == rhs.initial_angles);
    BOOST_CHECK(lhs.equations == rhs.equations);
    BOOST_CHECK(lhs.left_rights == rhs.left_rights);
    BOOST_REQUIRE_EQUAL(lhs.points.size(), rhs.points.size());
    for (std::size_t i = 0; i < lhs.points.size(); ++i) {
        for (std::size_t axis = 0; axis < 2; ++axis) {
            BOOST_CHECK_EQUAL(boost::multiprecision::lower(lhs.points[i][axis]),
                              boost::multiprecision::lower(rhs.points[i][axis]));
            BOOST_CHECK_EQUAL(boost::multiprecision::upper(lhs.points[i][axis]),
                              boost::multiprecision::upper(rhs.points[i][axis]));
        }
    }
}

class WorkerCountRestore final {
  private:
    unsigned int original;

  public:
    WorkerCountRestore()
        : original{billiards_worker_count()} {
    }

    ~WorkerCountRestore() {
        billiards_set_worker_count(original);
    }
};

static CodeSequence reported_long_cs() {
    return parse_code_sequence(
        "1 5 20 2 4 2 16 4 26 4 16 4 26 5 1 34 1 5 26 4 16 4 26 4 16 2 4 2 20 5 1 29 "
        "6 34 6 29 1 5 22 5 1 29 6 34 6 28 4 14 2 8 2 12 2 8 2 16 4 24 5 1 31 6 32 6 31 "
        "1 5 24 5 1 31 6 32 6 31 1 5 24 5 1 31 6 32 6 31 1 5 24 5 1 31 6 32 6 31 1 5 24 4 16 "
        "2 8 2 12 2 8 2 14 4 28 6 34 6 29 1 5 22 5 1 29 6 34 6 29");
}

/**
 * @brief Returns the three exact closed-stable codes whose MRRs were rejected in Abdul's 31 July nextIter report.
 * @return Independently owned parsed code sequences in report order; no external storage is retained.
 * @throws std::runtime_error If a literal no longer satisfies the code-sequence grammar.
 * @invariant Each returned sequence classifies as closed stable and previously reached the experimental equal-sign guard.
 */
// abdul 31/07/2026 [retain every reported AutoPolyVary MRR failure as a full calculation regression]
static std::vector<CodeSequence> reported_nextiter_autopolyvary_codes() {
    // Preserve point 1 candidate 5, the second native failure printed in nextIter.
    const auto point_one_candidate_five = parse_code_sequence(
        "1 5 22 4 22 4 20 4 22 4 20 4 24 5 1 33 6 29 1 6 1 27 5 1 33 6 32 6 30 6 33 1 5 24 4 20 4 24 5 1 33 6 30 6 33 1 5 26 4 16 4 28 6 34 6 28 4 16 4 26 4 16 4 26 4 16 4 28 6 34 6 28 4 16 4 26 5 1 33 6 30 6 33 1 5 24 4 20 4 24 5 1 33 6 30 6 32 6 33 1 5 27 1 6 1 29 6 33 1 5 24 4 20 4 22 4 20 4 22 4 22 5 1 30");
    // Preserve point 1 candidate 6, the first native failure printed in nextIter.
    const auto point_one_candidate_six = parse_code_sequence(
        "1 5 24 5 1 31 6 32 6 30 4 14 4 28 5 1 35 6 28 5 1 34 1 5 26 4 16 2 8 2 10 2 14 4 27 1 6 1 29 6 33 1 5 26 5 1 33 6 30 6 34 6 28 5 1 33 6 32 6 30 6 34 6 29 1 6 1 27 5 1 34 1 5 27 1 6 1 29 6 34 6 30 6 32 6 33 1 5 28 6 34 6 30 6 33 1 5 26 5 1 33 6 29 1 6 1 27 4 14 2 10 2 8 2 16 4 26 5 1 34 1 5 28 6 35 1 5 28 4 14 4 30 6 32 6 31");
    // Preserve point 2 candidate 6, whose maximum frequencies match the third printed failing curve.
    const auto point_two_candidate_six = parse_code_sequence(
        "1 5 22 5 1 28 1 6 1 27 4 16 4 26 5 1 33 6 30 4 12 4 31 1 5 26 5 1 33 6 30 6 33 1 5 26 5 1 31 4 12 4 30 6 33 1 5 26 4 16 4 27 1 6 1 28 1 5 22 5 1 29 6 33 1 5 26 5 1 33 6 28 4 16 4 26 5 1 33 6 30 6 33 1 5 26 5 1 32 1 5 26 5 1 33 6 30 6 33 1 5 26 4 16 4 28 6 33 1 5 26 5 1 33 6 29");
    // Return new values so individual test calculations cannot share mutable boundary state.
    return {
        point_one_candidate_five,
        point_one_candidate_six,
        point_two_candidate_six};
}

static uint64_t stable_boundary_hash(const Stable& stable) {
    // The benchmark compares this digest across processes and worker counts.
    // Include interval endpoints and boundary provenance, not only median
    // points, so a faster but representation-invalid MRR cannot pass.
    std::ostringstream normalized;
    normalized << std::setprecision(50) << stable.initial_angles << '\n';
    for (const auto& point : stable.points) {
        for (std::size_t axis = 0; axis < 2; ++axis) {
            normalized << boost::multiprecision::lower(point[axis]) << ','
                       << boost::multiprecision::upper(point[axis]) << ';';
        }
    }
    normalized << '\n';
    for (const auto& equation : stable.equations) {
        normalized << equation << '\n';
    }
    for (const auto& left_right : stable.left_rights) {
        normalized << left_right << '\n';
    }

    // abdul 28/07/2026 [allow opt-in benchmark payload dumps so certificate changes can be audited beyond a digest]
    const std::string normalized_text = normalized.str();
    if (std::getenv("BILLIARDS_DUMP_BENCHMARK_BOUNDARY") != nullptr) {
        std::cout << "BENCH_BOUNDARY_BEGIN\n"
                  << normalized_text
                  << "BENCH_BOUNDARY_END" << std::endl;
    }

    // FNV-1a is intentionally simple and stable; this is a regression digest,
    // not a cryptographic authenticity check.
    uint64_t hash = UINT64_C(14695981039346656037);
    for (const unsigned char value : normalized_text) {
        hash ^= value;
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

BOOST_AUTO_TEST_CASE(test_mrr_worker_count_is_deterministic_and_caps_tbb) {
    WorkerCountRestore restore;
    const auto code_seq = parse_code_sequence("1 3 3");
    const auto code_type = code_seq.type();

    billiards_set_worker_count(1);
    const auto single_worker = calculate_stable(code_seq, code_type);
    BOOST_REQUIRE(single_worker);

    billiards_set_worker_count(2);
    BOOST_CHECK_EQUAL(billiards_worker_count(), 2u);
    BOOST_CHECK_LE(tbb::global_control::active_value(tbb::global_control::max_allowed_parallelism), 2u);
    const auto two_workers = calculate_stable(code_seq, code_type);
    BOOST_REQUIRE(two_workers);

    check_same_stable_boundary(*single_worker, *two_workers);
}

BOOST_AUTO_TEST_CASE(test_reported_long_cs_mrr_regression) {
    if (std::getenv("BILLIARDS_RUN_SLOW_TESTS") == nullptr) {
        BOOST_TEST_MESSAGE("Skipping long MRR regression; run testBackendSlow to enable it");
        return;
    }

    WorkerCountRestore restore;
    const auto code_seq = reported_long_cs();
    const auto code_type = code_seq.type();
    BOOST_REQUIRE(code_type == CodeType::CS);

    billiards_set_worker_count(1);
    const auto single_worker = calculate_stable(code_seq, code_type);
    BOOST_REQUIRE(single_worker);

    billiards_set_worker_count(4);
    const auto four_workers = calculate_stable(code_seq, code_type);
    BOOST_REQUIRE(four_workers);

    check_same_stable_boundary(*single_worker, *four_workers);
}

// abdul 31/07/2026 [require all nextIter AutoPolyVary candidates to produce complete MRRs instead of skips or exceptions]
BOOST_AUTO_TEST_CASE(test_reported_nextiter_autopolyvary_mrrs_succeed) {
    // Keep the expensive real MRR calculations in the established slow native gate.
    if (std::getenv("BILLIARDS_RUN_SLOW_TESTS") == nullptr) {
        BOOST_TEST_MESSAGE("Skipping nextIter AutoPolyVary MRR regressions; run testBackendSlow to enable them");
        return;
    }

    // Restore the process worker limit after this deterministic compatibility check.
    WorkerCountRestore restore;
    // Use one worker so this test also covers the smallest supported native budget.
    billiards_set_worker_count(1);
    // Calculate every previously rejected code through the same production MRR entry point.
    for (const auto& code_seq : reported_nextiter_autopolyvary_codes()) {
        // Confirm the fixture still represents the closed-stable AutoPolyVary candidates from the report.
        BOOST_REQUIRE(code_seq.type() == CodeType::CS);
        // A missing value or thrown refinement error is the exact regression this test forbids.
        const auto stable = calculate_stable(code_seq, code_seq.type());
        // Require a complete nonempty stable region for publication and database caching.
        BOOST_REQUIRE(stable);
        // Require an ordered polygon rather than a degenerate placeholder result.
        BOOST_REQUIRE_GE(stable->points.size(), 3u);
    }
}

BOOST_AUTO_TEST_CASE(benchmark_reported_long_cs_mrr) {
    if (std::getenv("BILLIARDS_RUN_BENCHMARKS") == nullptr) {
        BOOST_TEST_MESSAGE("Skipping benchmark workload; use tools/benchmark/run-benchmarks.ps1");
        return;
    }

    const char* const raw_workers = std::getenv("BILLIARDS_BENCHMARK_WORKER");
    if (raw_workers == nullptr) {
        BOOST_FAIL("BILLIARDS_BENCHMARK_WORKER is required for the benchmark workload");
    }

    char* end = nullptr;
    const long parsed_workers = std::strtol(raw_workers, &end, 10);
    if (end == raw_workers || *end != '\0' || parsed_workers <= 0) {
        BOOST_FAIL("BILLIARDS_BENCHMARK_WORKER must be a positive integer");
    }

    WorkerCountRestore restore;
    billiards_set_worker_count(static_cast<unsigned int>(parsed_workers));
    const auto code_seq = reported_long_cs();
    const auto stable = calculate_stable(code_seq, code_seq.type());
    BOOST_REQUIRE(stable);

    std::cout << "BENCH_RESULT hash=" << std::hex << stable_boundary_hash(*stable) << std::dec
              << " points=" << stable->points.size()
              << " equations=" << stable->equations.size()
              << " workers=" << billiards_worker_count() << std::endl;
}
