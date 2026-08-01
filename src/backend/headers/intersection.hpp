#pragma once

#include "general.hpp"
// abdul 28/07/2026 [use exact straight-boundary substitution in line/trigonometric crossing certificates]
#include "linear_derivative.hpp"
#include "newton.hpp"

inline Sign curve_sign_at_point(const LinComArrZ<XYEta>& equation, const Vector2<Interval>& point) {
    const auto result = evalf<Interval>(equation, point[0], point[1]);
    return sign(result);
}

inline Sign curve_sign_at_point(const Equation<Sin>& equation, const Vector2<Interval>& point) {

    mpfi_t sum;
    mpfi_init2(sum, 168);

    mpfi_t a;
    mpfi_init2(a, 168);

    mpfi_t x;
    mpfi_init2(x, 168);

    mpfi_t y;
    mpfi_init2(y, 168);

    mpfi_set_si(sum, 0);

    for (const auto& kv : equation) {

        const auto& arg = kv.first.arg;
        const auto x_coeff = arg.coeff<XY::X>();
        const auto y_coeff = arg.coeff<XY::Y>();

        mpfi_mul_si(x, point[0].backend().data(), x_coeff);
        mpfi_mul_si(y, point[1].backend().data(), y_coeff);

        mpfi_add(a, x, y);

        mpfi_sin(a, a);

        mpfi_mul_si(a, a, kv.second);

        mpfi_add(sum, sum, a);
    }

    mpfi_clear(a);
    mpfi_clear(x);
    mpfi_clear(y);

    if (mpfi_is_strictly_pos(sum)) {
        mpfi_clear(sum);
        return Sign::POS;
    }

    if (mpfi_is_strictly_neg(sum)) {
        mpfi_clear(sum);
        return Sign::NEG;
    }

    if (mpfi_has_zero(sum)) {
        mpfi_clear(sum);
        return Sign::ZERO;
    }

    throw std::runtime_error("unable to find sign for sum");
}

inline Sign curve_sign_at_point(const Equation<Cos>& equation, const Vector2<Interval>& point) {

    mpfi_t sum;
    mpfi_init2(sum, 168);

    mpfi_t a;
    mpfi_init2(a, 168);

    mpfi_t x;
    mpfi_init2(x, 168);

    mpfi_t y;
    mpfi_init2(y, 168);

    mpfi_set_si(sum, 0);

    for (const auto& kv : equation) {

        const auto& arg = kv.first.arg;
        const auto x_coeff = arg.coeff<XY::X>();
        const auto y_coeff = arg.coeff<XY::Y>();

        mpfi_mul_si(x, point[0].backend().data(), x_coeff);
        mpfi_mul_si(y, point[1].backend().data(), y_coeff);

        mpfi_add(a, x, y);

        mpfi_cos(a, a);

        mpfi_mul_si(a, a, kv.second);

        mpfi_add(sum, sum, a);
    }

    mpfi_clear(a);
    mpfi_clear(x);
    mpfi_clear(y);

    if (mpfi_is_strictly_pos(sum)) {
        mpfi_clear(sum);
        return Sign::POS;
    }

    if (mpfi_is_strictly_neg(sum)) {
        mpfi_clear(sum);
        return Sign::NEG;
    }

    if (mpfi_has_zero(sum)) {
        mpfi_clear(sum);
        return Sign::ZERO;
    }

    throw std::runtime_error("unable to find sign for sum");
}

// p -> q, x is the query point
// dot(v, w) is proportional to cos(theta), where theta = angle between v and w
// cos(theta) < 0 for theta > 90, = 0 for theta = 90, and > 0 for theta < 90
inline int point_sign_line(const Vector2<Real>& p, const Vector2<Real>& q, const Vector2<Real>& x) {
    const Vector2<Real> v = q - p;
    const Vector2<Real> w = x - p;

    const Real dot = v.dot(w);

    const auto sign = dot.sign();

    return sign;
}

inline Vector2<Interval> interval_hull(
        const Vector2<Real>& first,
        const Vector2<Real>& second,
        const Vector2<Interval>& extra) {
    const Real x_low = std::min(
        std::min(first[0], second[0]),
        boost::multiprecision::lower(extra[0]));
    const Real x_high = std::max(
        std::max(first[0], second[0]),
        boost::multiprecision::upper(extra[0]));
    const Real y_low = std::min(
        std::min(first[1], second[1]),
        boost::multiprecision::lower(extra[1]));
    const Real y_high = std::max(
        std::max(first[1], second[1]),
        boost::multiprecision::upper(extra[1]));
    return {
        Interval{x_low, x_high},
        Interval{y_low, y_high}};
}

inline Vector2<Interval> interval_hull(
    const Vector2<Interval>& first,
    const Vector2<Interval>& second);

inline bool interval_strictly_inside(
        const Interval& inner,
        const Interval& outer) {
    return boost::multiprecision::lower(inner)
            > boost::multiprecision::lower(outer)
        && boost::multiprecision::upper(inner)
            < boost::multiprecision::upper(outer);
}

inline bool interval_strictly_between(
        const Interval& value,
        const Real& first,
        const Real& second) {
    const Real low = std::min(first, second);
    const Real high = std::max(first, second);
    return boost::multiprecision::lower(value) > low
        && boost::multiprecision::upper(value) < high;
}

// abdul 27/07/2026 [replace independent range-zero checks with a Krawczyk inclusion proof for one common root]
template <typename Symbols, typename T, typename S>
boost::optional<Vector2<Interval>> certify_common_root(
        const EquationGradient<Symbols, T>& eq0,
        const EquationGradient<Symbols, S>& eq1,
        const Vector2<Real>& approximation) {
    Vector2<Real> center = approximation;
    // abdul 28/07/2026 [polish the scalar root before Krawczyk inclusion so the certified box reflects proof precision rather than the caller's Newton stop tolerance]
    for (uint32_t iteration = 0; iteration < 12; ++iteration) {
        Matrix2<Real> jacobian;
        jacobian(0, 0) =
            evalf<Real>(eq0.diff0, center[0], center[1]);
        jacobian(0, 1) =
            evalf<Real>(eq0.diff1, center[0], center[1]);
        jacobian(1, 0) =
            evalf<Real>(eq1.diff0, center[0], center[1]);
        jacobian(1, 1) =
            evalf<Real>(eq1.diff1, center[0], center[1]);
        const auto decomposition = jacobian.fullPivLu();
        if (!decomposition.isInvertible()) {
            return boost::none;
        }

        Vector2<Real> residual;
        residual[0] =
            evalf<Real>(eq0.equation, center[0], center[1]);
        residual[1] =
            evalf<Real>(eq1.equation, center[0], center[1]);
        const Vector2<Real> correction =
            decomposition.inverse() * residual;
        center -= correction;
        if (std::max(abs(correction[0]), abs(correction[1]))
                < Real{"1e-45"}) {
            break;
        }
    }

    Matrix2<Real> center_jacobian;
    center_jacobian(0, 0) =
        evalf<Real>(eq0.diff0, center[0], center[1]);
    center_jacobian(0, 1) =
        evalf<Real>(eq0.diff1, center[0], center[1]);
    center_jacobian(1, 0) =
        evalf<Real>(eq1.diff0, center[0], center[1]);
    center_jacobian(1, 1) =
        evalf<Real>(eq1.diff1, center[0], center[1]);

    const auto decomposition = center_jacobian.fullPivLu();
    if (!decomposition.isInvertible()) {
        return boost::none;
    }
    const Matrix2<Real> inverse = decomposition.inverse();

    Vector2<Real> residual;
    residual[0] =
        evalf<Real>(eq0.equation, center[0], center[1]);
    residual[1] =
        evalf<Real>(eq1.equation, center[0], center[1]);
    const Vector2<Real> corrected_center =
        center - inverse * residual;

    Real radius{"1e-30"};
    for (uint32_t attempt = 0; attempt < 24; ++attempt) {
        const Vector2<Interval> box{
            Interval{center[0] - radius, center[0] + radius},
            Interval{center[1] - radius, center[1] + radius}};

        const Interval j00 = evalf<Interval>(eq0.diff0, box[0], box[1]);
        const Interval j01 = evalf<Interval>(eq0.diff1, box[0], box[1]);
        const Interval j10 = evalf<Interval>(eq1.diff0, box[0], box[1]);
        const Interval j11 = evalf<Interval>(eq1.diff1, box[0], box[1]);

        const Interval m00 = Interval{1} - inverse(0, 0) * j00 - inverse(0, 1) * j10;
        const Interval m01 = -inverse(0, 0) * j01 - inverse(0, 1) * j11;
        const Interval m10 = -inverse(1, 0) * j00 - inverse(1, 1) * j10;
        const Interval m11 = Interval{1} - inverse(1, 0) * j01 - inverse(1, 1) * j11;

        const Interval centered_x = box[0] - center[0];
        const Interval centered_y = box[1] - center[1];
        const Vector2<Interval> krawczyk{
            corrected_center[0] + m00 * centered_x + m01 * centered_y,
            corrected_center[1] + m10 * centered_x + m11 * centered_y};

        if (interval_strictly_inside(krawczyk[0], box[0])
                && interval_strictly_inside(krawczyk[1], box[1])) {
            // abdul 28/07/2026 [return the tightest strict Krawczyk box so certified roots do not inject avoidable endpoint uncertainty into later arc proofs]
            return box;
        }
        radius *= 10;
    }
    return boost::none;
}

