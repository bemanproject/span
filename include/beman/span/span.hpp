// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_SPAN_SPAN_HPP
#define BEMAN_SPAN_SPAN_HPP

// C++ Standard Library: std::span equivalent.
// See https://eel.is/c++draft/views.span:
//
// 26.7.2 Header <span> synopsis [span.syn]
//
// inline constexpr size_t dynamic_extent = numeric_limits<size_t>::max();
//
// template<class ElementType, size_t Extent = dynamic_extent>
//   class span;
//
// Deduction guides
// as_bytes / as_writable_bytes free functions

#include <array>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>
#include <type_traits>

namespace beman::span {

// 26.7.2 [span.syn]
inline constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

// Forward declaration
template <class ElementType, std::size_t Extent = dynamic_extent>
class span;

namespace detail {

// Helper: detect if a type is a specialization of span
template <class T>
struct is_span : std::false_type {};

template <class T, std::size_t E>
struct is_span<span<T, E>> : std::true_type {};

// Helper: detect std::array
template <class T>
struct is_std_array : std::false_type {};

template <class T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

// Helper to check that a type is a contiguous range (C++20 range concept approximation)
template <class Range, class ElementType, class = void>
struct is_compatible_range : std::false_type {};

template <class Range, class ElementType>
struct is_compatible_range<
    Range,
    ElementType,
    std::void_t<
        decltype(std::data(std::declval<Range>())),
        decltype(std::size(std::declval<Range>())),
        std::enable_if_t<
            std::is_convertible_v<
                std::remove_pointer_t<decltype(std::data(std::declval<Range&>()))> (*)[],
                ElementType (*)[]>>>>
    : std::true_type {};

// Storage helper: stores a pointer (and optionally a size for dynamic_extent)
template <std::size_t Extent>
struct span_storage {
    constexpr span_storage() noexcept = default;
    constexpr span_storage(void* ptr, std::size_t /*size*/) noexcept
        : data_(static_cast<std::byte*>(ptr)) {}

    std::byte* data_ = nullptr;

    static constexpr std::size_t size_ = Extent;
};

template <>
struct span_storage<dynamic_extent> {
    constexpr span_storage() noexcept = default;
    constexpr span_storage(void* ptr, std::size_t size) noexcept
        : data_(static_cast<std::byte*>(ptr)), size_(size) {}

    std::byte* data_ = nullptr;
    std::size_t size_ = 0;
};

} // namespace detail

// 26.7.3 Class template span [views.span]
template <class ElementType, std::size_t Extent>
class span {
  public:
    // Member types
    using element_type     = ElementType;
    using value_type       = std::remove_cv_t<ElementType>;
    using size_type        = std::size_t;
    using difference_type  = std::ptrdiff_t;
    using pointer          = ElementType*;
    using const_pointer    = const ElementType*;
    using reference        = ElementType&;
    using const_reference  = const ElementType&;
    using iterator         = pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;

    // Member constants
    static constexpr size_type extent = Extent;

    // 26.7.3.2 Constructors, copy, and assignment [span.cons]

    // Default constructor: only valid when Extent == 0 or Extent == dynamic_extent
    template <std::size_t E = Extent,
              std::enable_if_t<E == dynamic_extent || E == 0, int> = 0>
    constexpr span() noexcept : data_(nullptr), size_(0) {}

    // Pointer + count constructor
    constexpr explicit(Extent != dynamic_extent) span(pointer ptr, size_type count)
        : data_(ptr), size_(count) {
        if constexpr (Extent != dynamic_extent) {
            assert(count == Extent);
        }
    }

    // Pointer pair constructor
    constexpr explicit(Extent != dynamic_extent) span(pointer first, pointer last)
        : data_(first), size_(static_cast<size_type>(last - first)) {
        if constexpr (Extent != dynamic_extent) {
            assert(static_cast<size_type>(last - first) == Extent);
        }
    }

