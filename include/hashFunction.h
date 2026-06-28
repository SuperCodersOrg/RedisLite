#pragma once
#ifndef HASH_FUNCTION_H
#define HASH_FUNCTION_H

#include <string>

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