// abdul 28/07/2026 [combine natural and centered mean-value interval forms before declaring a local sign inconclusive]
template <typename EquationType>
Sign certified_equation_sign_on_box(
        const EquationType& equation,
        const Vector2<Interval>& box) {
    const Interval natural =
        evalf<Interval>(equation, box[0], box[1]);
    const Sign natural_sign = sign(natural);
    if (natural_sign != Sign::ZERO) {
        return natural_sign;
    }

    const Real x_center =
        (boost::multiprecision::lower(box[0])
            + boost::multiprecision::upper(box[0])) / 2;
    const Real y_center =
        (boost::multiprecision::lower(box[1])
            + boost::multiprecision::upper(box[1])) / 2;
    const Interval center_value = evalf<Interval>(
        equation, Interval{x_center}, Interval{y_center});
    const Interval derivative_x = evalf<Interval>(
        diff<XY::X>(equation), box[0], box[1]);
    const Interval derivative_y = evalf<Interval>(
        diff<XY::Y>(equation), box[0], box[1]);
    const Interval mean_value_enclosure =
        center_value
        + derivative_x * (box[0] - x_center)
        + derivative_y * (box[1] - y_center);
    return sign(mean_value_enclosure);
}

inline Sign certified_equation_sign_on_box(
        const Coeff64 equation,
        const Vector2<Interval>& box) {
    return sign(evalf<Interval>(
        equation, box[0], box[1]));
}

// abdul 28/07/2026 [enclose the tangent determinant in both natural and centered forms so shared derivatives retain cancellation]
template <typename BoundaryType, typename CurveType>
Interval certified_tangent_determinant_enclosure_on_box(
        const EquationGradient<XY, BoundaryType>& boundary,
        const EquationGradient<XY, CurveType>& curve,
        const Vector2<Interval>& box) {
    const Interval boundary_x = evalf<Interval>(
        boundary.diff0, box[0], box[1]);
    const Interval boundary_y = evalf<Interval>(
        boundary.diff1, box[0], box[1]);
    const Interval curve_x = evalf<Interval>(
        curve.diff0, box[0], box[1]);
    const Interval curve_y = evalf<Interval>(
        curve.diff1, box[0], box[1]);
    const Interval natural =
        boundary_x * curve_y - boundary_y * curve_x;

    const Real x_center =
        (boost::multiprecision::lower(box[0])
            + boost::multiprecision::upper(box[0])) / 2;
    const Real y_center =
        (boost::multiprecision::lower(box[1])
            + boost::multiprecision::upper(box[1])) / 2;
    const Interval center_x{x_center};
    const Interval center_y{y_center};
    const Interval center_boundary_x = evalf<Interval>(
        boundary.diff0, center_x, center_y);
    const Interval center_boundary_y = evalf<Interval>(
        boundary.diff1, center_x, center_y);
    const Interval center_curve_x = evalf<Interval>(
        curve.diff0, center_x, center_y);
    const Interval center_curve_y = evalf<Interval>(
        curve.diff1, center_x, center_y);
    const Interval center_determinant =
        center_boundary_x * center_curve_y
        - center_boundary_y * center_curve_x;

    const auto boundary_xx = diff<XY::X>(boundary.diff0);
    const auto boundary_xy = diff<XY::Y>(boundary.diff0);
    const auto boundary_yx = diff<XY::X>(boundary.diff1);
    const auto boundary_yy = diff<XY::Y>(boundary.diff1);
    const auto curve_xx = diff<XY::X>(curve.diff0);
    const auto curve_xy = diff<XY::Y>(curve.diff0);
    const auto curve_yx = diff<XY::X>(curve.diff1);
    const auto curve_yy = diff<XY::Y>(curve.diff1);

    const Interval determinant_x =
        evalf<Interval>(boundary_xx, box[0], box[1]) * curve_y
        + boundary_x * evalf<Interval>(
            curve_yx, box[0], box[1])
        - evalf<Interval>(
            boundary_yx, box[0], box[1]) * curve_x
        - boundary_y * evalf<Interval>(
            curve_xx, box[0], box[1]);
    const Interval determinant_y =
        evalf<Interval>(boundary_xy, box[0], box[1]) * curve_y
        + boundary_x * evalf<Interval>(
            curve_yy, box[0], box[1])
        - evalf<Interval>(
            boundary_yy, box[0], box[1]) * curve_x
        - boundary_y * evalf<Interval>(
            curve_xy, box[0], box[1]);
    const Interval mean_value_enclosure =
        center_determinant
        + determinant_x * (box[0] - x_center)
        + determinant_y * (box[1] - y_center);
    const Real intersection_low = std::max(
        boost::multiprecision::lower(natural),
        boost::multiprecision::lower(mean_value_enclosure));
    const Real intersection_high = std::min(
        boost::multiprecision::upper(natural),
        boost::multiprecision::upper(mean_value_enclosure));
    if (intersection_low <= intersection_high) {
        return Interval{intersection_low, intersection_high};
    }
    return Interval{
        std::min(
            boost::multiprecision::lower(natural),
            boost::multiprecision::lower(mean_value_enclosure)),
        std::max(
            boost::multiprecision::upper(natural),
            boost::multiprecision::upper(mean_value_enclosure))};
}

template <typename BoundaryType, typename CurveType>
Sign certified_tangent_determinant_sign_on_box(
        const EquationGradient<XY, BoundaryType>& boundary,
        const EquationGradient<XY, CurveType>& curve,
        const Vector2<Interval>& box) {
    return sign(certified_tangent_determinant_enclosure_on_box(
        boundary, curve, box));
}

// abdul 28/07/2026 [bound curve variation along a monotone implicit graph before entering adaptive arc subdivision]
template <typename BoundaryType, typename CurveType>
bool certified_nonzero_on_monotone_implicit_arc(
        const EquationGradient<XY, BoundaryType>& boundary,
        const EquationGradient<XY, CurveType>& curve,
        const Vector2<Interval>& first,
        const Vector2<Interval>& second) {
    const Vector2<Interval> hull = interval_hull(first, second);
    const Sign boundary_x_sign =
        certified_equation_sign_on_box(
            boundary.diff0, hull);
    const Sign boundary_y_sign =
        certified_equation_sign_on_box(
            boundary.diff1, hull);
    if (boundary_x_sign == Sign::ZERO
            || boundary_y_sign == Sign::ZERO) {
        return false;
    }

    const Interval boundary_y = evalf<Interval>(
        boundary.diff1, hull[0], hull[1]);
    const Interval tangent_determinant =
        certified_tangent_determinant_enclosure_on_box(
            boundary, curve, hull);
    const Interval derivative_along_x =
        -tangent_determinant / boundary_y;
    const Real derivative_bound = std::max(
        abs(boost::multiprecision::lower(
            derivative_along_x)),
        abs(boost::multiprecision::upper(
            derivative_along_x)));
    const Real x_span =
        boost::multiprecision::upper(hull[0])
        - boost::multiprecision::lower(hull[0]);
    const Real maximum_change = derivative_bound * x_span;
    const Interval first_value = evalf<Interval>(
        curve.equation, first[0], first[1]);
    const Interval second_value = evalf<Interval>(
        curve.equation, second[0], second[1]);
    const auto endpoint_clears_change = [&](
            const Interval& value) {
        return boost::multiprecision::lower(value)
                    > maximum_change
            || boost::multiprecision::upper(value)
                    < -maximum_change;
    };
    return endpoint_clears_change(first_value)
        || endpoint_clears_change(second_value);
}

// abdul 28/07/2026 [try strict box signs, implicit Lipschitz bounds, and tangent monotonicity as bounded hidden-root proofs]
template <typename BoundaryType, typename CurveType>
bool certified_no_hidden_crossing_quick(
        const EquationGradient<XY, BoundaryType>& boundary,
        const EquationGradient<XY, CurveType>& curve,
        const Vector2<Interval>& first,
        const Vector2<Interval>& second) {
    const Vector2<Interval> hull = interval_hull(first, second);
    if (curve_sign_at_point(
                curve.equation, hull) != Sign::ZERO
            || certified_equation_sign_on_box(
                curve.equation, hull) != Sign::ZERO
            || certified_nonzero_on_monotone_implicit_arc(
                boundary, curve, first, second)) {
        return true;
    }

    const Sign boundary_x_sign =
        certified_equation_sign_on_box(
            boundary.diff0, hull);
    const Sign boundary_y_sign =
        certified_equation_sign_on_box(
            boundary.diff1, hull);
    const Real first_x_mid =
        (boost::multiprecision::lower(first[0])
            + boost::multiprecision::upper(first[0])) / 2;
    const Real second_x_mid =
        (boost::multiprecision::lower(second[0])
            + boost::multiprecision::upper(second[0])) / 2;
    const Real first_y_mid =
        (boost::multiprecision::lower(first[1])
            + boost::multiprecision::upper(first[1])) / 2;
    const Real second_y_mid =
        (boost::multiprecision::lower(second[1])
            + boost::multiprecision::upper(second[1])) / 2;
    const bool certified_graph =
        (boundary_y_sign != Sign::ZERO
            && first_x_mid != second_x_mid)
        || (boundary_x_sign != Sign::ZERO
            && first_y_mid != second_y_mid);
    return certified_graph
        && certified_tangent_determinant_sign_on_box(
            boundary, curve, hull) != Sign::ZERO;
}

