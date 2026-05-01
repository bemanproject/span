// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/span/span.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

namespace bsp = beman::span;

// ---------------------------------------------------------------------------
// Type traits / constants
// ---------------------------------------------------------------------------

TEST(SpanTest, static_extent_constant) {
    // Wrap in parentheses to prevent template commas from confusing the macro parser.
    EXPECT_EQ((bsp::span<int, 5>::extent), 5u);
    EXPECT_EQ((bsp::span<int>::extent), bsp::dynamic_extent);
    EXPECT_EQ(bsp::dynamic_extent, std::numeric_limits<std::size_t>::max());
}

// ---------------------------------------------------------------------------
// Default construction
// ---------------------------------------------------------------------------

TEST(SpanConstruction, default_dynamic) {
    bsp::span<int> s;
    EXPECT_EQ(s.size(), 0u);
    EXPECT_EQ(s.data(), nullptr);
    EXPECT_TRUE(s.empty());
}

TEST(SpanConstruction, default_static_zero) {
    bsp::span<int, 0> s;
    EXPECT_EQ(s.size(), 0u);
    EXPECT_TRUE(s.empty());
}

// ---------------------------------------------------------------------------
// Pointer + count
// ---------------------------------------------------------------------------

TEST(SpanConstruction, pointer_and_count_dynamic) {
    int            arr[] = {1, 2, 3, 4, 5};
    bsp::span<int> s(arr, 5);
    EXPECT_EQ(s.size(), 5u);
    EXPECT_EQ(s.data(), arr);
    EXPECT_FALSE(s.empty());
}

TEST(SpanConstruction, pointer_and_count_static) {
    int               arr[] = {10, 20, 30};
    bsp::span<int, 3> s(arr, 3);
    EXPECT_EQ(s.size(), 3u);
    EXPECT_EQ(s.data(), arr);
}

// ---------------------------------------------------------------------------
// Pointer pair (first, last)
// ---------------------------------------------------------------------------

TEST(SpanConstruction, pointer_pair) {
    int            arr[] = {5, 6, 7, 8};
    bsp::span<int> s(arr, arr + 4);
    EXPECT_EQ(s.size(), 4u);
    EXPECT_EQ(s[0], 5);
    EXPECT_EQ(s[3], 8);
}

// ---------------------------------------------------------------------------
// C-array
// ---------------------------------------------------------------------------

TEST(SpanConstruction, c_array_dynamic) {
    int            arr[] = {1, 2, 3};
    bsp::span<int> s(arr);
    EXPECT_EQ(s.size(), 3u);
    EXPECT_EQ(s.data(), arr);
}

TEST(SpanConstruction, c_array_static) {
    int               arr[] = {10, 20};
    bsp::span<int, 2> s(arr);
    EXPECT_EQ(s.size(), 2u);
}

TEST(SpanConstruction, c_array_deduction) {
    int       arr[] = {1, 2, 3, 4};
    bsp::span s(arr);
    static_assert(std::is_same_v<decltype(s), bsp::span<int, 4>>);
    EXPECT_EQ(s.size(), 4u);
}

// ---------------------------------------------------------------------------
// std::array
// ---------------------------------------------------------------------------

TEST(SpanConstruction, std_array_mutable) {
    std::array<int, 4> arr = {1, 2, 3, 4};
    bsp::span<int>     s(arr);
    EXPECT_EQ(s.size(), 4u);
    EXPECT_EQ(s.data(), arr.data());
}

TEST(SpanConstruction, std_array_const) {
    const std::array<int, 3> arr = {7, 8, 9};
    bsp::span<const int>     s(arr);
    EXPECT_EQ(s.size(), 3u);
    EXPECT_EQ(s[0], 7);
}

TEST(SpanConstruction, std_array_deduction_mutable) {
    std::array<double, 2> arr = {1.0, 2.0};
    bsp::span             s(arr);
    static_assert(std::is_same_v<decltype(s), bsp::span<double, 2>>);
}

