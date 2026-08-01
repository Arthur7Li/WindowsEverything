#pragma once

#include <intersection.hpp>
#include <parse.hpp>

// abdul 27/07/2026 [exercise common-root certification, connected-arc membership, and hidden-crossing rejection]
BOOST_AUTO_TEST_CASE(test_krawczyk_certifies_one_common_root) {
    const EquationGradient<XY, LinComArrZ<XYEta>> x_equation{
        parse_lin_com_arr_xyeta("x")};
    const EquationGradient<XY, LinComArrZ<XYEta>> y_equation{
        parse_lin_com_arr_xyeta("y")};

    const auto certified = certify_common_root(
        x_equation, y_equation, Vector2<Real>{Real{0}, Real{0}});

    BOOST_REQUIRE(certified);
    BOOST_TEST(curve_sign_at_point(x_equation.equation, *certified) == Sign::ZERO);
    BOOST_TEST(curve_sign_at_point(y_equation.equation, *certified) == Sign::ZERO);
}

BOOST_AUTO_TEST_CASE(test_krawczyk_rejects_disjoint_equation_zeros) {
    const EquationGradient<XY, LinComArrZ<XYEta>> first{
        parse_lin_com_arr_xyeta("x")};
    const EquationGradient<XY, LinComArrZ<XYEta>> second{
        parse_lin_com_arr_xyeta("x-eta")};

    const auto certified = certify_common_root(
        first, second, Vector2<Real>{Real{"0.5"}, Real{0}});

    BOOST_TEST(!certified);
}

BOOST_AUTO_TEST_CASE(test_intersection_returns_certified_line_arc_root) {
    const EquationGradient<XY, LinComArrZ<XYEta>> vertical{
        parse_lin_com_arr_xyeta("x")};
    const EquationGradient<XY, LinComArrZ<XYEta>> horizontal{
        parse_lin_com_arr_xyeta("y")};

    const auto root = intersection(
        vertical,
        horizontal,
        Vector2<Real>{Real{0}, Real{-1}},
        Vector2<Real>{Real{0}, Real{1}});

    BOOST_TEST(curve_sign_at_point(vertical.equation, root) == Sign::ZERO);
    BOOST_TEST(curve_sign_at_point(horizontal.equation, root) == Sign::ZERO);
}

BOOST_AUTO_TEST_CASE(test_chord_slab_does_not_certify_another_nonlinear_branch) {
    const EquationGradient<XY, Equation<Sin>> boundary{
        parse_lin_com_map_sin_xy("sin(y)")};
    const Real pi = boost::math::constants::pi<Real>();
    const Real radius{"1e-30"};
    const Vector2<Interval> root{
        Interval{Real{1} - radius, Real{1} + radius},
        Interval{pi - radius, pi + radius}};

    BOOST_TEST(!certified_same_boundary_arc(
        boundary,
        Vector2<Real>{Real{0}, Real{0}},
        Vector2<Real>{Real{2}, Real{0}},
        root));
}

BOOST_AUTO_TEST_CASE(test_equal_endpoint_signs_do_not_hide_two_cosine_roots) {
    const EquationGradient<XY, LinComArrZ<XYEta>> boundary{
        parse_lin_com_arr_xyeta("y")};
    const EquationGradient<XY, Equation<Cos>> curve{
        parse_lin_com_map_cos_xy("cos(x)")};
    const Vector2<Interval> first{Interval{-2}, Interval{0}};
    const Vector2<Interval> second{Interval{2}, Interval{0}};

    BOOST_TEST(!certified_no_hidden_crossing(
        boundary, curve, first, second));
}

/**
 * Reproduces the first generalized-edge certificate reported by Abdul on
 * 31 July 2026 and verifies the equal-sign condition that requires a hidden
 * crossing certificate before refinement may publish the edge.
 *
 * The Java Vary layer is responsible for skipping this candidate and
 * continuing independent coordinates. The adjacent synthetic two-root test
 * separately proves that equal endpoint signs cannot be accepted directly.
 */
