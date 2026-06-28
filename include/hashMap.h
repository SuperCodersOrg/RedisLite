#pragma once
#ifndef HASHMAP_H
#define HASHMAP_H

#include "dynamicArray.h"
#include "doublyLinkedList.h"
#include "hashFunction.h"

template<typename K, typename V>
struct Node {
    K key;
    V value;
    Node() {}
    Node(K k, V v) : key(k), value(v) {}
};

template<typename K, typename V>
class HashMap {
private:
    DynamicArray<DoublyLinkedList<Node<K, V>>> buckets;
    int size; 
    int capacity; 
    HashFunction hashGenerator;

    int getBucketIndex(K key) const;
    void rehash();

public:
    HashMap();
    HashMap(const HashMap& other);
    HashMap& operator=(const HashMap& other);
    ~HashMap();
    
    void set(K key, V value);
    V get(K key) const;
    void remove(K key);
    int getSize() const;
};

#include "../src/customLibrery/hashMap.cpp"
#endif // HASHMAP_H
