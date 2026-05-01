# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(ALL_EXAMPLES span_basic_usage span_subviews span_bytes)
message("Examples to be built: ${ALL_EXAMPLES}")

foreach(example ${ALL_EXAMPLES})
    add_executable(beman.span.examples.${example})
    target_sources(beman.span.examples.${example} PRIVATE ${example}.cpp)
    target_link_libraries(beman.span.examples.${example} PRIVATE beman::span)
endforeach()