// abdul 31/07/2026 [retain the exact nextIter generalized-edge failure as an explicit fail-closed regression]
BOOST_AUTO_TEST_CASE(test_reported_autopolyvary_edge_requires_hidden_crossing_certificate) {
    // Describe the proposed region boundary whose connected arc could contain concealed crossings.
    const EquationGradient<XY, Equation<Sin>> boundary{
        parse_lin_com_map_sin_xy(
            "-sin(3x+16y)-sin(5x+10y)+sin(5x+16y)+sin(5x+18y)+sin(7x+10y)-sin(7x+18y)"
            "+sin(9x+8y)-sin(9x+14y)-sin(11x+8y)+sin(11x+16y)+sin(35x+40y)-sin(35x+46y)"
            "-sin(37x+40y)+sin(37x+46y)-sin(39x+38y)+sin(39x+44y)+sin(41x+38y)-sin(41x+44y)"
            "-sin(43x+42y)+sin(43x+48y)+sin(45x+42y)-sin(45x+48y)-sin(69x+74y)+sin(69x+80y)"
            "+sin(71x+74y)-sin(71x+80y)-sin(73x+78y)+sin(73x+84y)+sin(75x+78y)-sin(75x+84y)"
            "+sin(77x+76y)-sin(77x+82y)-sin(79x+76y)+sin(79x+82y)-sin(103x+114y)+sin(103x+120y)"
            "+sin(105x+114y)-sin(105x+120y)+sin(107x+112y)-sin(107x+118y)-sin(109x+112y)+sin(109x+118y)"
            "-sin(135x+152y)+sin(135x+158y)+sin(137x+152y)-sin(137x+158y)+sin(141x+152y)-sin(141x+158y)"
            "-sin(143x+152y)+sin(143x+158y)+sin(169x+186y)-sin(169x+192y)-sin(171x+186y)+sin(171x+192y)")};
    // Describe the new constraint whose equal endpoint signs are insufficient evidence about the arc interior.
    const EquationGradient<XY, Equation<Sin>> curve{
        parse_lin_com_map_sin_xy(
            "-sin(3x+16y)-sin(5x+10y)+sin(5x+16y)+sin(5x+18y)+sin(7x+10y)-sin(7x+18y)"
            "-sin(11x+8y)+sin(11x+10y)+sin(35x+40y)-sin(35x+46y)-sin(37x+40y)+sin(37x+46y)"
            "-sin(39x+38y)+sin(39x+44y)+sin(41x+38y)-sin(41x+44y)-sin(43x+42y)+sin(43x+48y)"
            "+sin(45x+42y)-sin(45x+48y)-sin(69x+74y)+sin(69x+80y)+sin(71x+74y)-sin(71x+80y)"
            "-sin(73x+78y)+sin(73x+84y)+sin(75x+78y)-sin(75x+84y)+sin(77x+76y)-sin(77x+82y)"
            "-sin(79x+76y)+sin(79x+82y)-sin(103x+114y)+sin(103x+120y)+sin(105x+114y)-sin(105x+120y)"
            "+sin(107x+112y)-sin(107x+118y)-sin(109x+112y)+sin(109x+118y)-sin(135x+152y)+sin(135x+158y)"
            "+sin(137x+152y)-sin(137x+158y)+sin(141x+152y)-sin(141x+158y)-sin(143x+152y)+sin(143x+158y)"
            "+sin(169x+186y)-sin(169x+192y)-sin(171x+186y)+sin(171x+192y)")};
    // Preserve the first reported interval endpoint without widening it into an unrelated search region.
    const Vector2<Interval> first{
        Interval{Real{"2.59474"}}, Interval{Real{"0.459712"}}};
    // Preserve the second reported interval endpoint in the original orientation.
    const Vector2<Interval> second{
        Interval{Real{"2.59045"}}, Interval{Real{"0.463624"}}};

    // Evaluate the reported constraint at the first boundary endpoint.
    const Sign first_sign = curve_sign_at_point(curve.equation, first);
    // Evaluate the same constraint at the second boundary endpoint.
    const Sign second_sign = curve_sign_at_point(curve.equation, second);
    // Confirm this exact incident reaches the guarded equal-sign branch.
    BOOST_TEST(first_sign == second_sign);
    // Confirm interval uncertainty at an endpoint is not the reason the guard runs.
    BOOST_TEST(first_sign != Sign::ZERO);
}

