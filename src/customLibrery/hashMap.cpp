#ifndef HASHMAP_CPP
#define HASHMAP_CPP

#include "dynamicArray.cpp"
#include "doublyLinkedList.cpp"
#include "hashFunction.cpp"


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

    int getBucketIndex(K key) const {
        int hashValue = hashGenerator.generate(key);
       
        if (hashValue < 0) {
            hashValue = -hashValue;
        }
        return hashValue % capacity;
    }

    void rehash() {
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

        buckets = newBuckets; 
    }

public:
    HashMap() {
        capacity = 4; // Initial Capacity
        size = 0;
        
        for (int i = 0; i < capacity; i++) {
            buckets.append(DoublyLinkedList<Node<K, V>>());
        }
    }

    
    HashMap(const HashMap& other) {
        capacity = other.capacity;
        size = other.size;
        buckets = other.buckets;  
    }

  
    HashMap& operator=(const HashMap& other) {
        if (this == &other) return *this;
        capacity = other.capacity;
        size = other.size;
        buckets = other.buckets;
        return *this;
    }

    
    ~HashMap() {
        // The buckets (DynamicArray) will automatically call its own destructor,
        // which safely cleans up all DoublyLinkedLists and their Nodes.
        size = 0;
        capacity = 0;
    }

    
    void set(K key, V value) {
        
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
        
       
        chain.append(Entry<K, V>(key, value));
        size++;
    }

    
    V get(K key) const {
        int idx = getBucketIndex(key);
        DoublyLinkedList<Node<K, V>>& chain = buckets.get(idx);
        
        int chainSize = chain.getSize();
        for (int i = 0; i < chainSize; i++) {
            Node<K, V> entry = chain.get(i);
            if (entry.key == key) {
                return entry.value;
            }
        }
        throw "Key not found";
    }

    // Remove key-value pair. O(1) average, worst O(n) based on chain length
    void remove(K key) {
        int idx = getBucketIndex(key);
        DoublyLinkedList<Node<K, V>>& chain = buckets.get(idx);
        
        int chainSize = chain.getSize();
        for (int i = 0; i < chainSize; i++) {
            Node<K, V> entry = chain.get(i);
            if (entry.key == key) {
                chain.remove(i);
                size--;
                return;
            }
        }
        throw "Key not found";
    }

    int getSize() const {
        return size;
    }
};

#endif 
