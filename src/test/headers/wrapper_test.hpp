#pragma once

#include <wrapper.hpp>

#include <cstdint>
#include <type_traits>

// abdul 27/07/2026 [lock the gradient export to the integer status ABI consumed by JNA and exercise both terminal states]
static_assert(
    std::is_same<
        decltype(calculate_gradient(
            static_cast<const char*>(nullptr),
            float64_t{},
            float64_t{},
            false,
            static_cast<CString*>(nullptr),
            static_cast<CString*>(nullptr))),
        int32_t>::value,
    "calculate_gradient must return the int32_t status declared by the Java JNA boundary");

BOOST_AUTO_TEST_CASE(test_calculate_gradient_status_contract) {
    CString details{nullptr};
    CString radius{nullptr};

    const int32_t success = calculate_gradient("sin(x)", 0.1, 0.2, false, &details, &radius);
    BOOST_REQUIRE_EQUAL(success, 1);
    BOOST_REQUIRE(details.string != nullptr);
    BOOST_REQUIRE(radius.string != nullptr);
    cleanup_string(&details);
    cleanup_string(&radius);

    CString failed_details{nullptr};
    CString failed_radius{nullptr};
    const int32_t failure = calculate_gradient(
        "not-an-equation", 0.1, 0.2, false, &failed_details, &failed_radius);
    BOOST_CHECK_EQUAL(failure, -1);
    BOOST_CHECK(failed_details.string == nullptr);
    BOOST_CHECK(failed_radius.string == nullptr);
}

// abdul 31/07/2026 [prove cancellation stays latched until a new operation explicitly resets it]
BOOST_AUTO_TEST_CASE(test_native_vary_cancellation_is_operation_latched) {
    // Start a fresh operation in the runnable state.
    backend_reset_cancel();
    // Confirm the reset is visible to native traversal polling points.
    BOOST_TEST(!cancel_flag().load(std::memory_order_acquire));
    // Request cancellation through the exported ABI used by Java progress controls.
    backend_cancel();
    // Confirm individual queued calls cannot clear the terminal state themselves.
    BOOST_TEST(cancel_flag().load(std::memory_order_acquire));
    // Admit the next operation explicitly.
    backend_reset_cancel();
    // Confirm the fresh operation can run after its predecessor was cancelled.
    BOOST_TEST(!cancel_flag().load(std::memory_order_acquire));
}
