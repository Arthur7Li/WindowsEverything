#include "gradient.hpp"
#include "evalf.hpp"
#include "trig_identities.hpp"

// So far, when gradf(a, b) = 0, we do a full factor, and
// grad f_factored(a, b) /= 0. So other lines in there (ones other
// than sin(x), sin(y), or sin(x + y)) don't show up. Alright, so
// that means we're pretty safe.

// So the premise is this: we have a function f at a point (a, b)
// and f(a, b) = 0 and grad f(a, b) = 0. What does this mean? Well,
// it could mean that f has a local min, max, or saddle point at
// (a, b), and is also shifted down. Or, it could mean there is
// an intersection of a two curves there. How do we determine which
// one? Well, we could check the Hessian, and if det H(a, b) = 0,
// then it is semidefinite. This doesn't guarantee that we have
// an intersection, but det H(a, b) != 0 would indicate we don't.

// (0, 0)
// could get all three lines for this case
static bool is_zero_zero(const Vector2<Interval>& point) {

    const auto coord0_sign = sign(point[0]);
    const auto coord1_sign = sign(point[1]);

    return coord0_sign == Sign::ZERO && coord1_sign == Sign::ZERO;
}

// (0, pi/2)
static bool is_zero_pi2(const Vector2<Interval>& point) {

    const auto coord0_sign = sign(point[0]);

    const Interval coord1_diff = point[1] - boost::math::constants::half_pi<Interval>();
    const auto coord1_sign = sign(coord1_diff);

    return coord0_sign == Sign::ZERO && coord1_sign == Sign::ZERO;
}

// (pi/2, 0)
static bool is_pi2_zero(const Vector2<Interval>& point) {

    const Interval coord0_diff = point[0] - boost::math::constants::half_pi<Interval>();
    const auto coord0_sign = sign(coord0_diff);

    const auto coord1_sign = sign(point[1]);

    return coord0_sign == Sign::ZERO && coord1_sign == Sign::ZERO;
}

// (pi/2, pi/2)
static bool is_pi2_pi2(const Vector2<Interval>& point) {

    const Interval coord0_diff = point[0] - boost::math::constants::half_pi<Interval>();
    const auto coord0_sign = sign(coord0_diff);

    const Interval coord1_diff = point[1] - boost::math::constants::half_pi<Interval>();
    const auto coord1_sign = sign(coord1_diff);

    return coord0_sign == Sign::ZERO && coord1_sign == Sign::ZERO;
}

namespace {

// abdul 28/07/2026 [derive the corner multiplicity search bound from stored frequencies and reject every inexact integer normalization]
// 20! is the largest factorial representable by the signed 64-bit coefficient
// type.  A trigonometric polynomial with m stored terms has at most 2m
// exponential frequencies, so a nonzero directional restriction cannot have
// multiplicity greater than 2m-1.  Search that expression-derived bound while
// refusing to overflow the exact coefficient representation.
constexpr uint32_t max_exact_factorial_order = 20;

Coeff64 exact_divide(
        const Coeff64 numerator,
        const Coeff64 denominator,
        const char* const component,
        const uint32_t factor_multiplicity) {
    if (denominator == 0 || numerator % denominator != 0) {
        std::ostringstream err;
        err << "non-exact special-corner gradient division for " << component
            << " at factor multiplicity " << factor_multiplicity
            << ": " << numerator << " / " << denominator;
        throw std::runtime_error(err.str());
    }
    return numerator / denominator;
}

Coeff64 remove_normalization_factors(
        Coeff64 value,
        const uint32_t factor_multiplicity,
        const char* const component) {
    for (uint32_t i = 0; i < factor_multiplicity; ++i) {
        value = exact_divide(value, 2, component, factor_multiplicity);
    }
    return value;
}

template <typename T>
uint32_t special_corner_search_limit(const T& equation) {
    const size_t frequency_bound = 2 * equation.size();
    return static_cast<uint32_t>(
        std::min<size_t>(frequency_bound, max_exact_factorial_order - 1));
}

} // namespace

