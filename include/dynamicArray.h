#pragma once
#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

template<typename T>
class DynamicArray {
private:
    T* arr;
    int size;
    int capacity;
    void resize();
public:
    DynamicArray();
    DynamicArray(const DynamicArray& other);
    DynamicArray& operator=(const DynamicArray& other);
    ~DynamicArray();
    void append(T val);
    T& get(int idx);
    const T& get(int idx) const;
    void insert(int idx, T val);
    void remove(int idx);
    int getSize();
};

#include "../src/customLibrery/dynamicArray.cpp"
#endif // DYNAMIC_ARRAY_H