TEST(SpanConstruction, std_array_deduction_const) {
    const std::array<double, 2> arr = {3.0, 4.0};
    bsp::span                   s(arr);
    static_assert(std::is_same_v<decltype(s), bsp::span<const double, 2>>);
}

// ---------------------------------------------------------------------------
// Range (vector)
// ---------------------------------------------------------------------------

TEST(SpanConstruction, from_vector) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    bsp::span<int>   s(v);
    EXPECT_EQ(s.size(), 5u);
    EXPECT_EQ(s.data(), v.data());
}

TEST(SpanConstruction, from_const_vector) {
    const std::vector<int> v = {10, 20};
    bsp::span<const int>   s(v);
    EXPECT_EQ(s.size(), 2u);
    EXPECT_EQ(s[1], 20);
}

// ---------------------------------------------------------------------------
// Copy construction / conversion
// ---------------------------------------------------------------------------

TEST(SpanConstruction, copy_dynamic_from_dynamic) {
    int            arr[] = {1, 2, 3};
    bsp::span<int> a(arr);
    bsp::span<int> b(a);
    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(b.data(), arr);
}

TEST(SpanConstruction, const_from_mutable) {
    int                  arr[] = {4, 5, 6};
    bsp::span<int>       mutable_s(arr);
    bsp::span<const int> const_s(mutable_s);
    EXPECT_EQ(const_s.size(), 3u);
    EXPECT_EQ(const_s[0], 4);
}

TEST(SpanConstruction, dynamic_from_static) {
    int               arr[] = {1, 2, 3, 4};
    bsp::span<int, 4> fixed(arr);
    bsp::span<int>    dynamic(fixed);
    EXPECT_EQ(dynamic.size(), 4u);
}

TEST(SpanInitList, dynamic_from_braced_list) {
    auto verify = [](bsp::span<const int> s) {
        EXPECT_EQ(s.size(), 3u);
        EXPECT_EQ(s[0], 1);
        EXPECT_EQ(s[1], 2);
        EXPECT_EQ(s[2], 3);
    };
    verify({1, 2, 3});
}

TEST(SpanInitList, fixed_extent_from_braced_list) {
    auto verify = [](bsp::span<const int, 3> s) {
        EXPECT_EQ(s.size(), 3u);
        EXPECT_EQ(s[0], 1);
        EXPECT_EQ(s[2], 3);
    };
    verify(bsp::span<const int, 3>{1, 2, 3});
}

TEST(SpanInitList, const_bool_from_braced_list) {
    auto verify = [](bsp::span<const bool> s) {
        EXPECT_EQ(s.size(), 3u);
        EXPECT_TRUE(s[0]);
        EXPECT_FALSE(s[1]);
        EXPECT_TRUE(s[2]);
    };
    verify({true, false, true});
}

TEST(SpanInitList, named_initializer_list_keeps_array_alive) {
    std::initializer_list<int> il = {10, 20, 30};
    bsp::span<const int>       s(il);
    EXPECT_EQ(s.size(), 3u);
    EXPECT_EQ(s[0], 10);
    EXPECT_EQ(s[2], 30);
}

TEST(SpanInitList, pointer_and_size_resolves_to_pointer_count_ctor) {
    bool                  data[4] = {true, false, true, false};
    bool*                 ptr     = data;
    std::size_t           n       = 4;
    bsp::span<const bool> s(ptr, n);
    EXPECT_EQ(s.size(), 4u);
    EXPECT_EQ(s.data(), data);
}

namespace init_list_detail {
template <class T, class IL, class = void>
struct constructible_from_il : std::false_type {};

template <class T, class IL>
struct constructible_from_il<T, IL, std::void_t<decltype(T(std::declval<IL>()))>> : std::true_type {};
} // namespace init_list_detail