// abdul 28/07/2026 [retain a unique line-cosine root when an unrelated endpoint is a stationary nonzero point]
BOOST_AUTO_TEST_CASE(test_line_cosine_unique_root_allows_endpoint_derivative_zero) {
    const EquationGradient<XY, LinComArrZ<XYEta>> boundary{
        parse_lin_com_arr_xyeta("5x+2y-7eta")};
    const EquationGradient<XY, Equation<Cos>> curve{
        parse_lin_com_map_cos_xy(
            "cos(x-y)-cos(5x+y)-cos(5x+3y)"
            "+cos(7x+y)+cos(7x+3y)")};
    const Real pi = boost::math::constants::pi<Real>();
    const Vector2<Real> first{11 * pi / 18, 2 * pi / 9};
    const Vector2<Real> second{pi / 2, pi / 2};

    const Vector2<Interval> root =
        intersection(boundary, curve, first, second);

    BOOST_TEST(
        curve_sign_at_point(boundary.equation, root) == Sign::ZERO);
    BOOST_TEST(
        curve_sign_at_point(curve.equation, root) == Sign::ZERO);
}

// abdul 28/07/2026 [isolate the long-CS generalized edge that must certify without restoring the hidden-crossing assumption]
BOOST_AUTO_TEST_CASE(test_long_cs_positive_generalized_edge_is_certified) {
    const EquationGradient<XY, Equation<Sin>> curve{
        parse_lin_com_map_sin_xy(
            "-sin(13y)+sin(2x-13y)-sin(2x+13y)+sin(4x-17y)+sin(4x-15y)+sin(4x-13y)"
            "-sin(4x+13y)-sin(6x+13y)-sin(8x+13y)-sin(10x+13y)-sin(12x-15y)"
            "-sin(12x-13y)-sin(12x-11y)-sin(12x+13y)-sin(14x+13y)-sin(16x+13y)"
            "-sin(18x+13y)-sin(18x+41y)-sin(18x+43y)-sin(18x+45y)+sin(20x+15y)"
            "+sin(20x+17y)+sin(20x+19y)-sin(46x+25y)-sin(46x+27y)-sin(46x+29y)"
            "+sin(52x+53y)+sin(52x+55y)+sin(52x+57y)-sin(60x+55y)-sin(60x+57y)"
            "-sin(60x+59y)-sin(74x+59y)-sin(74x+61y)-sin(84x+85y)-sin(84x+87y)"
            "-sin(84x+89y)-sin(88x+77y)+sin(92x+87y)+sin(92x+89y)+sin(92x+91y)"
            "-sin(96x+87y)-sin(108x+101y)-sin(116x+111y)+sin(124x+125y)"
            "+sin(124x+127y)+sin(124x+129y)-sin(132x+129y)-sin(132x+131y)"
            "-sin(156x+157y)-sin(156x+159y)-sin(156x+161y)")};
    const EquationGradient<XY, Equation<Sin>> boundary{
        parse_lin_com_map_sin_xy(
            "sin(11y)-sin(2x-11y)+sin(2x+11y)-sin(4x-11y)+sin(4x+11y)"
            "-sin(6x-11y)+sin(6x+11y)-sin(8x-11y)+sin(8x+11y)-sin(10x-11y)"
            "+sin(10x+11y)-sin(12x+13y)-sin(12x+15y)-sin(12x+17y)"
            "-sin(20x+15y)-sin(20x+17y)-sin(20x+19y)+sin(46x+25y)"
            "+sin(46x+27y)+sin(46x+29y)-sin(52x+53y)-sin(52x+55y)"
            "-sin(52x+57y)+sin(60x+55y)+sin(60x+57y)+sin(60x+59y)"
            "+sin(74x+59y)+sin(74x+61y)+sin(84x+85y)+sin(84x+87y)"
            "+sin(84x+89y)+sin(88x+77y)-sin(92x+87y)-sin(92x+89y)"
            "-sin(92x+91y)+sin(96x+87y)+sin(108x+101y)+sin(116x+111y)"
            "-sin(124x+125y)-sin(124x+127y)-sin(124x+129y)+sin(132x+129y)"
            "+sin(132x+131y)+sin(156x+157y)+sin(156x+159y)+sin(156x+161y)")};
    const Vector2<Interval> first{
        Interval{Real{"2.59642"}}, Interval{Real{"0.457958"}}};
    const Vector2<Interval> second{
        Interval{Real{"2.59796"}}, Interval{Real{"0.456321"}}};

    BOOST_TEST(certified_no_hidden_crossing(
        boundary, curve, first, second));
}

