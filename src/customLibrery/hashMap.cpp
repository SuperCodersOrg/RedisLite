#pragma once
#ifndef HASHMAP_CPP
#define HASHMAP_CPP

#include "hashMap.h"

template<typename K, typename V>
int HashMap<K, V>::getBucketIndex(K key) const {
    int hashValue = hashGenerator.generate(key);
    if (hashValue < 0) {
        hashValue = -hashValue;
    }
    return hashValue % capacity;
}

template<typename K, typename V>
void HashMap<K, V>::rehash() {
    int oldCapacity = capacity;
    capacity *= 2;
    
    DynamicArray<DoublyLinkedList<Node<K, V>>> newBuckets;
    for (int i = 0; i < capacity; i++) {
        newBuckets.append(DoublyLinkedList<Node<K, V>>());
    }

    for (int i = 0; i < oldCapacity; i++) {
        DoublyLinkedList<Node<K, V>>& chain = buckets.get(i);
        int chainSize = chain.getSize();
        for (int j = 0; j < chainSize; j++) {
            Node<K, V> entry = chain.get(j);
            int newIdx = getBucketIndex(entry.key);
            newBuckets.get(newIdx).append(entry);
        }
    }

    // operator= on DynamicArray destroys the old buckets array
    // and copies newBuckets — no explicit destructor calls needed.
    buckets = newBuckets;
}

template<typename K, typename V>
HashMap<K, V>::HashMap() {
    capacity = 4; // Initial Capacity
    size = 0;
    
    for (int i = 0; i < capacity; i++) {
        buckets.append(DoublyLinkedList<Node<K, V>>());
    }
}

template<typename K, typename V>
HashMap<K, V>::HashMap(const HashMap& other) {
    capacity = other.capacity;
    size = other.size;
    buckets = other.buckets;  
}

template<typename K, typename V>
HashMap<K, V>& HashMap<K, V>::operator=(const HashMap& other) {
    if (this == &other) return *this;
    capacity = other.capacity;
    size = other.size;
    buckets = other.buckets;
    return *this;
}

template<typename K, typename V>
HashMap<K, V>::~HashMap() {
    size = 0;
    capacity = 0;
}

template<typename K, typename V>
void HashMap<K, V>::set(K key, V value) {
    if ((float)size / capacity > 0.75) {
        rehash();
    }

    int idx = getBucketIndex(key);
    DoublyLinkedList<Node<K, V>>& chain = buckets.get(idx);
    
    int chainSize = chain.getSize();
    for (int i = 0; i < chainSize; i++) {
        Node<K, V> entry = chain.get(i);
        if (entry.key == key){
            chain.remove(i);
            chain.append(Node<K, V>(key, value));
            return;
        }
    }
    
    chain.append(Node<K, V>(key, value));
    size++;
}

template<typename K, typename V>
V& HashMap<K, V>::get(K key) {
    int idx = getBucketIndex(key);
    DoublyLinkedList<Node<K, V>>& chain = buckets.get(idx);
    
    int chainSize = chain.getSize();
    for (int i = 0; i < chainSize; i++){
        Node<K, V>& entry = chain.get(i);
        if (entry.key == key) {
            return entry.value;
        }
    }
    throw "Key not found";
}

template<typename K, typename V>
const V& HashMap<K, V>::get(K key) const {
    int idx = getBucketIndex(key);
    const DoublyLinkedList<Node<K, V>>& chain = buckets.get(idx);
    
    int chainSize = chain.getSize();
    for (int i = 0; i < chainSize; i++) {
        const Node<K, V>& entry = chain.get(i);
        if (entry.key == key) {
            return entry.value;
        }
    }
    throw "Key not found";
}

template<typename K, typename V>
void HashMap<K, V>::remove(K key) {
    int idx = getBucketIndex(key);
    DoublyLinkedList<Node<K, V>>& chain = buckets.get(idx);
    
    int chainSize = chain.getSize();
    for (int i = 0; i < chainSize; i++) {
        const Node<K, V>& entry = chain.get(i);
        if (entry.key == key) {
            chain.remove(i);
            size--;
            return;
        }
    }
    throw "Key not found";
}

template<typename K, typename V>
int HashMap<K, V>::getSize() const {
    return size;
}

template<typename K, typename V>
bool HashMap<K, V>::exist(K key) const {
    int idx = getBucketIndex(key);
    const DoublyLinkedList<Node<K, V>>& chain = buckets.get(idx);
    
    int chainSize = chain.getSize();
    for (int i = 0; i < chainSize; i++) {
        const Node<K, V>& entry = chain.get(i);
        if (entry.key == key) {
            return true;
        }
    }
    return false;
}

template<typename K, typename V>
void HashMap<K, V>::clear() {
    for (int i = 0; i < capacity; i++) {
        // operator= on DoublyLinkedList already frees all nodes.
        // Calling ~DoublyLinkedList() here first would double-free them.
        buckets.get(i) = DoublyLinkedList<Node<K, V>>();
    }
    size = 0;
}

template<typename K, typename V>
bool HashMap<K, V>::empty() const {
    return size == 0;
}

#endif // HASHMAP_CPP