TEST(SpanInitList, non_const_element_type_rejected) {
    static_assert(init_list_detail::constructible_from_il<bsp::span<const int>, std::initializer_list<int>>::value);
    static_assert(!init_list_detail::constructible_from_il<bsp::span<int>, std::initializer_list<int>>::value);
}

TEST(SpanInitList, exact_type_match_required) {
    static_assert(!init_list_detail::constructible_from_il<bsp::span<const float>, std::initializer_list<int>>::value);
    static_assert(!init_list_detail::constructible_from_il<bsp::span<const int>, std::initializer_list<long>>::value);
}

// ---------------------------------------------------------------------------
// Element access
// ---------------------------------------------------------------------------

TEST(SpanElementAccess, subscript) {
    int            arr[] = {10, 20, 30, 40};
    bsp::span<int> s(arr);
    EXPECT_EQ(s[0], 10);
    EXPECT_EQ(s[3], 40);
}

TEST(SpanElementAccess, front_and_back) {
    int            arr[] = {100, 200, 300};
    bsp::span<int> s(arr);
    EXPECT_EQ(s.front(), 100);
    EXPECT_EQ(s.back(), 300);
}

TEST(SpanElementAccess, data_pointer) {
    std::array<int, 3> arr = {1, 2, 3};
    bsp::span<int>     s(arr);
    EXPECT_EQ(s.data(), arr.data());
}

TEST(SpanElementAccess, write_through_span) {
    int            arr[] = {1, 2, 3};
    bsp::span<int> s(arr);
    s[1] = 99;
    EXPECT_EQ(arr[1], 99);
}

// at() bounds-checked access (P2821R5, C++26)
TEST(SpanAt, in_bounds_returns_element) {
    int            arr[] = {10, 20, 30};
    bsp::span<int> s(arr);
    EXPECT_EQ(s.at(0), 10);
    EXPECT_EQ(s.at(1), 20);
    EXPECT_EQ(s.at(2), 30);
}

TEST(SpanAt, returns_lvalue_reference_for_mutable_span) {
    int            arr[] = {1, 2, 3};
    bsp::span<int> s(arr);
    s.at(1) = 42;
    EXPECT_EQ(arr[1], 42);
    static_assert(std::is_same_v<decltype(s.at(0)), int&>);
}

TEST(SpanAt, throws_out_of_range_at_size_boundary) {
    int            arr[] = {1, 2, 3};
    bsp::span<int> s(arr);
    EXPECT_THROW(s.at(3), std::out_of_range);
}

TEST(SpanAt, throws_out_of_range_well_past_size) {
    int            arr[] = {1, 2, 3};
    bsp::span<int> s(arr);
    EXPECT_THROW(s.at(100), std::out_of_range);
}

TEST(SpanAt, empty_span_always_throws) {
    bsp::span<int> s;
    EXPECT_THROW(s.at(0), std::out_of_range);
}

TEST(SpanAt, fixed_extent_in_bounds_and_out_of_range) {
    int               arr[] = {7, 8, 9, 10};
    bsp::span<int, 4> s(arr);
    EXPECT_EQ(s.at(3), 10);
    EXPECT_THROW(s.at(4), std::out_of_range);
}

TEST(SpanAt, const_span_returns_reference_to_const) {
    const int            arr[] = {1, 2, 3};
    bsp::span<const int> s(arr);
    EXPECT_EQ(s.at(2), 3);
    static_assert(std::is_same_v<decltype(s.at(0)), const int&>);
}

// ---------------------------------------------------------------------------
// Observers
// ---------------------------------------------------------------------------

TEST(SpanObservers, size) {
    int            arr[] = {0, 1, 2, 3, 4};
    bsp::span<int> s(arr);
    EXPECT_EQ(s.size(), 5u);
}

TEST(SpanObservers, size_bytes) {
    int            arr[] = {0, 1, 2};
    bsp::span<int> s(arr);
    EXPECT_EQ(s.size_bytes(), 3 * sizeof(int));
}

