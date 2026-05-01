// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Demonstrates beman::span byte-level view helpers: as_bytes() and
// as_writable_bytes().  Useful for serialisation, checksumming, etc.

#include <beman/span/span.hpp>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>

namespace bsp = beman::span;

// Simple byte-by-byte checksum (XOR fold) for illustration.
std::byte checksum(bsp::span<const std::byte> data) {
    std::byte result{0};
    for (auto b : data)
        result ^= b;
    return result;
}

int main() {
    // ----------------------------------------------------------------
    // as_bytes: read-only view over the raw bytes of any span
    // ----------------------------------------------------------------
    int            arr[] = {0x01020304, 0x05060708};
    bsp::span<int> s(arr);

    auto bytes = bsp::as_bytes(s);
    std::cout << "as_bytes extent (fixed): " << decltype(bytes)::extent << "  size: " << bytes.size() << "\n";

    std::cout << "Raw bytes of {0x01020304, 0x05060708}: ";
    for (auto b : bytes)
        std::cout << std::hex << static_cast<unsigned>(b) << " ";
    std::cout << std::dec << "\n";

    std::cout << "Checksum: 0x" << std::hex << static_cast<unsigned>(checksum(bytes)) << std::dec << "\n";

    // ----------------------------------------------------------------
    // as_writable_bytes: write-only (mutable) view over raw bytes
    // ----------------------------------------------------------------
    int            value = 0;
    bsp::span<int> vs(&value, 1);
    auto           wb = bsp::as_writable_bytes(vs);

    // Write 0xDEADBEEF byte by byte (little-endian on most platforms)
    const std::uint8_t pattern[] = {0xEF, 0xBE, 0xAD, 0xDE};
    for (std::size_t i = 0; i < sizeof(int) && i < wb.size(); ++i)
        wb[i] = static_cast<std::byte>(pattern[i]);

    std::cout << "After as_writable_bytes write: 0x" << std::hex << value << std::dec << "\n";
}
