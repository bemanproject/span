// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Demonstrates basic beman::span usage: construction from C-arrays, std::array,
// std::vector, element access, and iteration.

#include <beman/span/span.hpp>

#include <array>
#include <iostream>
#include <numeric>
#include <vector>

// Print every element of a span — accepts any element type and extent.
template <class T, std::size_t E>
void print_span(beman::span::span<T, E> s) {
    std::cout << "[";
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (i)
            std::cout << ", ";
        std::cout << s[i];
    }
    std::cout << "]  (size=" << s.size() << ")\n";
}

// Returns the sum of all elements in a span.
template <class T, std::size_t E>
T sum(beman::span::span<T, E> s) {
    T total{};
    for (const auto& v : s)
        total += v;
    return total;
}

int main() {
    // ----------------------------------------------------------------
    // 1. Construct from a C-array (fixed extent deduced)
    // ----------------------------------------------------------------
    int               c_arr[] = {10, 20, 30, 40, 50};
    beman::span::span s1(c_arr); // span<int, 5>
    std::cout << "From C-array:   ";
    print_span(s1);

    // ----------------------------------------------------------------
    // 2. Construct from std::array
    // ----------------------------------------------------------------
    std::array<int, 4> std_arr = {1, 2, 3, 4};
    beman::span::span  s2(std_arr); // span<int, 4>
    std::cout << "From std::array: ";
    print_span(s2);

    // ----------------------------------------------------------------
    // 3. Construct from std::vector (dynamic extent)
    // ----------------------------------------------------------------
    std::vector<int> vec(6);
    std::iota(vec.begin(), vec.end(), 1); // {1,2,3,4,5,6}
    beman::span::span<int> s3(vec);
    std::cout << "From vector:     ";
    print_span(s3);

    // ----------------------------------------------------------------
    // 4. Element access
    // ----------------------------------------------------------------
    std::cout << "front=" << s3.front() << "  back=" << s3.back() << "\n";

    // ----------------------------------------------------------------
    // 5. Modify through span
    // ----------------------------------------------------------------
    for (auto& v : s3)
        v *= 2;
    std::cout << "After *2:        ";
    print_span(s3);
    std::cout << "Sum:             " << sum(s3) << "\n";

    // ----------------------------------------------------------------
    // 6. size_bytes
    // ----------------------------------------------------------------
    std::cout << "size_bytes=" << s3.size_bytes() << " (=" << s3.size() << " * " << sizeof(int) << ")\n";
}
