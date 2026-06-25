#ifndef HASH_FUNCTION_CPP
#define HASH_FUNCTION_CPP

#include <string>

class HashFunction {
public:

    int generate(int key) const {
        return key;
    }

    int generate(char key) const {
        return key;
    }

    int generate(bool key) const {
        return key ? 1 : 0;
    }

    int generate(const std::string& key) const {

        size_t hash = 0;

        for(char ch : key) {
            hash = hash * 31 + ch;
        }

        return hash;
    }

    // User-defined class with hashCode()
    template<typename T>
    int generate(const T& key) const
    {
        return static_cast<int>(key.hashCode());
    }   

    // Fallback byte-wise hash
    template<typename T>
    int generateFallback(const T& key) const {

        const unsigned char* bytes =
            reinterpret_cast<const unsigned char*>(&key);

        int hash = 0;

        for(size_t i = 0; i < sizeof(T); i++) {
            hash = hash * 31 + bytes[i];
        }

        return hash;
    }
};

#endif // HASH_FUNCTION_CPP
