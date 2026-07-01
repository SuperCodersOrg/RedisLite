#pragma once
#ifndef HASH_FUNCTION_H
#define HASH_FUNCTION_H

#include <string>
#include <type_traits>

template<typename T>
struct HasHashCode {
    template<typename U>
    static auto test(U* u) -> decltype(u->hashCode());
    static auto test(...) -> void;
    static constexpr bool value = std::is_same<decltype(test(std::declval<T*>())), int>::value;
};

class HashFunction {
public:
    int generate(int key) const;
    int generate(char key) const;
    int generate(bool key) const;
    int generate(const std::string& key) const;

    template<typename T>
    int generate(const T& key) const;

    template<typename T>
    int generateFallback(const T& key) const;
};

#include "../src/customLibrery/hashFunction.cpp"
#endif // HASH_FUNCTION_H
