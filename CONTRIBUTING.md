# Development

## Configure and Build the Project Using CMake Presets

The simplest way of configuring and building the project is to use [CMake
Presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html). Appropriate
presets for major compilers have been included by default.  You can use `cmake
--list-presets=workflow` to see all available presets.

Example using the `gcc-debug` preset:

```shell
cmake --workflow --preset gcc-debug
```

Generally, there are two kinds of presets — `debug` and `release`.

- **debug**: debuginfo and sanitizers enabled, suitable for development.
- **release**: highest optimisation (`-O3`), suitable for benchmarking.

## Configure and Build Manually

If presets are not suitable, you can invoke CMake directly. Note that GoogleTest
must be installed (`libgtest-dev` on Debian/Ubuntu, `gtest` via vcpkg, etc.).

```bash
cmake -B build -S . -DCMAKE_CXX_STANDARD=20
cmake --build build
ctest --test-dir build
```

> **Important:** `beman.span` is a [passive project](
> https://github.com/bemanproject/beman/blob/main/docs/beman_standard.md#cmakepassive_projects).
> Always pass `-DCMAKE_CXX_STANDARD=20` when configuring manually.

## Dependency Management

### FetchContent (automatic)

Configure the project with `-DFETCHCONTENT_PROVIDER=fetchcontent` (or set it
in a CMake preset) and CMake will automatically download GoogleTest.

### Package manager

Install GoogleTest through your system package manager or vcpkg:

```bash
# Debian / Ubuntu
sudo apt install libgtest-dev

# vcpkg
vcpkg install gtest
```

## Code Style

- Follow the [Beman Standard](https://github.com/bemanproject/beman/blob/main/docs/beman_standard.md).
- All source files must start with the SPDX identifier:
  ```cpp
  // SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
  ```
- Header guards: `#ifndef BEMAN_SPAN_<FILENAME>_HPP`
- Test files use the `*.test.cpp` naming convention.
- Run `clang-format` (`.clang-format` config in the repo root) before submitting.
