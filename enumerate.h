#pragma once

#include <iterator>
#include <tuple>
#include <utility>     // Required for std::forward
#include <cstddef>

template <typename T,
         typename TIter = decltype(std::begin(std::declval<T&>())),
         typename = decltype(std::end(std::declval<T&>()))>
constexpr auto enumerate(T && iterable) {
    using ssize_t = std::ptrdiff_t;

    struct iterator {
        ssize_t i;
        TIter iter;
        bool operator != (const iterator & other) const { return iter != other.iter; }
        void operator ++ ()                             { ++i; ++iter; }
        auto operator * () const                        { return std::tie(i, *iter); }
    };

    struct iterable_wrapper {
        T iterable;
        auto begin() { return iterator{ 0, std::begin(iterable) }; }
        auto end()   { return iterator{ 0, std::end(iterable) }; }
    };

    return iterable_wrapper { std::forward<T>(iterable) };
}