TEST(SpanObservers, empty_true) {
    bsp::span<int> s;
    EXPECT_TRUE(s.empty());
}

TEST(SpanObservers, empty_false) {
    int            arr[] = {1};
    bsp::span<int> s(arr);
    EXPECT_FALSE(s.empty());
}

// ---------------------------------------------------------------------------
// Iterators
// ---------------------------------------------------------------------------

TEST(SpanIterators, range_for) {
    int            arr[] = {1, 2, 3, 4, 5};
    bsp::span<int> s(arr);
    int            sum = 0;
    for (int v : s)
        sum += v;
    EXPECT_EQ(sum, 15);
}

TEST(SpanIterators, begin_end) {
    int            arr[] = {5, 10, 15};
    bsp::span<int> s(arr);
    auto           it = s.begin();
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(*it, 10);
    EXPECT_EQ(s.end() - s.begin(), 3);
}

TEST(SpanIterators, reverse_iteration) {
    int              arr[] = {1, 2, 3};
    bsp::span<int>   s(arr);
    std::vector<int> rev(s.rbegin(), s.rend());
    EXPECT_EQ(rev[0], 3);
    EXPECT_EQ(rev[1], 2);
    EXPECT_EQ(rev[2], 1);
}

TEST(SpanIterators, cbegin_cend) {
    int            arr[] = {7, 8, 9};
    bsp::span<int> s(arr);
    EXPECT_EQ(*s.cbegin(), 7);
    EXPECT_EQ(s.cend() - s.cbegin(), 3);

    static_assert(std::is_same_v<decltype(s.cbegin()), bsp::span<int>::const_iterator>);
    static_assert(std::is_same_v<bsp::span<int>::const_iterator, const int*>);
    static_assert(std::is_same_v<decltype(s.crbegin()), bsp::span<int>::const_reverse_iterator>);
}

TEST(SpanIterators, std_algorithm_sort) {
    int            arr[] = {5, 3, 1, 4, 2};
    bsp::span<int> s(arr);
    std::sort(s.begin(), s.end());
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[4], 5);
}

TEST(SpanIterators, std_algorithm_iota) {
    std::array<int, 5> arr{};
    bsp::span<int>     s(arr);
    std::iota(s.begin(), s.end(), 1);
    for (int i = 0; i < 5; ++i)
        EXPECT_EQ(arr[i], i + 1);
}

// ---------------------------------------------------------------------------
// Subviews
// ---------------------------------------------------------------------------

TEST(SpanSubviews, first_static) {
    int               arr[] = {1, 2, 3, 4, 5};
    bsp::span<int, 5> s(arr);
    auto              f = s.first<3>();
    static_assert(decltype(f)::extent == 3);
    EXPECT_EQ(f.size(), 3u);
    EXPECT_EQ(f[0], 1);
    EXPECT_EQ(f[2], 3);
}

TEST(SpanSubviews, first_dynamic) {
    int            arr[] = {10, 20, 30, 40};
    bsp::span<int> s(arr);
    auto           f = s.first(2);
    EXPECT_EQ(f.size(), 2u);
    EXPECT_EQ(f[0], 10);
    EXPECT_EQ(f[1], 20);
}

TEST(SpanSubviews, last_static) {
    int               arr[] = {1, 2, 3, 4, 5};
    bsp::span<int, 5> s(arr);
    auto              l = s.last<2>();
    static_assert(decltype(l)::extent == 2);
    EXPECT_EQ(l[0], 4);
    EXPECT_EQ(l[1], 5);
}

TEST(SpanSubviews, last_dynamic) {
    int            arr[] = {10, 20, 30, 40};
    bsp::span<int> s(arr);
    auto           l = s.last(3);
    EXPECT_EQ(l.size(), 3u);
    EXPECT_EQ(l[0], 20);
}