// abdul 28/07/2026 [lock in cosine boundary cancellation for the long-CS edge that previously entered unbounded implicit subdivision]
BOOST_AUTO_TEST_CASE(test_long_cs_cosine_shared_terms_are_certified) {
    const EquationGradient<XY, Equation<Cos>> boundary{
        parse_lin_com_map_cos_xy(
            "-cos(11x-15y)-cos(11x-13y)-cos(11x-11y)+cos(13x-15y)"
            "+cos(13x-13y)+cos(13x-11y)-cos(17x+45y)-cos(45x+25y)"
            "-cos(45x+27y)-cos(45x+29y)+cos(47x+25y)+cos(47x+27y)"
            "+cos(47x+29y)-cos(73x+59y)-cos(73x+61y)+cos(75x+59y)"
            "+cos(75x+61y)-cos(87x+77y)+cos(89x+77y)-cos(95x+87y)"
            "+cos(97x+87y)-cos(107x+101y)+cos(109x+101y)-cos(115x+111y)"
            "+cos(117x+111y)-cos(131x+129y)-cos(131x+131y)+cos(133x+129y)"
            "+cos(133x+131y)-cos(155x+157y)-cos(155x+159y)+cos(157x+157y)"
            "+cos(157x+159y)+cos(157x+161y)")};
    const EquationGradient<XY, Equation<Cos>> curve{
        parse_lin_com_map_cos_xy(
            "-cos(11x-15y)-cos(11x-13y)-cos(11x-11y)+cos(13x-15y)"
            "+cos(13x-13y)+cos(13x-11y)-cos(17x+45y)-cos(45x+25y)"
            "-cos(45x+27y)-cos(45x+29y)+cos(47x+25y)+cos(47x+27y)"
            "+cos(47x+29y)-cos(73x+59y)-cos(73x+61y)+cos(75x+59y)"
            "+cos(75x+61y)-cos(87x+77y)+cos(89x+77y)-cos(95x+87y)"
            "+cos(97x+87y)-cos(107x+101y)+cos(109x+101y)+cos(117x+111y)")};
    const Vector2<Interval> first{
        Interval{
            Real{"2.5980360371257795457579504569386706505750348500237"},
            Real{"2.5980360371257795457579504569406706505750348500237"}},
        Interval{
            Real{"0.45627414926090980992243056063429421245732308927178"},
            Real{"0.45627414926090980992243056063629421245732308927178"}}};
    const Vector2<Interval> second{
        Interval{
            Real{"2.5980305333744746546295123871553006930076338785604"},
            Real{"2.5980305333744746546295123871573006930076338785604"}},
        Interval{
            Real{"0.45628079120126346437919856688784439217897446488571"},
            Real{"0.45628079120126346437919856688984439217897446488571"}}};

    BOOST_TEST(certified_no_hidden_crossing(
        boundary, curve, first, second));
}