// abdul 28/07/2026 [recover the (0,pi/2) reduced gradient with exact factorial and power-of-two divisions]
template <template <typename> class Trig>
std::pair<Coeff64, Coeff64> gradient_zero_pi2(const LinComMapZ<Trig<LinComArrZ<XY>>>& f) {
    // (0, pi/2)
    // f = sin(a)^n * g

    // TODO double check the Hessian?

    Coeff64 factorial_n = 1;

    // Same type as f
    LinComMapZ<Trig<LinComArrZ<XY>>> diff_a_same{};
    LinComMapZ<Trig<LinComArrZ<XY>>> diff_b_same{};

    // Different type than f
    auto diff_a_different = diff<XY::X>(f);
    auto diff_b_different = diff<XY::Y>(f);

    const uint32_t search_limit = special_corner_search_limit(f);
    for (uint32_t n = 1; n <= search_limit; ++n) {

        factorial_n *= n;
        const Coeff64 factorial_n_plus_one = factorial_n * (n + 1);

        Coeff64 eval_a;
        Coeff64 eval_b;
        if (n % 2 == 0) {
            diff_a_different = diff<XY::X>(diff_a_same);
            diff_b_different = diff<XY::X>(diff_b_same);

            // abdul 27/07/2026 [evaluate recovered derivatives at the actual (0,pi/2) corner instead of the unrelated (pi/2,pi/2) corner]
            eval_a = simplify_lin_com_zero_pi2(diff_a_different);
            eval_b = simplify_lin_com_zero_pi2(diff_b_different);
        } else {
            diff_a_same = diff<XY::X>(diff_a_different);
            diff_b_same = diff<XY::X>(diff_b_different);

            eval_a = simplify_lin_com_zero_pi2(diff_a_same);
            eval_b = simplify_lin_com_zero_pi2(diff_b_same);
        }

        if (eval_a != 0 || eval_b != 0) {
            auto g_a = exact_divide(eval_a, factorial_n_plus_one, "x", n);
            auto g_b = exact_divide(eval_b, factorial_n, "y", n);
            g_a = remove_normalization_factors(g_a, n, "x");
            g_b = remove_normalization_factors(g_b, n, "y");

            return {g_a, g_b};
        }
    }

    std::ostringstream err;
    err << "uncertified zero gradient after " << search_limit
        << " exact factor divisions for " << '\n'
        << f << '\n'
        << " at (0, pi/2)";

    throw std::runtime_error(err.str());
}

// abdul 28/07/2026 [recover the (pi/2,0) reduced gradient with exact factorial and power-of-two divisions]
template <template <typename> class Trig>
std::pair<Coeff64, Coeff64> gradient_pi2_zero(const LinComMapZ<Trig<LinComArrZ<XY>>>& f) {
    // (pi/2, zero)
    // f = sin(b)^n * g

    // TODO double check the Hessian?

    Coeff64 factorial_n = 1;

    // Same type as f
    LinComMapZ<Trig<LinComArrZ<XY>>> diff_a_same{};
    LinComMapZ<Trig<LinComArrZ<XY>>> diff_b_same{};

    // Different type than f
    auto diff_a_different = diff<XY::X>(f);
    auto diff_b_different = diff<XY::Y>(f);

    const uint32_t search_limit = special_corner_search_limit(f);
    for (uint32_t n = 1; n <= search_limit; ++n) {

        factorial_n *= n;
        const Coeff64 factorial_n_plus_one = factorial_n * (n + 1);

        Coeff64 eval_a;
        Coeff64 eval_b;
        if (n % 2 == 0) {
            diff_a_different = diff<XY::Y>(diff_a_same);
            diff_b_different = diff<XY::Y>(diff_b_same);

            // abdul 27/07/2026 [evaluate recovered derivatives at the actual (pi/2,0) corner instead of the unrelated (pi/2,pi/2) corner]
            eval_a = simplify_lin_com_pi2_zero(diff_a_different);
            eval_b = simplify_lin_com_pi2_zero(diff_b_different);
        } else {
            diff_a_same = diff<XY::Y>(diff_a_different);
            diff_b_same = diff<XY::Y>(diff_b_different);

            eval_a = simplify_lin_com_pi2_zero(diff_a_same);
            eval_b = simplify_lin_com_pi2_zero(diff_b_same);
        }

        if (eval_a != 0 || eval_b != 0) {
            auto g_a = exact_divide(eval_a, factorial_n, "x", n);
            auto g_b = exact_divide(eval_b, factorial_n_plus_one, "y", n);
            g_a = remove_normalization_factors(g_a, n, "x");
            g_b = remove_normalization_factors(g_b, n, "y");

            return {g_a, g_b};
        }
    }

    std::ostringstream err;
    err << "uncertified zero gradient after " << search_limit
        << " exact factor divisions for " << '\n'
        << f << '\n'
        << " at (pi/2, 0)";

    throw std::runtime_error(err.str());
}

