#include "../src/arbpp.hpp"

#define BOOST_TEST_MODULE arb_test
#include <boost/test/unit_test.hpp>

#include <type_traits>

// NOTE: this may change in the future to flint/flint.h, if Arb
// gets this change as well.
#include "arb.h"
#include "flint.h"

using namespace arbpp;

template <typename T, typename U = T, typename = void>
struct is_addable
{
    static const bool value = false;
};

template <typename T, typename U>
struct is_addable<T,U,typename std::enable_if<
    std::is_same<
    decltype(std::declval<T>() + std::declval<U>()),
    decltype(std::declval<U>() + std::declval<T>())>::value
>::type>
{
    static const bool value = true;
};

BOOST_AUTO_TEST_CASE(arb_ctor_assignment_test)
{
    // Some type-traits checks.
    BOOST_CHECK((!std::is_constructible<arb,long double>::value));
    BOOST_CHECK((!std::is_constructible<arb,long long>::value));
    BOOST_CHECK((std::is_constructible<arb,long>::value));
    BOOST_CHECK((std::is_constructible<arb,char>::value));
    BOOST_CHECK((std::is_constructible<arb,unsigned char>::value));
    // Ctors.
    arb a0;
    BOOST_CHECK(::arf_is_zero(arb_midref(a0.get_arb_t())));
    BOOST_CHECK(::mag_is_zero(arb_radref(a0.get_arb_t())));
    BOOST_CHECK_EQUAL(a0.get_precision(),arb::get_default_precision());
    arb a1{a0};
    BOOST_CHECK(::arf_is_zero(arb_midref(a1.get_arb_t())));
    BOOST_CHECK(::mag_is_zero(arb_radref(a1.get_arb_t())));
    BOOST_CHECK_EQUAL(a1.get_precision(),arb::get_default_precision());
    a1 = arb{1};
    a1.set_precision(100);
    arb a2{a1};
    BOOST_CHECK(::arf_is_one(arb_midref(a2.get_arb_t())));
    BOOST_CHECK(::mag_is_zero(arb_radref(a2.get_arb_t())));
    BOOST_CHECK_EQUAL(a2.get_precision(),100);
    arb a3{std::move(a2)};
    BOOST_CHECK(::arf_is_one(arb_midref(a3.get_arb_t())));
    BOOST_CHECK(::mag_is_zero(arb_radref(a3.get_arb_t())));
    BOOST_CHECK_EQUAL(a3.get_precision(),100);
    BOOST_CHECK(::arf_is_zero(arb_midref(a2.get_arb_t())));
    BOOST_CHECK(::mag_is_zero(arb_radref(a2.get_arb_t())));
    BOOST_CHECK_EQUAL(a2.get_precision(),arb::get_default_precision());
    // Generic assignment.
    a1 = arb{0.5};
    a1.set_precision(100);
    a1.add_error(.1);
    BOOST_CHECK(!::mag_is_zero(arb_radref(a1.get_arb_t())));
    a1 = 1;
    BOOST_CHECK(::arf_is_one(arb_midref(a1.get_arb_t())));
    BOOST_CHECK(::mag_is_zero(arb_radref(a1.get_arb_t())));
    BOOST_CHECK_EQUAL(a1.get_precision(),53);
}

BOOST_AUTO_TEST_CASE(arb_arithmetic_test)
{
    BOOST_CHECK(is_addable<arb>::value);
    BOOST_CHECK((is_addable<arb,double>::value));
    BOOST_CHECK((!is_addable<arb,long double>::value));
}

BOOST_AUTO_TEST_CASE(arb_base_test)
{
    
    arb a0{20};
    a0 += 1;
    std::cout << a0 << '\n';
    arb a1{0.2};
    a1.set_precision(70);
    //a1.add_error(0.1f);
    std::cout << a1 << '\n';
    std::cout << (a0 + a1) << '\n';
    std::cout << (a0 + 6) << '\n';
    std::cout << (6 + a0) << '\n';
    std::cout << (6. + a0) << '\n';
    std::cout << (std::numeric_limits<long>::max() + a0) << '\n';
    a0.set_precision(100);
    std::cout << (std::numeric_limits<long>::max() + a0) << '\n';
    std::cout << (a0 + a1) << '\n';
    std::cout << cos(arb{0.0000001}) << '\n';
    a0.add_error(1./0.);
    std::cout << a0 << '\n';
}

// Keep this for last, in order to have proper
// memory cleanup and make valgrind happy.
BOOST_AUTO_TEST_CASE(arb_cleanup)
{
    ::flint_cleanup();
}
