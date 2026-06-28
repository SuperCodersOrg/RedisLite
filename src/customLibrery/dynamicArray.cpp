#pragma once
#ifndef DYNAMIC_ARRAY_CPP
#define DYNAMIC_ARRAY_CPP

#include "dynamicArray.h"
#include <cstdlib>
#include <new>

template<typename T>
void DynamicArray<T>::resize(){
    capacity *= 2;
    T* newArr = (T*)malloc(capacity*sizeof(T));
    for(int i = 0; i<size; i++){
        new(&newArr[i]) T(arr[i]);
        arr[i].~T();
    }
    free(arr);
    arr = newArr;
}

template<typename T>
DynamicArray<T>::DynamicArray(){
    capacity = 4;
    arr = (T*)malloc(capacity*sizeof(T));
    size = 0;
}

template<typename T>
DynamicArray<T>::DynamicArray(const DynamicArray& other){
    capacity = other.capacity;
    size = other.size;
    arr = (T*)malloc(capacity*sizeof(T));
    for(int i =0; i<size ;i++){
        new(&arr[i]) T(other.arr[i]);
    }
}

template<typename T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray& other){
    if(this == &other) return *this;
    for(int i = 0; i < size; i++){
        arr[i].~T();
    }
    free(arr);
    capacity = other.capacity;
    size = other.size;
    arr = (T*)malloc(capacity*sizeof(T));
    for(int i = 0; i < size; i++){
        new(&arr[i]) T(other.arr[i]);
    }
    return *this;
}

template<typename T>
DynamicArray<T>::~DynamicArray(){
    for(int i =0; i<size ;i++){
        arr[i].~T();
    }
    free(arr);
    arr = nullptr;
}

template<typename T>
void DynamicArray<T>::append(T val){
     if(size == capacity){
        resize();
     }
     new(&arr[size]) T(val);
     size++;
}

template<typename T>
T& DynamicArray<T>::get(int idx){
    if(idx<0 || idx >= size){
        throw "Index out of bounds";
    }
    return arr[idx];
}

template<typename T>
const T& DynamicArray<T>::get(int idx) const {
    if(idx<0 || idx >= size){
        throw "Index out of bounds";
    }
    return arr[idx];
}

template<typename T>
void DynamicArray<T>::insert(int idx, T val){
    if(idx<0 || idx > size){
        throw "Index out of bounds";
    }
    if(size == capacity){
        resize();
    }
    for(int i = size-1; i>= idx; i--){
        new(&arr[i+1]) T(arr[i]);
    }
    new(&arr[idx]) T(val);
    size++;
}

template<typename T>
void DynamicArray<T>::remove(int idx){
    if(idx<0 || idx >= size){
        throw "Index out of bounds";
    }
    for(int i = idx; i<size-1; i++){
        new(&arr[i]) T(arr[i+1]);
    }
    size--;
}

template<typename T>
int DynamicArray<T>::getSize(){
    return size;
}

#endif // DYNAMIC_ARRAY_CPP
