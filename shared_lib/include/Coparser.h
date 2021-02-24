#ifndef COPARSER_H
#define COPARSER_H

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <ostream>
#include <vector>
#include <memory>
#include <optional>
#include <functional>

namespace IRC::Coparser {
    template<typename T>
    using Coparser = std::function<void(std::ostringstream&, const T&)>;

    template<typename T>
    auto Coparse(Coparser<T> p, T v) -> std::string {
        std::ostringstream ss;
        p(ss, v);
        return ss.str();
    }

    template<size_t size>
    struct Replicate {
        char buf_[size + 1] = {};
        constexpr Replicate(char ch) {
            for (size_t i = 0; i < size; i++)
                buf_[i] = ch;
        }
    };

    template<size_t size>
    constexpr auto operator<<(std::ostream& os, const Replicate<size>& repl) -> std::ostream& {
        return os << repl.buf_;
    }

    template<template <typename> class V = std::vector, typename A = std::string>
    struct SepBy {
        const V<A> elems_;
        const A delim_;
        constexpr SepBy(V<A> elems, A delim) : elems_(elems), delim_(delim) {}
    };

    template<template <typename> class V = std::vector, typename A = std::string>
    constexpr auto operator<<(std::ostream& os, const SepBy<V, A>& sep) -> std::ostream& {
        for (auto it = sep.elems_.begin(); it != sep.elems_.end();) {
            os << *it;
            ++it;
            if (it != sep.elems_.end())
                os << sep.delim_;
        }
        return os;
    }
};

#endif // COPARSER_H