// abdul 28/07/2026 [use a centered mean-value enclosure when natural x/y interval evaluation loses decisive local derivative or curve signs]
template <typename Symbols, typename T>
bool certified_same_boundary_arc(
        const EquationGradient<Symbols, T>& boundary,
        const Vector2<Real>& first,
        const Vector2<Real>& second,
        const Vector2<Interval>& root) {
    const Vector2<Interval> hull = interval_hull(first, second, root);
    const Interval dx = evalf<Interval>(boundary.diff0, hull[0], hull[1]);
    const Interval dy = evalf<Interval>(boundary.diff1, hull[0], hull[1]);
    const Sign dx_sign = sign(dx) != Sign::ZERO
        ? sign(dx)
        : certified_equation_sign_on_box(
            boundary.diff0, hull);
    const Sign dy_sign = sign(dy) != Sign::ZERO
        ? sign(dy)
        : certified_equation_sign_on_box(
            boundary.diff1, hull);

    // A derivative with one strict sign across the entire hull makes the
    // boundary a single implicit graph there.  Requiring the corresponding
    // graph parameter to lie between the endpoint parameters binds the root
    // to that connected edge rather than another branch in the chord slab.
    return (dy_sign != Sign::ZERO
                && interval_strictly_between(root[0], first[0], second[0]))
        || (dx_sign != Sign::ZERO
                && interval_strictly_between(root[1], first[1], second[1]));
}

enum class ArcProperty : uint8_t {
    CURVE_NONZERO,
    CURVE_MONOTONE,
};

// abdul 28/07/2026 [subdivide one certified implicit graph and fail closed when curve sign or monotonicity remains inconclusive]
template <typename Symbols, typename T, typename S>
bool certified_implicit_arc_property(
        const EquationGradient<Symbols, T>& boundary,
        const EquationGradient<Symbols, S>& curve,
        const Vector2<Interval>& first,
        const Vector2<Interval>& second,
        const ArcProperty property) {
    const Vector2<Interval> full_hull = interval_hull(first, second);
    const Interval full_dx =
        evalf<Interval>(boundary.diff0, full_hull[0], full_hull[1]);
    const Interval full_dy =
        evalf<Interval>(boundary.diff1, full_hull[0], full_hull[1]);
    const Sign full_dx_sign = sign(full_dx) != Sign::ZERO
        ? sign(full_dx)
        : certified_equation_sign_on_box(
            boundary.diff0, full_hull);
    const Sign full_dy_sign = sign(full_dy) != Sign::ZERO
        ? sign(full_dy)
        : certified_equation_sign_on_box(
            boundary.diff1, full_hull);

    const bool parameter_is_x =
        full_dy_sign != Sign::ZERO
        && boost::multiprecision::lower(first[0])
            != boost::multiprecision::lower(second[0]);
    const bool parameter_is_y =
        full_dx_sign != Sign::ZERO
        && boost::multiprecision::lower(first[1])
            != boost::multiprecision::lower(second[1]);
    if (!parameter_is_x && !parameter_is_y) {
        return false;
    }

    struct ParameterPiece final {
        Real low;
        Real high;
        uint32_t depth;
    };
    std::vector<ParameterPiece> pending{
        ParameterPiece{Real{0}, Real{1}, 0}};
    constexpr uint32_t max_depth = 22;
    const Real x_span =
        boost::multiprecision::upper(full_hull[0])
        - boost::multiprecision::lower(full_hull[0]);
    const Real y_span =
        boost::multiprecision::upper(full_hull[1])
        - boost::multiprecision::lower(full_hull[1]);
    const Real enclosure_margin =
        std::max(x_span, y_span) + Real{"1e-20"};

    while (!pending.empty()) {
        const auto piece = pending.back();
        pending.pop_back();
        const Interval parameter{piece.low, piece.high};
        const Interval complement = Interval{1} - parameter;
        const Interval parameter_coordinate = parameter_is_x
            ? complement * first[0] + parameter * second[0]
            : complement * first[1] + parameter * second[1];
        const Interval dependent_hull = parameter_is_x
            ? Interval{
                boost::multiprecision::lower(full_hull[1]) - enclosure_margin,
                boost::multiprecision::upper(full_hull[1]) + enclosure_margin}
            : Interval{
                boost::multiprecision::lower(full_hull[0]) - enclosure_margin,
                boost::multiprecision::upper(full_hull[0]) + enclosure_margin};

        const auto interval_mid = [](const Interval& value) {
            return (boost::multiprecision::lower(value)
                + boost::multiprecision::upper(value)) / 2;
        };
        const Real parameter_mid =
            (boost::multiprecision::lower(parameter_coordinate)
                + boost::multiprecision::upper(parameter_coordinate)) / 2;
        const Real t_mid = (piece.low + piece.high) / 2;
        const Real first_dependent = parameter_is_x
            ? interval_mid(first[1])
            : interval_mid(first[0]);
        const Real second_dependent = parameter_is_x
            ? interval_mid(second[1])
            : interval_mid(second[0]);
        Real dependent_center =
            (Real{1} - t_mid) * first_dependent
            + t_mid * second_dependent;
        bool scalar_newton_ready = true;
        for (uint32_t iteration = 0; iteration < 40; ++iteration) {
            const Real value = parameter_is_x
                ? evalf<Real>(
                    boundary.equation, parameter_mid, dependent_center)
                : evalf<Real>(
                    boundary.equation, dependent_center, parameter_mid);
            const Real derivative = parameter_is_x
                ? evalf<Real>(
                    boundary.diff1, parameter_mid, dependent_center)
                : evalf<Real>(
                    boundary.diff0, dependent_center, parameter_mid);
            if (derivative == 0) {
                scalar_newton_ready = false;
                break;
            }
            const Real step = value / derivative;
            dependent_center -= step;
            if (abs(step) < Real{"1e-35"}) {
                break;
            }
        }
        const Real parameter_width = piece.high - piece.low;
        const Interval interpolated_dependent = parameter_is_x
            ? complement * first[1] + parameter * second[1]
            : complement * first[0] + parameter * second[0];
        const Real endpoint_uncertainty_radius = std::max(
            abs(
                dependent_center
                - boost::multiprecision::lower(
                    interpolated_dependent)),
            abs(
                boost::multiprecision::upper(
                    interpolated_dependent)
                - dependent_center));
        const Vector2<Interval> slope_box = parameter_is_x
            ? Vector2<Interval>{
                parameter_coordinate, dependent_hull}
            : Vector2<Interval>{
                dependent_hull, parameter_coordinate};
        const Interval slope_numerator = parameter_is_x
            ? evalf<Interval>(
                boundary.diff0, slope_box[0], slope_box[1])
            : evalf<Interval>(
                boundary.diff1, slope_box[0], slope_box[1]);
        const Interval slope_denominator = parameter_is_x
            ? evalf<Interval>(
                boundary.diff1, slope_box[0], slope_box[1])
            : evalf<Interval>(
                boundary.diff0, slope_box[0], slope_box[1]);
        Real implicit_slope_radius{0};
        if (sign(slope_denominator) != Sign::ZERO) {
            const Real denominator_minimum = std::min(
                abs(boost::multiprecision::lower(slope_denominator)),
                abs(boost::multiprecision::upper(slope_denominator)));
            const Real numerator_maximum = std::max(
                abs(boost::multiprecision::lower(slope_numerator)),
                abs(boost::multiprecision::upper(slope_numerator)));
            const Real parameter_radius = std::max(
                abs(
                    parameter_mid
                    - boost::multiprecision::lower(
                        parameter_coordinate)),
                abs(
                    boost::multiprecision::upper(
                        parameter_coordinate)
                    - parameter_mid));
            implicit_slope_radius =
                numerator_maximum / denominator_minimum
                * parameter_radius;
        }
        // abdul 28/07/2026 [retain certified endpoint-box uncertainty when shrinking implicit parameter pieces near an edge endpoint]
        const Real initial_radius =
            endpoint_uncertainty_radius
            + implicit_slope_radius
            + parameter_width * (enclosure_margin + Real{1})
            + Real{"1e-18"};
        const Real initial_low = dependent_center - initial_radius;
        const Real initial_high = dependent_center + initial_radius;
        Interval dependent{
            std::max(
                boost::multiprecision::lower(dependent_hull),
                initial_low),
            std::min(
                boost::multiprecision::upper(dependent_hull),
                initial_high)};
        bool included = false;
        for (uint32_t iteration = 0;
                scalar_newton_ready && iteration < 12;
                ++iteration) {
            const Real dependent_mid =
                (boost::multiprecision::lower(dependent)
                    + boost::multiprecision::upper(dependent)) / 2;
            const Vector2<Interval> midpoint_box = parameter_is_x
                ? Vector2<Interval>{
                    parameter_coordinate, Interval{dependent_mid}}
                : Vector2<Interval>{
                    Interval{dependent_mid}, parameter_coordinate};
            const Vector2<Interval> candidate_box = parameter_is_x
                ? Vector2<Interval>{parameter_coordinate, dependent}
                : Vector2<Interval>{dependent, parameter_coordinate};
            const Interval derivative = parameter_is_x
                ? evalf<Interval>(
                    boundary.diff1, candidate_box[0], candidate_box[1])
                : evalf<Interval>(
                    boundary.diff0, candidate_box[0], candidate_box[1]);
            if (sign(derivative) == Sign::ZERO) {
                break;
            }
            const Real center_derivative = parameter_is_x
                ? evalf<Real>(boundary.diff1, parameter_mid, dependent_mid)
                : evalf<Real>(boundary.diff0, dependent_mid, parameter_mid);
            if (center_derivative == 0) {
                break;
            }
            const Real inverse_center_derivative = 1 / center_derivative;
            const Interval value = evalf<Interval>(
                boundary.equation, midpoint_box[0], midpoint_box[1]);
            const Interval image =
                dependent_mid - inverse_center_derivative * value
                + (Interval{1} - inverse_center_derivative * derivative)
                    * (dependent - dependent_mid);
            const Real low = std::max(
                boost::multiprecision::lower(image),
                boost::multiprecision::lower(dependent));
            const Real high = std::min(
                boost::multiprecision::upper(image),
                boost::multiprecision::upper(dependent));
            if (low > high) {
                break;
            }
            if (boost::multiprecision::lower(image)
                        >= boost::multiprecision::lower(dependent)
                    && boost::multiprecision::upper(image)
                        <= boost::multiprecision::upper(dependent)) {
                dependent = image;
                included = true;
                // abdul 28/07/2026 [continue interval-Newton contraction after existence is certified so property checks use the tightest available arc enclosure]
                continue;
            }
            const Real padding =
                (high - low) / 1000 + Real{"1e-40"};
            const Real padded_low = low - padding;
            const Real padded_high = high + padding;
            dependent = Interval{
                std::max(
                    boost::multiprecision::lower(dependent_hull),
                    padded_low),
                std::min(
                    boost::multiprecision::upper(dependent_hull),
                    padded_high)};
        }

        if (included) {
            const Vector2<Interval> arc_box = parameter_is_x
                ? Vector2<Interval>{parameter_coordinate, dependent}
                : Vector2<Interval>{dependent, parameter_coordinate};
            bool property_holds = false;
            if (property == ArcProperty::CURVE_NONZERO) {
                property_holds =
                    curve_sign_at_point(curve.equation, arc_box) != Sign::ZERO
                    || certified_equation_sign_on_box(
                        curve.equation, arc_box) != Sign::ZERO;
            } else {
                const Interval boundary_dx =
                    evalf<Interval>(boundary.diff0, arc_box[0], arc_box[1]);
                const Interval boundary_dy =
                    evalf<Interval>(boundary.diff1, arc_box[0], arc_box[1]);
                const Interval curve_dx =
                    evalf<Interval>(curve.diff0, arc_box[0], arc_box[1]);
                const Interval curve_dy =
                    evalf<Interval>(curve.diff1, arc_box[0], arc_box[1]);
                const Interval tangent_derivative =
                    boundary_dx * curve_dy
                        - boundary_dy * curve_dx;
                property_holds =
                    sign(tangent_derivative) != Sign::ZERO;
            }
            if (property_holds) {
                continue;
            }
        }

        if (piece.depth == max_depth) {
            return false;
        }
        const Real midpoint = (piece.low + piece.high) / 2;
        pending.push_back(
            ParameterPiece{piece.low, midpoint, piece.depth + 1});
        pending.push_back(
            ParameterPiece{midpoint, piece.high, piece.depth + 1});
    }
    return true;
}

