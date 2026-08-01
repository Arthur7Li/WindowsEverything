#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_backend
#include <boost/test/unit_test.hpp>

// Test that f(a) == b
// When writing tests, try testing against an incorrect value to ensure the test actually fails!

// Simpler with the build system to include everything in this one file
// It also compiles faster to have everything in one file
#include "bounding_region_test.hpp"
#include "code_sequence_test.hpp"
#include "diff_test.hpp"
#include "division_test.hpp"
#include "equations_test.hpp"
#include "general_test.hpp"
#include "gradient_test.hpp"
#include "intersection_test.hpp"
#include "falgo_test.hpp"
#include "parse_test.hpp"
#include "shooting_angles_test.hpp"
#include "triangle_billiard4_test.hpp"
#include "trig_identities_test.hpp"
#include "unfolding_test.hpp"
#include "wrapper_test.hpp"
// abdul 27/07/2026 [register focused native regressions for audited ABI, certified geometry, Vary4, restricted merge, and transforms]