    // C array constructor (fixed-size)
    template <std::size_t N,
              std::enable_if_t<Extent == dynamic_extent || Extent == N, int> = 0,
              std::enable_if_t<
                  std::is_convertible_v<std::remove_pointer_t<decltype(std::data(
                                            std::declval<ElementType (&)[N]>()))> (*)[],
                                        ElementType (*)[]>,
                  int> = 0>
    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    constexpr span(ElementType (&arr)[N]) noexcept
        : data_(arr), size_(N) {}

    // std::array constructor (fixed-size)
    template <class T,
              std::size_t N,
              std::enable_if_t<Extent == dynamic_extent || Extent == N, int> = 0,
              std::enable_if_t<std::is_convertible_v<T (*)[], ElementType (*)[]>, int> = 0>
    constexpr span(std::array<T, N>& arr) noexcept
        : data_(arr.data()), size_(N) {}

    // const std::array constructor
    template <class T,
              std::size_t N,
              std::enable_if_t<Extent == dynamic_extent || Extent == N, int> = 0,
              std::enable_if_t<std::is_convertible_v<const T (*)[], ElementType (*)[]>,
                               int> = 0>
    constexpr span(const std::array<T, N>& arr) noexcept
        : data_(arr.data()), size_(N) {}

    // Range constructor (generic contiguous range)
    template <
        class Range,
        std::enable_if_t<!detail::is_span<std::remove_cvref_t<Range>>::value, int> = 0,
        std::enable_if_t<!detail::is_std_array<std::remove_cvref_t<Range>>::value, int> = 0,
        std::enable_if_t<!std::is_array_v<std::remove_cvref_t<Range>>, int>             = 0,
        std::enable_if_t<detail::is_compatible_range<Range, ElementType>::value, int>    = 0>
    constexpr explicit(Extent != dynamic_extent) span(Range&& r)
        : data_(std::data(r)), size_(std::size(r)) {
        if constexpr (Extent != dynamic_extent) {
            assert(std::size(r) == Extent);
        }
    }

    // Copy constructor from another span
    template <class OtherElementType,
              std::size_t OtherExtent,
              std::enable_if_t<
                  (Extent == dynamic_extent || OtherExtent == dynamic_extent ||
                   Extent == OtherExtent) &&
                      std::is_convertible_v<OtherElementType (*)[], ElementType (*)[]>,
                  int> = 0>
    constexpr explicit(Extent != dynamic_extent &&
                       OtherExtent == dynamic_extent) span(const span<OtherElementType,
                                                                       OtherExtent>& s) noexcept
        : data_(s.data()), size_(s.size()) {
        if constexpr (Extent != dynamic_extent) {
            assert(s.size() == Extent);
        }
    }

    constexpr span(const span&) noexcept = default;
    constexpr span& operator=(const span&) noexcept = default;

    // 26.7.3.3 Subviews [span.sub]

    // first<Count>(): static count
    template <std::size_t Count>
    constexpr span<ElementType, Count> first() const noexcept {
        static_assert(Count <= Extent || Extent == dynamic_extent,
                      "Count out of range for fixed-extent span");
        assert(Count <= size());
        return span<ElementType, Count>{data_, Count};
    }

    // first(count): dynamic count
    constexpr span<ElementType, dynamic_extent> first(size_type count) const noexcept {
        assert(count <= size());
        return {data_, count};
    }

    // last<Count>(): static count
    template <std::size_t Count>
    constexpr span<ElementType, Count> last() const noexcept {
        static_assert(Count <= Extent || Extent == dynamic_extent,
                      "Count out of range for fixed-extent span");
        assert(Count <= size());
        return span<ElementType, Count>{data_ + (size() - Count), Count};
    }

    // last(count): dynamic count
    constexpr span<ElementType, dynamic_extent> last(size_type count) const noexcept {
        assert(count <= size());
        return {data_ + (size() - count), count};
    }