// abdul 28/07/2026 [require a connected-arc root and a strict tangent direction before accepting one crossing]
template <typename Symbols, typename T, typename S>
bool certified_unique_crossing_on_arc_impl(
        const EquationGradient<Symbols, T>& boundary,
        const EquationGradient<Symbols, S>& curve,
        const Vector2<Real>& first,
        const Vector2<Real>& second,
        const Vector2<Interval>& root) {
    if (!certified_same_boundary_arc(boundary, first, second, root)) {
        return false;
    }
    const Vector2<Interval> hull = interval_hull(first, second, root);
    const Interval boundary_dx = evalf<Interval>(boundary.diff0, hull[0], hull[1]);
    const Interval boundary_dy = evalf<Interval>(boundary.diff1, hull[0], hull[1]);
    const Interval curve_dx = evalf<Interval>(curve.diff0, hull[0], hull[1]);
    const Interval curve_dy = evalf<Interval>(curve.diff1, hull[0], hull[1]);
    const Interval tangent_derivative =
        boundary_dx * curve_dy - boundary_dy * curve_dx;
    if (sign(tangent_derivative) != Sign::ZERO) {
        return true;
    }
    const Vector2<Interval> first_box{
        Interval{first[0]}, Interval{first[1]}};
    const Vector2<Interval> second_box{
        Interval{second[0]}, Interval{second[1]}};
    return certified_implicit_arc_property(
        boundary,
        curve,
        first_box,
        second_box,
        ArcProperty::CURVE_MONOTONE);
}

template <typename Symbols, typename T, typename S>
bool certified_unique_crossing_on_arc(
        const EquationGradient<Symbols, T>& boundary,
        const EquationGradient<Symbols, S>& curve,
        const Vector2<Real>& first,
        const Vector2<Real>& second,
        const Vector2<Interval>& root) {
    return certified_unique_crossing_on_arc_impl(
        boundary, curve, first, second, root);
}

// abdul 28/07/2026 [cancel a zero-valued sine boundary before connected-arc uniqueness proof to retain exact roots with tighter intervals]
inline bool certified_unique_crossing_on_arc(
        const EquationGradient<XY, Equation<Sin>>& boundary,
        const EquationGradient<XY, Equation<Sin>>& curve,
        const Vector2<Real>& first,
        const Vector2<Real>& second,
        const Vector2<Interval>& root) {
    auto reduced = curve.equation;
    reduced.add(boundary.equation);
    if (reduced.is_zero()) {
        return false;
    }
    if (reduced.size() < curve.equation.size()) {
        const EquationGradient<XY, Equation<Sin>> reduced_gradient{reduced};
        if (certified_unique_crossing_on_arc_impl(
                boundary,
                reduced_gradient,
                first,
                second,
                root)) {
            return true;
        }
    }

    reduced = curve.equation;
    reduced.sub(boundary.equation);
    if (!reduced.is_zero()
            && reduced.size() < curve.equation.size()) {
        const EquationGradient<XY, Equation<Sin>> reduced_gradient{reduced};
        if (certified_unique_crossing_on_arc_impl(
                boundary,
                reduced_gradient,
                first,
                second,
                root)) {
            return true;
        }
    }

    const Vector2<Interval> hull =
        interval_hull(first, second, root);
    if (certified_same_boundary_arc(
                boundary, first, second, root)
            && certified_tangent_determinant_sign_on_box(
                boundary, curve, hull) != Sign::ZERO) {
        return true;
    }

    return certified_unique_crossing_on_arc_impl(
        boundary, curve, first, second, root);
}