TEST(SpanSubviews, subspan_static_offset) {
    int               arr[] = {0, 1, 2, 3, 4};
    bsp::span<int, 5> s(arr);
    auto              sub = s.subspan<2>();
    static_assert(decltype(sub)::extent == 3);
    EXPECT_EQ(sub.size(), 3u);
    EXPECT_EQ(sub[0], 2);
    EXPECT_EQ(sub[2], 4);
}

TEST(SpanSubviews, subspan_static_offset_and_count) {
    int               arr[] = {0, 1, 2, 3, 4};
    bsp::span<int, 5> s(arr);
    auto              sub = s.subspan<1, 3>();
    static_assert(decltype(sub)::extent == 3);
    EXPECT_EQ(sub.size(), 3u);
    EXPECT_EQ(sub[0], 1);
    EXPECT_EQ(sub[2], 3);
}

TEST(SpanSubviews, subspan_dynamic) {
    int            arr[] = {10, 20, 30, 40, 50};
    bsp::span<int> s(arr);
    auto           sub = s.subspan(1, 3);
    EXPECT_EQ(sub.size(), 3u);
    EXPECT_EQ(sub[0], 20);
    EXPECT_EQ(sub[2], 40);
}

TEST(SpanSubviews, subspan_dynamic_to_end) {
    int            arr[] = {1, 2, 3, 4};
    bsp::span<int> s(arr);
    auto           sub = s.subspan(2);
    EXPECT_EQ(sub.size(), 2u);
    EXPECT_EQ(sub[0], 3);
    EXPECT_EQ(sub[1], 4);
}

// ---------------------------------------------------------------------------
// as_bytes / as_writable_bytes
// ---------------------------------------------------------------------------

TEST(SpanObjectRepresentation, as_bytes) {
    int            arr[] = {1};
    bsp::span<int> s(arr);
    auto           bytes = bsp::as_bytes(s);
    EXPECT_EQ(bytes.size(), sizeof(int));
    static_assert(std::is_same_v<decltype(bytes)::element_type, const std::byte>);
}

TEST(SpanObjectRepresentation, as_writable_bytes) {
    int            arr[] = {1};
    bsp::span<int> s(arr);
    auto           bytes = bsp::as_writable_bytes(s);
    EXPECT_EQ(bytes.size(), sizeof(int));
    static_assert(std::is_same_v<decltype(bytes)::element_type, std::byte>);
}

TEST(SpanObjectRepresentation, as_bytes_fixed_extent) {
    int               arr[3] = {1, 2, 3};
    bsp::span<int, 3> s(arr);
    auto              bytes = bsp::as_bytes(s);
    static_assert(decltype(bytes)::extent == 3 * sizeof(int));
    EXPECT_EQ(bytes.size(), 3 * sizeof(int));
}

// ---------------------------------------------------------------------------
// Constexpr
// ---------------------------------------------------------------------------

// Local arrays cannot be used as constant expressions in all compilers; use
// a namespace-scope array instead.
static constexpr int kConstexprArr[] = {1, 2, 3};

TEST(SpanConstexpr, size_and_access) {
    constexpr bsp::span<const int, 3> s(kConstexprArr);
    static_assert(s.size() == 3);
    static_assert(s[0] == 1);
    static_assert(s.front() == 1);
    static_assert(s.back() == 3);
    EXPECT_EQ(s.size(), 3u);
}

// ---------------------------------------------------------------------------
// Comparison with std::span (when available)
// ---------------------------------------------------------------------------

#if defined(__cpp_lib_span) && __cpp_lib_span >= 202002L
    #include <span>

TEST(SpanCompareWithStd, same_size_and_values) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    std::span<int>   std_s(v);
    bsp::span<int>   beman_s(v);

    EXPECT_EQ(std_s.size(), beman_s.size());
    for (std::size_t i = 0; i < std_s.size(); ++i) {
        EXPECT_EQ(std_s[i], beman_s[i]);
    }
}
#endif
