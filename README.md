# beman.span: A Beman implementation of `std::span`

<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

[![Apache-2.0 License](https://img.shields.io/badge/license-Apache--2.0%20WITH%20LLVM--exception-blue.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)

**Implements:** `std::span` — a non-owning view over a contiguous sequence of objects.
See [cppreference — std::span](https://en.cppreference.com/w/cpp/container/span) and
[eel.is/c++draft/views.span](https://eel.is/c++draft/views.span).

**Status:** Under development and not yet ready for production use.
See the [Beman Library Maturity Model](https://github.com/bemanproject/beman/blob/main/docs/beman_library_maturity_model.md).

---

## Features

- Header-only, no external runtime dependencies
- Supports both fixed and dynamic extents (`beman::span::dynamic_extent`)
- Full constructor overloads: C-array, `std::array`, pointer+count, pointer pair, generic range
- Subviews: `first()`, `last()`, `subspan()` — both compile-time and runtime versions
- Object representation helpers: `as_bytes()`, `as_writable_bytes()`
- Deduction guides (C++17)
- `constexpr` throughout
- Compared with `std::span` in tests when available (`__cpp_lib_span`)

---

## Usage

```cpp
#include <beman/span/span.hpp>

#include <array>
#include <vector>
#include <iostream>

namespace bsp = beman::span;

void print(bsp::span<const int> s) {
    for (auto v : s)
        std::cout << v << ' ';
    std::cout << '\n';
}

int main() {
    // From a C-array (fixed extent deduced)
    int arr[] = {1, 2, 3, 4, 5};
    bsp::span s1(arr);           // span<int, 5>
    print(s1);

    // From a vector (dynamic extent)
    std::vector<int> v = {10, 20, 30};
    bsp::span<int> s2(v);
    print(s2.first(2));          // span<int, dynamic_extent> {10, 20}

    // Byte view
    auto bytes = bsp::as_bytes(s1);
    std::cout << "size_bytes=" << bytes.size() << '\n';
}
```

---

## Building

### Requirements

| Dependency | Version |
|------------|---------|
| CMake      | ≥ 3.30  |
| C++ compiler | C++20 (GCC 11+, Clang 13+, MSVC 19.29+) |
| GTest      | any recent version (for tests) |

### Build with presets

```bash
cmake --preset gcc-release    # configure
cmake --build build/gcc-release
ctest --test-dir build/gcc-release
```

### Manual build

```bash
cmake -B build -S . -DCMAKE_CXX_STANDARD=20
cmake --build build
ctest --test-dir build
```

### Options

| Option | Default | Description |
|--------|---------|-------------|
| `BEMAN_SPAN_BUILD_TESTS` | `ON` (top-level) | Build and run unit tests |
| `BEMAN_SPAN_BUILD_EXAMPLES` | `ON` (top-level) | Build example programs |

---

## Consuming as a dependency

```cmake
find_package(beman.span REQUIRED)
target_link_libraries(my_target PRIVATE beman::span)
```

Or via FetchContent:

```cmake
include(FetchContent)
FetchContent_Declare(
    beman.span
    GIT_REPOSITORY https://github.com/SamareshSingh/span.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(beman.span)
target_link_libraries(my_target PRIVATE beman::span)
```

---

## License

`beman.span` is licensed under the Apache License v2.0 with LLVM Exceptions.
See [LICENSE](LICENSE) for details.

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) and the
[Beman Standard](https://github.com/bemanproject/beman/blob/main/docs/beman_standard.md).