template <typename S>
bool certified_unique_crossing_on_arc(
        const EquationGradient<XY, LinComArrZ<XYEta>>& boundary,
        const EquationGradient<XY, S>& curve,
        const Vector2<Real>& first,
        const Vector2<Real>& second,
        const Vector2<Interval>& root) {
    if (!certified_same_boundary_arc(boundary, first, second, root)) {
        return false;
    }

    Real root_parameter_low{0};
    Real root_parameter_high{1};
    for (size_t dimension = 0; dimension < 2; ++dimension) {
        const Real delta = second[dimension] - first[dimension];
        if (delta == 0) {
            continue;
        }
        Real low =
            (boost::multiprecision::lower(root[dimension])
                - first[dimension]) / delta;
        Real high =
            (boost::multiprecision::upper(root[dimension])
                - first[dimension]) / delta;
        if (low > high) {
            std::swap(low, high);
        }
        root_parameter_low = std::max(root_parameter_low, low);
        root_parameter_high = std::min(root_parameter_high, high);
    }
    if (root_parameter_low >= root_parameter_high
            || root_parameter_low <= 0
            || root_parameter_high >= 1) {
        return false;
    }

    struct ParameterPiece final {
        Real low;
        Real high;
        uint32_t depth;
    };
    std::vector<ParameterPiece> pending{
        ParameterPiece{Real{0}, root_parameter_low, 0},
        ParameterPiece{root_parameter_high, Real{1}, 0}};
    constexpr uint32_t max_depth = 28;

    while (!pending.empty()) {
        const auto piece = pending.back();
        pending.pop_back();
        if (piece.low == piece.high) {
            continue;
        }
        const Interval parameter{piece.low, piece.high};
        const Interval complement = Interval{1} - parameter;
        const Vector2<Interval> arc_piece{
            complement * first[0] + parameter * second[0],
            complement * first[1] + parameter * second[1]};
        if (curve_sign_at_point(curve.equation, arc_piece) != Sign::ZERO) {
            continue;
        }
        if (piece.depth == max_depth) {
            return false;
        }
        const Real midpoint = (piece.low + piece.high) / 2;
        pending.push_back(
            ParameterPiece{piece.low, midpoint, piece.depth + 1});
        pending.push_back(
            ParameterPiece{midpoint, piece.high, piece.depth + 1});
    }
    return true;
}

inline Vector2<Interval> interval_hull(
        const Vector2<Interval>& first,
        const Vector2<Interval>& second) {
    return {
        Interval{
            std::min(
                boost::multiprecision::lower(first[0]),
                boost::multiprecision::lower(second[0])),
            std::max(
                boost::multiprecision::upper(first[0]),
                boost::multiprecision::upper(second[0]))},
        Interval{
            std::min(
                boost::multiprecision::lower(first[1]),
                boost::multiprecision::lower(second[1])),
            std::max(
                boost::multiprecision::upper(first[1]),
                boost::multiprecision::upper(second[1]))}};
}

// abdul 27/07/2026 [prove equal-sign endpoints cannot conceal two crossings before preserving or discarding an entire nonlinear edge]
template <typename Symbols, typename T, typename S>
bool certified_no_hidden_crossing_impl(
        const EquationGradient<Symbols, T>& boundary,
        const EquationGradient<Symbols, S>& curve,
        const Vector2<Interval>& first,
        const Vector2<Interval>& second) {
    const Vector2<Interval> hull = interval_hull(first, second);
    if (curve_sign_at_point(curve.equation, hull) != Sign::ZERO) {
        return true;
    }

    const Interval boundary_dx = evalf<Interval>(boundary.diff0, hull[0], hull[1]);
    const Interval boundary_dy = evalf<Interval>(boundary.diff1, hull[0], hull[1]);
    const Real first_x_mid =
        (boost::multiprecision::lower(first[0])
            + boost::multiprecision::upper(first[0])) / 2;
    const Real second_x_mid =
        (boost::multiprecision::lower(second[0])
            + boost::multiprecision::upper(second[0])) / 2;
    const Real first_y_mid =
        (boost::multiprecision::lower(first[1])
            + boost::multiprecision::upper(first[1])) / 2;
    const Real second_y_mid =
        (boost::multiprecision::lower(second[1])
            + boost::multiprecision::upper(second[1])) / 2;
    const bool graph_in_x = sign(boundary_dy) != Sign::ZERO
        && first_x_mid != second_x_mid;
    const bool graph_in_y = sign(boundary_dx) != Sign::ZERO
        && first_y_mid != second_y_mid;
    if (!graph_in_x && !graph_in_y) {
        return false;
    }

    const Interval curve_dx = evalf<Interval>(curve.diff0, hull[0], hull[1]);
    const Interval curve_dy = evalf<Interval>(curve.diff1, hull[0], hull[1]);
    const Interval tangent_derivative =
        boundary_dx * curve_dy - boundary_dy * curve_dx;
    if (sign(tangent_derivative) != Sign::ZERO) {
        return true;
    }
    return certified_implicit_arc_property(
        boundary,
        curve,
        first,
        second,
        ArcProperty::CURVE_NONZERO);
}

template <typename Symbols, typename T, typename S>
bool certified_no_hidden_crossing(
        const EquationGradient<Symbols, T>& boundary,
        const EquationGradient<Symbols, S>& curve,
        const Vector2<Interval>& first,
        const Vector2<Interval>& second) {
    return certified_no_hidden_crossing_impl(
        boundary, curve, first, second);
}

// abdul 28/07/2026 [cancel shared sine terms with the zero-valued boundary before interval certification to avoid dependency blow-up on long generalized edges]
inline bool certified_no_hidden_crossing(
        const EquationGradient<XY, Equation<Sin>>& boundary,
        const EquationGradient<XY, Equation<Sin>>& curve,
        const Vector2<Interval>& first,
        const Vector2<Interval>& second) {
    auto reduced = curve.equation;
    reduced.add(boundary.equation);
    if (reduced.is_zero()) {
        return false;
    }
    if (reduced.size() < curve.equation.size()) {
        const EquationGradient<XY, Equation<Sin>> reduced_gradient{reduced};
        if (certified_no_hidden_crossing_quick(
                boundary, reduced_gradient, first, second)) {
            return true;
        }
    }

    reduced = curve.equation;
    reduced.sub(boundary.equation);
    if (!reduced.is_zero()
            && reduced.size() < curve.equation.size()) {
        const EquationGradient<XY, Equation<Sin>> reduced_gradient{reduced};
        if (certified_no_hidden_crossing_quick(
                boundary, reduced_gradient, first, second)) {
            return true;
        }
    }

    if (certified_no_hidden_crossing_quick(
            boundary, curve, first, second)) {
        return true;
    }

    return certified_no_hidden_crossing_impl(
        boundary, curve, first, second);
}

// abdul 28/07/2026 [certify exact line-substituted trigonometric expressions one parameter interval at a time]
template <typename EquationType, typename DerivativeType>
Sign curve_sign_on_parameter_piece(
        const EquationType& equation,
        const DerivativeType& derivative,
        const Interval& parameter) {
    const Real center =
        (boost::multiprecision::lower(parameter)
            + boost::multiprecision::upper(parameter)) / 2;
    const Interval center_box{center};
    const Interval centered_parameter = parameter - center;
    const Interval mean_value_enclosure =
        evalf<Interval>(equation, center_box)
        + evalf<Interval>(derivative, parameter) * centered_parameter;
    return sign(mean_value_enclosure);
}

template <typename EquationType>
bool certified_univariate_nonzero(
        const EquationType& equation,
        const Interval& full_parameter) {
    if (equation.is_zero()) {
        return false;
    }
    struct ParameterPiece final {
        Real low;
        Real high;
        uint32_t depth;
    };
    std::vector<ParameterPiece> pending{ParameterPiece{
        boost::multiprecision::lower(full_parameter),
        boost::multiprecision::upper(full_parameter),
        0}};
    constexpr uint32_t max_depth = 24;
    const auto derivative = diff(equation);

    while (!pending.empty()) {
        const auto piece = pending.back();
        pending.pop_back();
        const Interval parameter{piece.low, piece.high};
        if (sign(evalf<Interval>(equation, parameter)) != Sign::ZERO
                || curve_sign_on_parameter_piece(
                    equation, derivative, parameter) != Sign::ZERO) {
            continue;
        }
        if (piece.depth == max_depth) {
            return false;
        }
        const Real midpoint = (piece.low + piece.high) / 2;
        pending.push_back(
            ParameterPiece{piece.low, midpoint, piece.depth + 1});
        pending.push_back(
            ParameterPiece{midpoint, piece.high, piece.depth + 1});
    }
    return true;
}

// abdul 28/07/2026 [preserve straight-edge x/y correlation by proving hidden-root and uniqueness properties after exact substitution]
template <template <typename> class Trig>
bool certified_no_hidden_crossing_on_line(
        const EquationGradient<XY, LinComArrZ<XYEta>>& boundary,
        const EquationGradient<
            XY, LinComMapZ<Trig<LinComArrZ<XY>>>>& curve,
        const Vector2<Interval>& first,
        const Vector2<Interval>& second) {
    const auto parameter_hull = [&](
            const size_t coordinate) {
        return Interval{
            std::min(
                boost::multiprecision::lower(first[coordinate]),
                boost::multiprecision::lower(second[coordinate])),
            std::max(
                boost::multiprecision::upper(first[coordinate]),
                boost::multiprecision::upper(second[coordinate]))};
    };

    const Coeff64 x_coefficient =
        boundary.equation.coeff<XYEta::X>();
    const Coeff64 y_coefficient =
        boundary.equation.coeff<XYEta::Y>();
    if (y_coefficient != 0) {
        const auto univariate =
            in_terms_of<XY::X>(
                curve.equation, boundary.equation);
        return certified_univariate_nonzero(
            univariate, parameter_hull(0));
    }
    if (x_coefficient != 0) {
        const auto univariate =
            in_terms_of<XY::Y>(
                curve.equation, boundary.equation);
        return certified_univariate_nonzero(
            univariate, parameter_hull(1));
    }
    return false;
}

