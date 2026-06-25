#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <cstdlib>
#include <new>
template<typename T>
class DynamicArray{
    private:
        T* arr;
        int size;
        int capacity;
        void resize(){
            capacity *= 2;
            T* newArr = (T*)malloc(capacity*sizeof(T));
            for(int i = 0; i<size; i++){
                new(&newArr[i]) T(arr[i]);
            }
            free(arr);
            arr = newArr;
        }
    public:
        DynamicArray(){
            capacity = 4;
            arr = (T*)malloc(capacity*sizeof(T));
            size = 0;

        }
        DynamicArray(const DynamicArray& other){
            capacity = other.capacity;
            size = other.size;
            arr = (T*)malloc(capacity*sizeof(T));
            for(int i =0; i<size ;i++){
                new(&arr[i]) T(other.arr[i]);
            }
        }

        DynamicArray& operator=(const DynamicArray& other){
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
        ~DynamicArray(){
            for(int i =0; i<size ;i++){
                arr[i].~T();
            }
            free(arr);
            arr = nullptr;
        }
        void append(T val){
             if(size == capacity){
                resize();
             }
             new(&arr[size]) T(val);
             size++;
        }
        T& get(int idx){
            if(idx<0 || idx >= size){
                throw "Index out of bounds";
            }
            return arr[idx];
        }
        void insert(int idx, T val){
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
        void remove(int idx){
            if(idx<0 || idx >= size){
                throw "Index out of bounds";
            }
            for(int i = idx; i<size-1; i++){
                new(&arr[i]) T(arr[i+1]);
            }
            size--;
        }
        int getSize(){
        return size;
    }
};

#endif // DYNAMIC_ARRAY_H
