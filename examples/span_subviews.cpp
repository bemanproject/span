// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Demonstrates beman::span subview operations: first(), last(), and subspan().

#include <beman/span/span.hpp>

#include <array>
#include <iostream>

namespace bsp = beman::span;

template <class T, std::size_t E>
void print(const char* label, bsp::span<T, E> s) {
    std::cout << label << ": [";
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (i)
            std::cout << ", ";
        std::cout << s[i];
    }
    std::cout << "]  extent=";
    if constexpr (E == bsp::dynamic_extent)
        std::cout << "dynamic\n";
    else
        std::cout << E << "\n";
}

int main() {
    std::array<int, 8> arr = {0, 1, 2, 3, 4, 5, 6, 7};
    bsp::span<int, 8>  full(arr);

    print("full      ", full);

    // first<N>: compile-time count, fixed extent result
    auto f3 = full.first<3>();
    print("first<3>  ", f3);

    // last<N>: compile-time count, fixed extent result
    auto l3 = full.last<3>();
    print("last<3>   ", l3);

    // subspan<Offset, Count>: both compile-time
    auto mid = full.subspan<2, 4>();
    print("sub<2,4>  ", mid);

    // subspan<Offset>: offset only — extent = Extent - Offset at compile time
    auto from2 = full.subspan<2>();
    print("sub<2>    ", from2);

    // Dynamic variants
    bsp::span<int> dyn(arr);
    auto           df = dyn.first(4);
    print("dyn.first(4) ", df);

    auto dl = dyn.last(2);
    print("dyn.last(2)  ", dl);

    auto ds = dyn.subspan(3, 3);
    print("dyn.sub(3,3) ", ds);
}