template <template <typename> class Trig>
bool certified_unique_crossing_on_line(
        const EquationGradient<XY, LinComArrZ<XYEta>>& boundary,
        const EquationGradient<
            XY, LinComMapZ<Trig<LinComArrZ<XY>>>>& curve,
        const Vector2<Real>& first,
        const Vector2<Real>& second,
        const Vector2<Interval>& root) {
    if (!certified_same_boundary_arc(
            boundary, first, second, root)) {
        return false;
    }

    const Coeff64 x_coefficient =
        boundary.equation.coeff<XYEta::X>();
    const Coeff64 y_coefficient =
        boundary.equation.coeff<XYEta::Y>();
    const size_t coordinate = y_coefficient != 0 ? 0 : 1;
    if (y_coefficient == 0 && x_coefficient == 0) {
        return false;
    }

    const Real low = std::min(
        first[coordinate], second[coordinate]);
    const Real high = std::max(
        first[coordinate], second[coordinate]);
    const Real root_low =
        boost::multiprecision::lower(root[coordinate]);
    const Real root_high =
        boost::multiprecision::upper(root[coordinate]);
    if (root_low <= low || root_high >= high) {
        return false;
    }

    const auto prove_unique = [&](
            const auto& univariate) {
        const Interval full_parameter{low, high};
        const auto derivative = diff(univariate);
        if (certified_univariate_nonzero(
                derivative, full_parameter)) {
            return true;
        }

        // abdul 28/07/2026 [separate a simple certified root from harmless derivative zeros elsewhere on a straight edge]
        const Real span = high - low;
        Real root_margin = span / 4;
        for (uint32_t attempt = 0; attempt < 24; ++attempt) {
            const Real candidate_low = root_low - root_margin;
            const Real candidate_high = root_high + root_margin;
            const Real local_low = std::max(
                low, candidate_low);
            const Real local_high = std::min(
                high, candidate_high);
            if (certified_univariate_nonzero(
                        derivative,
                        Interval{local_low, local_high})
                    && (local_low == low
                        || certified_univariate_nonzero(
                            univariate,
                            Interval{low, local_low}))
                    && (local_high == high
                        || certified_univariate_nonzero(
                            univariate,
                            Interval{local_high, high}))) {
                return true;
            }
            root_margin /= 2;
        }
        return false;
    };

    if (coordinate == 0) {
        return prove_unique(in_terms_of<XY::X>(
            curve.equation, boundary.equation));
    }
    return prove_unique(in_terms_of<XY::Y>(
        curve.equation, boundary.equation));
}

// abdul 28/07/2026 [prove line-curve root uniqueness in the exact substituted coordinate instead of an x/y chord box]
inline bool certified_unique_crossing_on_arc(
        const EquationGradient<XY, LinComArrZ<XYEta>>& boundary,
        const EquationGradient<XY, Equation<Sin>>& curve,
        const Vector2<Real>& first,
        const Vector2<Real>& second,
        const Vector2<Interval>& root) {
    return certified_unique_crossing_on_line(
        boundary, curve, first, second, root);
}

inline bool certified_unique_crossing_on_arc(
        const EquationGradient<XY, LinComArrZ<XYEta>>& boundary,
        const EquationGradient<XY, Equation<Cos>>& curve,
        const Vector2<Real>& first,
        const Vector2<Real>& second,
        const Vector2<Interval>& root) {
    return certified_unique_crossing_on_line(
        boundary, curve, first, second, root);
}

// abdul 28/07/2026 [preserve exact straight-edge x/y correlation by substituting the line before adaptive interval certification]
inline bool certified_no_hidden_crossing(
        const EquationGradient<XY, LinComArrZ<XYEta>>& boundary,
        const EquationGradient<XY, Equation<Sin>>& curve,
        const Vector2<Interval>& first,
        const Vector2<Interval>& second) {
    return certified_no_hidden_crossing_on_line(
        boundary, curve, first, second);
}

inline bool certified_no_hidden_crossing(
        const EquationGradient<XY, LinComArrZ<XYEta>>& boundary,
        const EquationGradient<XY, Equation<Cos>>& curve,
        const Vector2<Interval>& first,
        const Vector2<Interval>& second) {
    return certified_no_hidden_crossing_on_line(
        boundary, curve, first, second);
}

template <typename S>
bool certified_no_hidden_crossing(
        const EquationGradient<XY, LinComArrZ<XYEta>>&,
        const EquationGradient<XY, S>& curve,
        const Vector2<Interval>& first,
        const Vector2<Interval>& second) {
    struct ParameterPiece final {
        Real low;
        Real high;
        uint32_t depth;
    };

    // A linear boundary is exactly parameterized by its endpoint chord.  Use
    // adaptive interval subdivision of that parameter instead of a single
    // dependency-heavy bounding box; every accepted piece has a strict curve
    // sign, which rules out concealed even-multiplicity crossings.
    std::vector<ParameterPiece> pending{
        ParameterPiece{Real{0}, Real{1}, 0}};
    constexpr uint32_t max_depth = 24;

    while (!pending.empty()) {
        const auto piece = pending.back();
        pending.pop_back();
        const Interval parameter{piece.low, piece.high};
        const Interval complement = Interval{1} - parameter;
        const Vector2<Interval> point{
            complement * first[0] + parameter * second[0],
            complement * first[1] + parameter * second[1]};
        if (curve_sign_at_point(curve.equation, point) != Sign::ZERO) {
            continue;
        }
        if (piece.depth == max_depth) {
            return false;
        }
        const Real midpoint = (piece.low + piece.high) / 2;
        pending.push_back(
            ParameterPiece{piece.low, midpoint, piece.depth + 1});
        pending.push_back(
            ParameterPiece{midpoint, piece.high, piece.depth + 1});
    }
    return true;
}

template <typename S>
bool certified_curve_nonzero_on_chord(
        const EquationGradient<XY, S>& curve,
        const Vector2<Interval>& first,
        const Vector2<Interval>& second) {
    struct ParameterPiece final {
        Real low;
        Real high;
        uint32_t depth;
    };
    std::vector<ParameterPiece> pending{
        ParameterPiece{Real{0}, Real{1}, 0}};
    constexpr uint32_t max_depth = 26;
    while (!pending.empty()) {
        const auto piece = pending.back();
        pending.pop_back();
        const Interval parameter{piece.low, piece.high};
        const Interval complement = Interval{1} - parameter;
        const Vector2<Interval> point{
            complement * first[0] + parameter * second[0],
            complement * first[1] + parameter * second[1]};
        if (curve_sign_at_point(curve.equation, point) != Sign::ZERO) {
            continue;
        }
        if (piece.depth == max_depth) {
            return false;
        }
        const Real midpoint = (piece.low + piece.high) / 2;
        pending.push_back(
            ParameterPiece{piece.low, midpoint, piece.depth + 1});
        pending.push_back(
            ParameterPiece{midpoint, piece.high, piece.depth + 1});
    }
    return true;
}

inline bool same_single_cosine_zero_branch(
        const Equation<Cos>& factor,
        const Vector2<Interval>& first,
        const Vector2<Interval>& second) {
    if (factor.size() != 1
            || curve_sign_at_point(factor, first) != Sign::ZERO
            || curve_sign_at_point(factor, second) != Sign::ZERO) {
        return false;
    }
    const auto& argument = factor.begin()->first.arg;
    const auto interval_mid = [](const Interval& value) {
        return (boost::multiprecision::lower(value)
            + boost::multiprecision::upper(value)) / 2;
    };
    const Real first_phase =
        argument.coeff<XY::X>() * interval_mid(first[0])
        + argument.coeff<XY::Y>() * interval_mid(first[1]);
    const Real second_phase =
        argument.coeff<XY::X>() * interval_mid(second[0])
        + argument.coeff<XY::Y>() * interval_mid(second[1]);
    return abs(first_phase - second_phase)
        < boost::math::constants::half_pi<Real>();
}

