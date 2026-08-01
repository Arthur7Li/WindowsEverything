#pragma once

#include <triangle_billiard4.hpp>

#include <cmath>

BOOST_AUTO_TEST_CASE(test_triangle_billiard4_uses_vector_difference_values) {
    // abdul 27/07/2026 [reproduce the translated second-step trail where discarded subtractions widen the Vary4 beam]
    auto billiard = TriangleBilliard4::create(0.7, 0.8);
    auto right = billiard.getNext(false);
    BOOST_REQUIRE(right);

    const auto right_then_left = right->getNext(true);
    BOOST_REQUIRE(right_then_left);

    BOOST_CHECK_SMALL(right_then_left->interval() - 1.5, 1e-12);
    BOOST_CHECK(right_then_left->between(std::acos(-1.0) / 2.0));

    // abdul 27/07/2026 [lock the BUG-046 discriminator where a stale root beam admits an angle rejected by its descendant]
    const double descendant_angle = std::atan2(
        right_then_left->vertexA.y, right_then_left->vertexA.x);
    BOOST_CHECK(billiard.between(descendant_angle));
    BOOST_CHECK(!right_then_left->between(descendant_angle));
}
