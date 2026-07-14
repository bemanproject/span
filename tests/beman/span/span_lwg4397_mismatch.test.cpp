// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Negative-compile test for LWG 4397 (Improve span(R&& r)).
//
// Constructing a fixed-extent span from a range whose size is a constant
// expression that does NOT match the extent is a Mandates violation and must be
// ill-formed. Here ranges::size(views::empty<int>) is the constant 0, which
// does not equal the extent 42, so this translation unit must fail to compile.
//
// This file is compiled by try_compile() in the surrounding CMakeLists.txt as
// the probe that decides whether the toolchain realizes the Mandate (i.e.
// implements P2280R4). When it does, this same file is also built as an
// EXCLUDE_FROM_ALL target and registered as a ctest with the WILL_FAIL property.

#include <beman/span/span.hpp>

#include <ranges>

int main() {
    beman::span::span<int, 42> s(std::views::empty<int>);
    (void)s;
    return 0;
}