template <typename S>
bool certified_no_hidden_crossing_cosine_boundary_impl(
        const EquationGradient<XY, Equation<Cos>>& boundary,
        const EquationGradient<XY, S>& curve,
        const Vector2<Interval>& first,
        const Vector2<Interval>& second) {
    if (boundary.equation.size() == 1
            && same_single_cosine_zero_branch(
                boundary.equation, first, second)) {
        return certified_curve_nonzero_on_chord(
            curve, first, second);
    }

    if (boundary.equation.size() == 2) {
        auto iterator = boundary.equation.begin();
        const auto first_term = *iterator;
        ++iterator;
        const auto second_term = *iterator;
        if (first_term.second == second_term.second) {
            const auto& first_argument = first_term.first.arg;
            const auto& second_argument = second_term.first.arg;
            const Coeff64 sum_x =
                first_argument.coeff<XY::X>()
                + second_argument.coeff<XY::X>();
            const Coeff64 sum_y =
                first_argument.coeff<XY::Y>()
                + second_argument.coeff<XY::Y>();
            const Coeff64 difference_x =
                first_argument.coeff<XY::X>()
                - second_argument.coeff<XY::X>();
            const Coeff64 difference_y =
                first_argument.coeff<XY::Y>()
                - second_argument.coeff<XY::Y>();
            if (sum_x % 2 == 0 && sum_y % 2 == 0
                    && difference_x % 2 == 0
                    && difference_y % 2 == 0) {
                const Equation<Cos> sum_factor{{
                    {Cos<LinComArrZ<XY>>{
                        LinComArrZ<XY>{sum_x / 2, sum_y / 2}}, 1}}};
                const Equation<Cos> difference_factor{{
                    {Cos<LinComArrZ<XY>>{
                        LinComArrZ<XY>{
                            difference_x / 2, difference_y / 2}}, 1}}};
                const Vector2<Interval> hull =
                    interval_hull(first, second);

                const bool sum_is_active =
                    same_single_cosine_zero_branch(
                        sum_factor, first, second)
                    && curve_sign_at_point(
                        difference_factor, hull) != Sign::ZERO;
                const bool difference_is_active =
                    same_single_cosine_zero_branch(
                        difference_factor, first, second)
                    && curve_sign_at_point(
                        sum_factor, hull) != Sign::ZERO;
                if (sum_is_active || difference_is_active) {
                    // cos(u)+cos(v)=2cos((u+v)/2)cos((u-v)/2).
                    // A nonzero companion factor proves which straight
                    // zero branch owns both generalized-edge endpoints.
                    return certified_curve_nonzero_on_chord(
                        curve, first, second);
                }
            }
        }
    }
    return certified_implicit_arc_property(
        boundary,
        curve,
        first,
        second,
        ArcProperty::CURVE_NONZERO);
}

template <typename S>
bool certified_no_hidden_crossing(
        const EquationGradient<XY, Equation<Cos>>& boundary,
        const EquationGradient<XY, S>& curve,
        const Vector2<Interval>& first,
        const Vector2<Interval>& second) {
    return certified_no_hidden_crossing_cosine_boundary_impl(
        boundary, curve, first, second);
}

// abdul 28/07/2026 [cancel a zero-valued cosine boundary before interval certification to avoid shared-term dependency blow-up]
inline bool certified_no_hidden_crossing(
        const EquationGradient<XY, Equation<Cos>>& boundary,
        const EquationGradient<XY, Equation<Cos>>& curve,
        const Vector2<Interval>& first,
        const Vector2<Interval>& second) {
    auto reduced = curve.equation;
    reduced.add(boundary.equation);
    if (reduced.is_zero()) {
        return false;
    }
    if (reduced.size() < curve.equation.size()) {
        const EquationGradient<XY, Equation<Cos>> reduced_gradient{reduced};
        if (certified_no_hidden_crossing_quick(
                boundary, reduced_gradient, first, second)) {
            return true;
        }
    }

    reduced = curve.equation;
    reduced.sub(boundary.equation);
    if (!reduced.is_zero()
            && reduced.size() < curve.equation.size()) {
        const EquationGradient<XY, Equation<Cos>> reduced_gradient{reduced};
        if (certified_no_hidden_crossing_quick(
                boundary, reduced_gradient, first, second)) {
            return true;
        }
    }

    if (certified_no_hidden_crossing_quick(
            boundary, curve, first, second)) {
        return true;
    }

    return certified_no_hidden_crossing_cosine_boundary_impl(
        boundary, curve, first, second);
}

// abdul 28/07/2026 [return only Krawczyk-certified roots that satisfy the requested connected-arc crossing contract]
template <typename Symbols, typename T, typename S>
boost::optional<Vector2<Interval>> intersection_unchecked(
        const EquationGradient<Symbols, T>& eq0,
        const EquationGradient<Symbols, S>& eq1,
        const Vector2<Real>& seed_a,
        const Vector2<Real>& seed_b,
        const Vector2<Real>& arc_a,
        const Vector2<Real>& arc_b,
        const bool require_unique_crossing) {

    // The midpoint between a and b is the inital approximation
    const Vector2<Real> init = (seed_a + seed_b) / 2;

    const auto inside = [&](const Vector2<Real>& x_trial) {
        // The point x_trial is inside the solution region if both of these are positive
        return point_sign_line(seed_a, seed_b, x_trial) > 0
            && point_sign_line(seed_b, seed_a, x_trial) > 0;
    };

    // The Newton object must be temporary, since it takes references
    const auto x = Newton<Symbols, T, S, decltype(inside)>{eq0, eq1, inside}.solve(init);
    const auto certified = certify_common_root(eq0, eq1, x);
    if (!certified) {
        return boost::none;
    }
    const bool on_arc = require_unique_crossing
        ? certified_unique_crossing_on_arc(eq0, eq1, arc_a, arc_b, *certified)
        : certified_same_boundary_arc(eq0, arc_a, arc_b, *certified);
    return on_arc ? certified : boost::none;
}

/**
 * @brief Computes the production-compatible Newton intersection enclosure used by Abdul Windows and the Linux release.
 * @tparam Symbols Symbol set shared by the two equations.
 * @tparam T First equation representation.
 * @tparam S Second equation representation.
 * @param eq0 First equation and gradient; borrowed and never modified.
 * @param eq1 Second equation and gradient; borrowed and never modified.
 * @param a First seed-segment endpoint.
 * @param b Second seed-segment endpoint.
 * @return A fixed-width interval enclosure around Newton's converged common-root approximation.
 * @throws std::runtime_error When Newton cannot converge inside the supplied seed segment.
 * @invariant The returned radius matches the established evaluator tolerance used before strict arc certification was introduced.
 * @note No references or caller-owned storage are retained.
 */
// abdul 31/07/2026 [restore the legacy-valid intersection enclosure after strict arc uniqueness rejected nextIter MRRs]
template <typename Symbols, typename T, typename S>
Vector2<Interval> intersection_legacy(
        const EquationGradient<Symbols, T>& eq0,
        const EquationGradient<Symbols, S>& eq1,
        const Vector2<Real>& a,
        const Vector2<Real>& b) {
    // Seed Newton at the middle of the boundary interval as in the established implementation.
    const Vector2<Real> init = (a + b) / 2;
    // Restrict Newton iterates to the open seed segment so an unrelated root is not selected.
    const auto inside = [&](const Vector2<Real>& x_trial) {
        // Require the trial to lie between the oriented endpoint half-planes.
        return point_sign_line(a, b, x_trial) > 0
            && point_sign_line(b, a, x_trial) > 0;
    };
    // Solve both equations simultaneously; the temporary owns no state after this statement.
    const auto root =
        Newton<Symbols, T, S, decltype(inside)>{eq0, eq1, inside}
            .solve(init);
    // Match the long-standing practical tolerance accepted by interval sign verification.
    const Real radius{"1e-25"};
    // Enclose the first root coordinate without transferring ownership.
    const Interval root_x{root[0] - radius, root[0] + radius};
    // Enclose the second root coordinate using the same symmetric tolerance.
    const Interval root_y{root[1] - radius, root[1] + radius};
    // Return the independently owned two-dimensional enclosure.
    return {root_x, root_y};
}

/**
 * @brief Finds a non-endpoint common root using the established inward Newton seed sequence.
 * @tparam Symbols Symbol set shared by the two equations.
 * @tparam T Boundary equation representation.
 * @tparam S Clipping-curve equation representation.
 * @param eq0 Boundary equation and gradient; borrowed and never modified.
 * @param eq1 Clipping equation and gradient; borrowed and never modified.
 * @param v Endpoint already known to lie on the clipping curve.
 * @param w Opposite endpoint of the boundary segment.
 * @return A verified interval enclosure of the distinct interior common root.
 * @throws std::runtime_error When every established inward seed fails to produce an enclosure containing both zeros.
 * @invariant Seeds avoid the known zero endpoint so Newton selects the additional interior crossing.
 * @note The returned intervals own their numeric storage and retain the established one-quarter and inward-fudge search order.
 */
