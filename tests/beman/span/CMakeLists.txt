# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

find_package(GTest REQUIRED)

add_executable(beman.span.tests.span)
target_sources(beman.span.tests.span PRIVATE span.test.cpp)
target_link_libraries(
    beman.span.tests.span
    PRIVATE beman::span GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(beman.span.tests.span DISCOVERY_TIMEOUT 60)
