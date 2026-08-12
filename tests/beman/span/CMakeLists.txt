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

# ---------------------------------------------------------------------------
# LWG 4397 negative-compile test (guarded).
#
# The LWG 4397 Mandate rejects e.g. span<int, 42>(views::empty<int>) at compile
# time, but only on compilers that treat ranges::size(r) on a value-independent
# range as a constant expression through the constructor parameter (i.e. that
# implement P2280R4). On other compilers the constructor is well-formed and the
# extent is only checked at run time, so a negative-compile test would be a
# false failure there.
#
# Probe the current compiler at configure time: the matched-extent construction
# must compile and the mismatched-extent construction must NOT. Only when both
# hold do we register the negative-compile test.
# ---------------------------------------------------------------------------

try_compile(
    BEMAN_SPAN_LWG4397_MATCH_COMPILES
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/span_lwg4397_match.test.cpp
    CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${PROJECT_SOURCE_DIR}/include"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

try_compile(
    BEMAN_SPAN_LWG4397_MISMATCH_COMPILES
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/span_lwg4397_mismatch.test.cpp
    CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${PROJECT_SOURCE_DIR}/include"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(
    BEMAN_SPAN_LWG4397_MATCH_COMPILES
    AND NOT BEMAN_SPAN_LWG4397_MISMATCH_COMPILES
)
    add_executable(
        beman.span.tests.span_lwg4397_fail
        EXCLUDE_FROM_ALL
        span_lwg4397_mismatch.test.cpp
    )
    target_link_libraries(
        beman.span.tests.span_lwg4397_fail
        PRIVATE beman::span
    )
    target_compile_features(
        beman.span.tests.span_lwg4397_fail
        PRIVATE cxx_std_20
    )

    add_test(
        NAME beman.span.tests.span_lwg4397_mismatch_is_ill_formed
        COMMAND
            ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target
            beman.span.tests.span_lwg4397_fail --config $<CONFIG>
    )

    set_tests_properties(
        beman.span.tests.span_lwg4397_mismatch_is_ill_formed
        PROPERTIES WILL_FAIL TRUE
    )
    message(STATUS "beman.span: LWG 4397 negative-compile test enabled")
else()
    message(
        STATUS
        "beman.span: LWG 4397 negative-compile test skipped "
        "(compiler does not reject the mismatch at compile time)"
    )
endif()