// abdul 31/07/2026 [restore the old main and Linux zero-endpoint intersection path consistently with ordinary nextIter roots]
template <typename Symbols, typename T, typename S>
Vector2<Interval> intersection_zero(const EquationGradient<Symbols, T>& eq0, const EquationGradient<Symbols, S>& eq1, const Vector2<Real>& v, const Vector2<Real>& w) {
    // Split the segment so the first Newton seed stays away from the known zero endpoint.
    const Vector2<Real> midpoint = (v + w) / 2;
    // Try the midpoint-to-opposite-end interval first; this is the normal interior-root basin.
    auto inter = intersection_legacy(eq0, eq1, midpoint, w);
    // Verify that the first equation contains zero on the candidate enclosure.
    auto eq0_sign = curve_sign_at_point(eq0.equation, inter);
    // Verify that the second equation contains zero on the same enclosure.
    auto eq1_sign = curve_sign_at_point(eq1.equation, inter);
    // Accept only a common interval root of both equations.
    if (eq0_sign == Sign::ZERO && eq1_sign == Sign::ZERO) {
        // Return the verified normal-basin result.
        return inter;
    }
    // Build the left edge of the central-quarter fallback seed.
    const Vector2<Real> left_quarter = (v + midpoint) / 2;
    // Build the right edge of the central-quarter fallback seed.
    const Vector2<Real> right_quarter = (midpoint + w) / 2;
    // Retry the documented difficult cases in the central quarter.
    inter = intersection_legacy(eq0, eq1, left_quarter, right_quarter);
    // Re-evaluate the first equation over the central-quarter enclosure.
    eq0_sign = curve_sign_at_point(eq0.equation, inter);
    // Re-evaluate the second equation over the central-quarter enclosure.
    eq1_sign = curve_sign_at_point(eq1.equation, inter);
    // Accept the central-quarter candidate only when both equations contain zero.
    if (eq0_sign == Sign::ZERO && eq1_sign == Sign::ZERO) {
        // Return the independently owned central-quarter enclosure.
        return inter;
    }
    // Compute the segment direction for the final seed that moves just inside the known zero endpoint.
    const auto direction = w - v;
    // Reproduce the established one-ten-thousandth inward displacement.
    const auto fudge = direction / 10000.0;
    // Move the known zero endpoint inward so Newton does not converge back to it.
    const auto v_fudge = v + fudge;
    // Try the final inward-to-midpoint basin used by the established implementation.
    inter = intersection_legacy(eq0, eq1, v_fudge, midpoint);
    // Re-evaluate the first equation over the inward-fudge enclosure.
    eq0_sign = curve_sign_at_point(eq0.equation, inter);
    // Re-evaluate the second equation over the inward-fudge enclosure.
    eq1_sign = curve_sign_at_point(eq1.equation, inter);
    // Accept the final candidate only when it encloses a common root.
    if (eq0_sign == Sign::ZERO && eq1_sign == Sign::ZERO) {
        // Return the independently owned inward-fudge enclosure.
        return inter;
    }
    // Assemble the established diagnostic only after every inward seed fails verification.
    std::ostringstream err;
    // Preserve both equations, the last candidate, and the original endpoints for diagnosis.
    err << "Signs " << eq0_sign << ", " << eq1_sign
        << " for:" << '\n'
        << eq0.equation << '\n'
        << eq1.equation << '\n'
        << "at intersection " << inter << '\n'
        << "between points " << v << ", " << w;
    // Fail visibly rather than accepting an enclosure that does not contain both zeros.
    throw std::runtime_error(err.str());
}

/**
 * @brief Finds a common boundary/curve root using the three established Newton seed intervals.
 * @tparam Symbols Symbol set shared by the two equations.
 * @tparam T Boundary equation representation.
 * @tparam S Clipping-curve equation representation.
 * @param eq0 Boundary equation and gradient; borrowed and never modified.
 * @param eq1 Clipping equation and gradient; borrowed and never modified.
 * @param v First connected boundary endpoint.
 * @param w Second connected boundary endpoint.
 * @return An interval enclosure whose two equations both evaluate to an interval containing zero.
 * @throws std::runtime_error When none of the full, first-half, or second-half seeds produces a verified common root.
 * @invariant The legacy evaluator verification, rather than experimental arc-uniqueness certification, decides acceptance.
 * @note The returned intervals own their numeric storage.
 */
// abdul 31/07/2026 [restore the production intersection acceptance used by old main and Linux for valid long MRRs]
template <typename Symbols, typename T, typename S>
Vector2<Interval> intersection(const EquationGradient<Symbols, T>& eq0, const EquationGradient<Symbols, S>& eq1, const Vector2<Real>& v, const Vector2<Real>& w) {
    // Try the complete boundary interval first because it is the normal and cheapest seed.
    auto inter = intersection_legacy(eq0, eq1, v, w);
    // Evaluate the boundary equation over the returned enclosure.
    auto eq0_sign = curve_sign_at_point(eq0.equation, inter);
    // Evaluate the clipping equation over the same enclosure.
    auto eq1_sign = curve_sign_at_point(eq1.equation, inter);
    // Accept only a common interval root of both exact equations.
    if (eq0_sign == Sign::ZERO && eq1_sign == Sign::ZERO) {
        // Return the verified full-interval result.
        return inter;
    }

    // Bisect the seed interval for documented cases whose full chord gives Newton a poor initial point.
    const Vector2<Real> m = (v + w) / 2;
    // Retry Newton on the first half of the connected boundary interval.
    inter = intersection_legacy(eq0, eq1, v, m);
    // Re-evaluate the boundary equation on the first-half candidate.
    eq0_sign = curve_sign_at_point(eq0.equation, inter);
    // Re-evaluate the clipping equation on the first-half candidate.
    eq1_sign = curve_sign_at_point(eq1.equation, inter);
    // Accept a verified common root from the first half.
    if (eq0_sign == Sign::ZERO && eq1_sign == Sign::ZERO) {
        // Return the independently owned first-half enclosure.
        return inter;
    }

    // Retry Newton on the second half of the connected boundary interval.
    inter = intersection_legacy(eq0, eq1, m, w);
    // Re-evaluate the boundary equation on the second-half candidate.
    eq0_sign = curve_sign_at_point(eq0.equation, inter);
    // Re-evaluate the clipping equation on the second-half candidate.
    eq1_sign = curve_sign_at_point(eq1.equation, inter);
    // Accept a verified common root from the second half.
    if (eq0_sign == Sign::ZERO && eq1_sign == Sign::ZERO) {
        // Return the independently owned second-half enclosure.
        return inter;
    }

    // Assemble a complete diagnostic only after every established Newton basin fails verification.
    std::ostringstream err;
    // Preserve both equations, the last candidate, and the original endpoints for diagnosis.
    err << "Signs " << eq0_sign << ", " << eq1_sign
        << " for:" << '\n'
        << eq0.equation << '\n'
        << eq1.equation << '\n'
        << "at intersection " << inter << '\n'
        << "between points " << v << ", " << w;
    // Fail visibly rather than publishing an enclosure that does not contain both zeros.
    throw std::runtime_error(err.str());
}

// T is the type of curve
// S is the type of equation in the variant
// WARNING: always make instances of this class temporaries
template <typename Symbols, typename T>
class IntersectionVariant final : public boost::static_visitor<Vector2<Interval>> {
  private:
    const EquationGradient<Symbols, T>& curve;
    const Vector2<Real>& point0;
    const Vector2<Real>& point1;

  public:
    explicit IntersectionVariant(const EquationGradient<Symbols, T>& curve_, const Vector2<Real>& point0_, const Vector2<Real>& point1_)
        : curve{curve_},
          point0{point0_},
          point1{point1_} {
    }

    // This is the same for all the types in the variant. All we need is the type, and then
    // we can template the rest
    template <typename S>
    Vector2<Interval> operator()(const EquationGradient<Symbols, S>& eq_grad) const {
        return intersection(eq_grad, curve, point0, point1);
    }
};

template <typename Symbols, typename T>
class IntersectionZeroVariant final : public boost::static_visitor<Vector2<Interval>> {
  private:
    const EquationGradient<Symbols, T>& curve;
    const Vector2<Real>& point0;
    const Vector2<Real>& point1;

  public:
    explicit IntersectionZeroVariant(const EquationGradient<Symbols, T>& curve_, const Vector2<Real>& point0_, const Vector2<Real>& point1_)
        : curve{curve_},
          point0{point0_},
          point1{point1_} {
    }

    // This is the same for all the types in the variant. All we need is the type, and then
    // we can template the rest
    template <typename S>
    Vector2<Interval> operator()(const EquationGradient<Symbols, S>& eq_grad) const {
        return intersection_zero(eq_grad, curve, point0, point1);
    }
};

// abdul 28/07/2026 [dispatch hidden-crossing certificates across boundary variants without weakening the concrete overloads]
template <typename Symbols, typename T>
class NoHiddenCrossingVariant final : public boost::static_visitor<bool> {
  private:
    const EquationGradient<Symbols, T>& curve;
    const Vector2<Interval>& point0;
    const Vector2<Interval>& point1;

  public:
    explicit NoHiddenCrossingVariant(
            const EquationGradient<Symbols, T>& curve_,
            const Vector2<Interval>& point0_,
            const Vector2<Interval>& point1_)
        : curve{curve_},
          point0{point0_},
          point1{point1_} {
    }

    template <typename S>
    bool operator()(const EquationGradient<Symbols, S>& boundary) const {
        return certified_no_hidden_crossing(
            boundary, curve, point0, point1);
    }
};
