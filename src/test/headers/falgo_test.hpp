#pragma once

#include <falgo.hpp>

#include <vector>

namespace {

// abdul 27/07/2026 [ensure transform capacity optimization never introduces a default-construction requirement]
struct NonDefaultTransformResult final {
    int value;

    NonDefaultTransformResult() = delete;
    explicit NonDefaultTransformResult(const int value_) : value{value_} {}

    friend bool operator==(const NonDefaultTransformResult& lhs, const NonDefaultTransformResult& rhs) {
        return lhs.value == rhs.value;
    }
};

} // namespace

BOOST_AUTO_TEST_CASE(test_vector_transform_preserves_size_order_and_nondefault_result) {
    // abdul 27/07/2026 [protect transform semantics while reserving its exactly-known output capacity]
    const std::vector<int> input{3, 1, 4, 1, 5};
    const auto transformed = falgo::transform(input, [](const int value) {
        return NonDefaultTransformResult{value * 2};
    });

    const std::vector<NonDefaultTransformResult> expected{
        NonDefaultTransformResult{6},
        NonDefaultTransformResult{2},
        NonDefaultTransformResult{8},
        NonDefaultTransformResult{2},
        NonDefaultTransformResult{10}
    };
    BOOST_CHECK(transformed == expected);

    const std::vector<int> empty;
    const auto transformed_empty = falgo::transform(empty, [](const int value) {
        return NonDefaultTransformResult{value};
    });
    BOOST_CHECK(transformed_empty.empty());
}
