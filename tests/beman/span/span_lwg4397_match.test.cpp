// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Positive compile probe for LWG 4397 (Improve span(R&& r)).
//
// Constructing a fixed-extent span from a range whose size is a constant
// expression that matches the extent is well-formed. Here
// ranges::size(views::empty<int>) is the constant 0, which equals the extent 0.
//
// This file is not built as part of the test suite; it is compiled by
// try_compile() in the surrounding CMakeLists.txt as the control probe that
// confirms the toolchain accepts the matching case (paired with
// span_lwg4397_mismatch.test.cpp, which must be rejected).

#include <beman/span/span.hpp>

#include <ranges>

int main() {
    beman::span::span<int, 0> s(std::views::empty<int>);
    (void)s;
    return 0;
}
