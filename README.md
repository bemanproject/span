# beman.span: A Beman implementation of std::span

<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable-next-line line-length -->
![Library Status](https://raw.githubusercontent.com/bemanproject/beman/refs/heads/main/images/badges/beman_badge-beman_library_under_development.svg) ![Continuous Integration Tests](https://github.com/bemanproject/span/actions/workflows/ci_tests.yml/badge.svg) ![Lint Check (pre-commit)](https://github.com/bemanproject/span/actions/workflows/pre-commit-check.yml/badge.svg) [![Coverage](https://coveralls.io/repos/github/bemanproject/span/badge.svg?branch=main)](https://coveralls.io/github/bemanproject/span?branch=main) ![Standard Target](https://github.com/bemanproject/beman/blob/main/images/badges/cpp29.svg)

`beman.span` is an implementation of various proposed updates to `std::span`.

**Implements:** `std::span` a non-owning view over a contiguous sequence of objects.
See [cppreference — std::span](https://en.cppreference.com/w/cpp/container/span) and
[eel.is/c++draft/views.span](https://eel.is/c++draft/views.span).

**Status**: [Under development and not yet ready for production use.](https://github.com/bemanproject/beman/blob/main/docs/beman_library_maturity_model.md#under-development-and-not-yet-ready-for-production-use)

## License

`beman.span` is licensed under the Apache License v2.0 with LLVM Exceptions.

## Features

- Header-only, no external runtime dependencies
- Supports both fixed and dynamic extents (`beman::span::dynamic_extent`)
- Full constructor overloads: C-array, `std::array`, pointer+count, pointer pair, generic range
- Subviews: `first()`, `last()`, `subspan()` both compile-time and runtime versions
- Object representation helpers: `as_bytes()`, `as_writable_bytes()`
- Deduction guides (C++17)
- `constexpr` throughout
- Compared with `std::span` in tests when available (`__cpp_lib_span`)

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

Full runnable examples can be found in [`examples/`](examples/).

## Dependencies

### Build Environment

This project requires at least the following to build:

* A C++ compiler that conforms to the C++20 standard or greater
* CMake 3.30 or later
* (Test Only) GoogleTest

You can disable building tests by setting CMake option `BEMAN_SPAN_BUILD_TESTS` to
`OFF` when configuring the project.

### Supported Platforms

| Compiler   | Version | C++ Standards | Standard Library  |
|------------|---------|---------------|-------------------|
| GCC        | 15-13   | C++26-C++20   | libstdc++         |
| GCC        | 12-11   | C++23, C++20  | libstdc++         |
| Clang      | 22-19   | C++26-C++20   | libstdc++, libc++ |
| Clang      | 18      | C++26-C++20   | libc++            |
| Clang      | 18      | C++23, C++20  | libstdc++         |
| Clang      | 17      | C++26-C++20   | libc++            |
| Clang      | 17      | C++20         | libstdc++         |
| AppleClang | latest  | C++26-C++20   | libc++            |
| MSVC       | latest  | C++23         | MSVC STL          |

## Development

See the [Contributing Guidelines](CONTRIBUTING.md).

## Integrate beman.span into your project

### Build

You can build span using a CMake workflow preset:

```bash
cmake --workflow --preset gcc-release
```

To list available workflow presets, you can invoke:

```bash
cmake --list-presets=workflow
```

For details on building beman.span without using a CMake preset, refer to the
[Contributing Guidelines](CONTRIBUTING.md).

### Installation

#### Vcpkg

The preferred way to install span is via vcpkg. To do so, after installing vcpkg
itself, you need to add support for the Beman project's [vcpkg
registry](https://github.com/bemanproject/vcpkg-registry) by configuring a
`vcpkg-configuration.json` file (which span [provides](vcpkg-configuration.json)).

Then, simply run `vcpkg install beman-span`.

#### Manual

To install beman.span globally after building with the `gcc-release` preset, you can
run:

```bash
sudo cmake --install build/gcc-release
```

Alternatively, to install to a prefix, for example `/opt/beman`, you can run:

```bash
sudo cmake --install build/gcc-release --prefix /opt/beman
```

This will generate the following directory structure:

```txt
/opt/beman
├── include
│   └── beman
│       └── span
│           ├── span.hpp
│           └── ...
└── lib
    └── cmake
        └── beman.span
            ├── beman.span-config-version.cmake
            ├── beman.span-config.cmake
            └── beman.span-targets.cmake
```

### CMake Configuration

If you installed beman.span to a prefix, you can specify that prefix to your CMake
project using `CMAKE_PREFIX_PATH`; for example, `-DCMAKE_PREFIX_PATH=/opt/beman`.

You need to bring in the `beman.span` package to define the `beman::span` CMake
target:

```cmake
find_package(beman.span REQUIRED)
```

You will then need to add `beman::span` to the link libraries of any libraries or
executables that include `beman.span` headers.

```cmake
target_link_libraries(yourlib PUBLIC beman::span)
```

### Using beman.span

To use `beman.span` in your C++ project,
include an appropriate `beman.span` header from your source code.

```c++
#include <beman/span/span.hpp>
```

> [!NOTE]
>
> `beman.span` headers are to be included with the `beman/span/` prefix.
> Altering include search paths to spell the include target another way (e.g.
> `#include <span.hpp>`) is unsupported.