    // subspan<Offset, Count>(): static offset and count
    template <std::size_t Offset, std::size_t Count = dynamic_extent>
    constexpr auto subspan() const noexcept {
        static_assert(Offset <= Extent || Extent == dynamic_extent,
                      "Offset out of range");
        static_assert(Count == dynamic_extent || Count <= Extent - Offset ||
                          Extent == dynamic_extent,
                      "Count out of range");
        assert(Offset <= size());
        assert(Count == dynamic_extent || Count <= size() - Offset);

        constexpr std::size_t E =
            (Count != dynamic_extent)
                ? Count
                : (Extent != dynamic_extent ? Extent - Offset : dynamic_extent);

        return span<ElementType, E>{data_ + Offset,
                                    Count == dynamic_extent ? size() - Offset : Count};
    }

    // subspan(offset, count): dynamic
    constexpr span<ElementType, dynamic_extent> subspan(size_type offset,
                                                        size_type count = dynamic_extent) const
        noexcept {
        assert(offset <= size());
        assert(count == dynamic_extent || count <= size() - offset);
        return {data_ + offset, count == dynamic_extent ? size() - offset : count};
    }

    // 26.7.3.4 Observers [span.obs]

    [[nodiscard]] constexpr size_type size() const noexcept { return size_; }

    [[nodiscard]] constexpr size_type size_bytes() const noexcept {
        return size() * sizeof(element_type);
    }

    [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0; }

    // 26.7.3.5 Element access [span.elem]

    constexpr reference operator[](size_type idx) const noexcept {
        assert(idx < size());
        return data_[idx];
    }

    constexpr reference front() const noexcept {
        assert(!empty());
        return data_[0];
    }

    constexpr reference back() const noexcept {
        assert(!empty());
        return data_[size() - 1];
    }

    constexpr pointer data() const noexcept { return data_; }

    // 26.7.3.6 Iterator support [span.iterators]

    constexpr iterator begin() const noexcept { return data_; }
    constexpr iterator end() const noexcept { return data_ + size_; }

    constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
    constexpr reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

    // cbegin/cend are the same as begin/end for span (element_type already carries const)
    constexpr iterator cbegin() const noexcept { return begin(); }
    constexpr iterator cend() const noexcept { return end(); }
    constexpr reverse_iterator crbegin() const noexcept { return rbegin(); }
    constexpr reverse_iterator crend() const noexcept { return rend(); }

  private:
    pointer   data_;
    size_type size_;
};

// Deduction guides (C++17)
template <class T, std::size_t N>
// NOLINTNEXTLINE(modernize-avoid-c-arrays)
span(T (&)[N]) -> span<T, N>;

template <class T, std::size_t N>
span(std::array<T, N>&) -> span<T, N>;

template <class T, std::size_t N>
span(const std::array<T, N>&) -> span<const T, N>;

template <class Range>
span(Range&&) -> span<std::remove_reference_t<decltype(*std::data(std::declval<Range>()))>>;

// 26.7.3.7 Views of object representation [span.objectrep]

// as_bytes: reinterpret a span as read-only bytes
template <class ElementType, std::size_t Extent>
auto as_bytes(span<ElementType, Extent> s) noexcept
    -> span<const std::byte,
            Extent == dynamic_extent ? dynamic_extent : sizeof(ElementType) * Extent> {
    using return_type =
        span<const std::byte,
             Extent == dynamic_extent ? dynamic_extent : sizeof(ElementType) * Extent>;
    return return_type{reinterpret_cast<const std::byte*>(s.data()), s.size_bytes()};
}

// as_writable_bytes: reinterpret a mutable span as writable bytes
template <class ElementType,
          std::size_t Extent,
          std::enable_if_t<!std::is_const_v<ElementType>, int> = 0>
auto as_writable_bytes(span<ElementType, Extent> s) noexcept
    -> span<std::byte,
            Extent == dynamic_extent ? dynamic_extent : sizeof(ElementType) * Extent> {
    using return_type =
        span<std::byte, Extent == dynamic_extent ? dynamic_extent : sizeof(ElementType) * Extent>;
    return return_type{reinterpret_cast<std::byte*>(s.data()), s.size_bytes()};
}

} // namespace beman::span

#endif // BEMAN_SPAN_SPAN_HPP