// abdul 28/07/2026 [recover the (pi/2,pi/2) reduced gradient without silent truncation or a fixed multiplicity guess]
template <template <typename> class Trig>
std::pair<Coeff64, Coeff64> gradient_pi2_pi2(const LinComMapZ<Trig<LinComArrZ<XY>>>& f) {
    // (pi/2, pi/2)
    // f = sin(a + b)^n * g

    // TODO double check the Hessian?

    Coeff64 sign = 1;
    Coeff64 factorial_n = 1;

    // Same type as f
    LinComMapZ<Trig<LinComArrZ<XY>>> diff_a_same{};
    LinComMapZ<Trig<LinComArrZ<XY>>> diff_b_same{};

    // Different type than f
    auto diff_a_different = diff<XY::X>(f);
    auto diff_b_different = diff<XY::Y>(f);

    const uint32_t search_limit = special_corner_search_limit(f);
    for (uint32_t n = 1; n <= search_limit; ++n) {

        sign *= -1;
        factorial_n *= n;
        const Coeff64 factorial_n_plus_one = factorial_n * (n + 1);

        Coeff64 eval_a;
        Coeff64 eval_b;
        if (n % 2 == 0) {
            diff_a_different = diff<XY::X>(diff_a_same);
            diff_b_different = diff<XY::Y>(diff_b_same);

            eval_a = simplify_lin_com_pi2_pi2(diff_a_different);
            eval_b = simplify_lin_com_pi2_pi2(diff_b_different);
        } else {
            diff_a_same = diff<XY::X>(diff_a_different);
            diff_b_same = diff<XY::Y>(diff_b_different);

            eval_a = simplify_lin_com_pi2_pi2(diff_a_same);
            eval_b = simplify_lin_com_pi2_pi2(diff_b_same);
        }

        if (eval_a != 0 || eval_b != 0) {
            auto g_a = exact_divide(sign * eval_a, factorial_n_plus_one, "x", n);
            auto g_b = exact_divide(sign * eval_b, factorial_n_plus_one, "y", n);
            g_a = remove_normalization_factors(g_a, n, "x");
            g_b = remove_normalization_factors(g_b, n, "y");

            return {g_a, g_b};
        }
    }

    std::ostringstream err;
    err << "uncertified zero gradient after " << search_limit
        << " exact factor divisions for " << '\n'
        << f << '\n'
        << " at (pi/2, pi/2)";

    throw std::runtime_error(err.str());
}

template <template <typename> class Trig>
std::pair<Coeff64, Coeff64> zero_gradient(const LinComMapZ<Trig<LinComArrZ<XY>>>& lin_com, const Vector2<Interval>& point) {

    if (is_zero_zero(point)) {
        // abdul 27/07/2026 [fail closed at the three-factor corner until a unique exact factorization proves which reduced curve owns the boundary]
        std::ostringstream err;
        err << "uncertified special-corner gradient at (0, 0): multiple canonical boundary factors may be present\n"
            << lin_com;
        throw std::runtime_error(err.str());
    } else if (is_zero_pi2(point)) {
        return gradient_zero_pi2(lin_com);
    } else if (is_pi2_zero(point)) {
        return gradient_pi2_zero(lin_com);
    } else if (is_pi2_pi2(point)) {
        return gradient_pi2_pi2(lin_com);
    }

    // throw exception
    std::ostringstream err{};
    err << "undealt with case in zero_gradient:\n"
        << lin_com << '\n'
        << point;
    throw std::runtime_error(err.str());
}

Vector2<Interval> gradient(const EquationGradient<XY, LinComArrZ<XYEta>>& eq_grad, const Vector2<Interval>& point) {
    const Interval x = evalf<Interval>(eq_grad.diff0, point[0], point[1]);
    const Interval y = evalf<Interval>(eq_grad.diff1, point[0], point[1]);

    const auto sign_x = sign(x);
    const auto sign_y = sign(y);

    if (sign_x != Sign::ZERO || sign_y != Sign::ZERO) {
        return {x, y};
    }

    // This should always have a nonzero gradient
    std::ostringstream err;
    err << "zero gradient for " << eq_grad.equation << " at " << point;
    throw std::runtime_error(err.str());
}

template <template <typename> class Trig>
Vector2<Interval> gradient(const EquationGradient<XY, Equation<Trig>>& eq_grad, const Vector2<Interval>& point) {

    const Interval x = evalf<Interval>(eq_grad.diff0, point[0], point[1]);
    const Interval y = evalf<Interval>(eq_grad.diff1, point[0], point[1]);

    const auto sign_x = sign(x);
    const auto sign_y = sign(y);

    if (sign_x != Sign::ZERO || sign_y != Sign::ZERO) {
        return {x, y};
    }

    // Zero gradient
    // TODO make sure we don't repeat calculating derivatives
    const auto grad = zero_gradient(eq_grad.equation, point);

    return {grad.first, grad.second};
}

template Vector2<Interval> gradient(const EquationGradient<XY, Equation<Sin>>& eq_grad, const Vector2<Interval>& point);
template Vector2<Interval> gradient(const EquationGradient<XY, Equation<Cos>>& eq_grad, const Vector2<Interval>& point);
