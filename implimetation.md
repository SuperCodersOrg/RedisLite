# DESIGN PROPOSAL
# SuperCoders Collections Library & Redis Lite
## Introduction
Redis Lite is a lightweight, in-memory key-value database designed to demonstrate the internal architecture and working principles of modern caching systems such as Redis. The project focuses on implementing all core data structures manually in C++ without relying on STL containers, thereby providing a deeper understanding of memory management, hashing techniques, collision handling, cache eviction policies, and generic programming.
The primary objective of Redis Lite is to create a scalable and efficient storage system capable of performing key-value operations with constant average-time complexity while maintaining controlled memory usage. The architecture is designed to be modular, extensible, and suitable for future enhancements.
## SuperCoders Collections Library
Before Redis Lite, introduce the custom library.
### Dynamic Array
Purpose
Provides resizable contiguous storage similar to vector.
#### Why Dynamic Array?
Fast random access O(1) 
Required for bucket storage in HashMap 
Efficient memory utilization 
#### Methods:-
append() - O(1) amortized , get(index) - O(1) , insert(index) - O(n), remove(index) - O(n)
#### Generic Design
template<typename T>
class DynamicArray;
#### Why Generic?
A generic implementation allows the same Dynamic Array to store different data types without modifying the underlying code.
Examples:
DynamicArray<int>
DynamicArray<string>
DynamicArray<Node*>
DynamicArray<Entry<K,V>>
Benefits:
Code reusability 
Type safety 
Scalability 
Support for user-defined objects
Memory Management
Initial Capacity = 4
Reason:
Small memory footprint 
Suitable for lightweight applications 
Reduces initial memory waste 
Growth Strategy:
4 → 8 → 16 → 32 → ...
Capacity doubles when full.
Benefits:
Amortized O(1) insertion 
Fewer reallocations 

### Doubly Linked List
Purpose
Stores nodes through pointer connections.
#### Why Doubly Linked List?
Chosen over Singly Linked List because LFU cache requires:
Forward traversal 
Backward traversal 
O(1) node removal 
#### Methods:-
insertFront - O(1) , deleteFront - O(1) , search - O(n) , insert(index) O(n) 
#### Generic Design
template<typename T>
class DoublyLinkedList;
The generic implementation allows the same linked list to store different data types and be reused throughout the SuperCoders Collections Library, including HashMap bucket chains and LFU frequency lists.
#### Memory Management
Each node stores:
Data
Next Pointer
Previous Pointer
Nodes are dynamically allocated during insertion and released during deletion.

### Generic HashMap
Purpose
Provides key-value storage.
#### Why HashMap?
O(1) average insertion 
O(1) average lookup 
O(1) average deletion 
Ideal for Redis-style databases.
#### Methods:-
set - O(1) average,  get - O(1), average - rehash O(1),  remove - average O(n) 
#### Generic Design Using Templates
To maximize code reusability and flexibility, all major data structures will be implemented using C++ templates.
template<typename K, typename V>
class HashMap;
This generic implementation allows the same HashMap to store various data types without modifying the core implementation.
Supported Key-Value Combinations
int → string
string → string
string → User Object
CustomClass → Record
For primitive data types, hashing can be generated directly. For user-defined classes, a custom hash function must be implemented.
Generic Hash Function Implementation
Since STL hashing functions cannot be used, Redis Lite will implement its own generic hashing mechanism.
For primitive types:
int hashCode(int key){
    return key;
}
For strings:
int hashCode(string key){
    int hash = 0;

    for(char ch : key){
        hash = (hash * 31) + ch;
    }
// 31 is prime.
// Prime numbers generally distribute values more uniformly and reduce collisions.

    return hash;
}
For custom objects:
class User{
public:
    string name;
    int id;
    int hashCode() const{
        return id;
    }
};
This approach ensures extensibility while maintaining compatibility with generic programming principles.
#### Why Generic?
Code reusability 
Type independence 
Supports primitive and user-defined objects 
Examples:
string → string
int → string
string → User
## Redis Lite Architecture Overview
Redis Lite follows a layered architecture in which each custom data structure is responsible for a specific task.
### The system combines:
Generic HashMap
Linked Lists
Custom Hash Functions
Load Factor Monitoring
Dynamic Rehashing
LFU (Least Frequently Used) Cache Management
The HashMap acts as the primary storage engine, while Linked Lists manage collisions. Frequency-based lists are used for cache eviction and memory optimization.

### High-Level Architecture
                         Client Commands
                              ▼
                         Redis Lite Engine
              ┌───────────────┬───────────────┐
              ▼               ▼               ▼
          HashMap         Rehashing       LFU Cache
              ▼
  Linked List Chains
This modular design improves maintainability, scalability, and future feature integration.

### HashMap as the Primary Storage Engine
The core functionality of Redis Lite revolves around key-value storage operations.
Typical commands include:
SET name Yash
GET name
DEL name
These commands map directly to HashMap operations.
Advantages of HashMap
O(1) average insertion
O(1) average lookup
O(1) average deletion
Direct key-based access
Efficient memory utilization
Because Redis itself is fundamentally a key-value store, HashMap is the most suitable choice for implementing the storage layer.

### Collision Handling Using Linked Lists
Hash collisions occur when multiple keys generate the same bucket index.
Example
hash(name) % 8 = 3
hash(city) % 8 = 3
Both keys point to Bucket 3.
To resolve this issue, Redis Lite will implement Separate Chaining using custom Linked Lists.
Bucket 3
[name : Yash]
       ▼
[city : Delhi]
Benefits
Simple implementation
Efficient insertion and deletion
Supports unlimited entries per bucket
Easy integration with generic HashMap
Each bucket stores a linked list of nodes, where each node contains:
Key
Value
Next Pointer
This design ensures that collisions do not overwrite existing data.

### Load Factor Monitoring and Rehashing
The performance of a HashMap largely depends on maintaining a balanced distribution of keys across buckets.
Load Factor
Load Factor = Number of Elements / Number of Buckets
When the load factor exceeds 0.75, the system initiates rehashing.
Rehashing Process
Allocate a new bucket array.
Double the bucket count.
Recalculate bucket indices.
Reinsert all existing elements.
Delete the old bucket array.
Benefits of Rehashing
Reduces collision frequency.
Improves lookup efficiency.
Maintains O(1) average complexity.
Enhances scalability as data grows.
Without rehashing, bucket chains would continue growing, degrading performance toward O(n).

### LFU Cache Management
Since Redis Lite is an in-memory database, memory capacity must be managed efficiently.
To prevent unlimited memory growth, Redis Lite implements an LFU (Least Frequently Used) eviction strategy.
Each cache entry stores:
Key
Value
Frequency
Whenever a successful GET operation occurs, the frequency count is incremented.
Example
name    → 20 accesses
city    → 5 accesses
country → 1 access
When memory capacity is reached, the entry with the lowest frequency count is removed before inserting new data.
Advantages
Retains frequently used data.
Improves cache hit ratio.
Optimizes memory usage.
Mimics production-grade caching systems.

### O(1) LFU Architecture
To achieve constant-time cache operations, Redis Lite combines:
HashMap<Key, Node*>
+
Frequency Lists
Internal Structure
HashMap
    ▼
Key → Node*
    ▼
Frequency Lists
Freq 1 → [A] [B]
Freq 2 → [C]
Freq 3 → [D]
Working Principle
HashMap provides direct access to cache nodes.
Frequency lists group nodes according to access count.
Frequency updates occur by moving nodes between lists.
Eviction removes nodes from the lowest-frequency list.

Time Complexity
Operation
Complexity
GET
O(1)
SET
O(1)
Frequency Update
O(1)
LFU Eviction
O(1)

This architecture avoids scanning the entire cache and provides highly efficient memory management.

Node architecture:-
template<typename K, typename V>
class Node
{
public:
    K key;
    V value;
    int frequency;
    // TTL Information (Future)
    long long creationTime;
    long long expiryTime;
    // HashMap Collision Chain
    Node<K,V>* bucketNext;
    // LFU Frequency List
    Node<K,V>* freqPrev;
    Node<K,V>* freqNext;
}


### Architecture Summary
Component
Responsibility
Generic HashMap<K,V>
Primary key-value storage
Linked List
Collision handling through separate chaining
Custom Hash Function
Bucket index generation
Load Factor Monitor
Performance monitoring
Rehashing System
Dynamic bucket expansion
LFU Frequency Lists
O(1) cache eviction mechanism

Super Structure
                            ┌─────────────────────┐
                            │      Redis Lite     │
                            └──────────┬──────────┘                   
                                       │ Imports
                                       ▼
         ┌─────────────────────────────────────────┐
         │   SuperCoders Collections Library       │
         └──────────────────┬──────────────────────┘
                            │   
         ┌──────────────────┼────────────────────┐
         ▼                  ▼                    ▼
┌────────────┐    ┌──────────────┐    ┌──────────────┐
│DynamicArray│    │DoublyLinked  │    │   HashMap    │
│   <T>      │    │  List<T>     │    │   <K,V>      │
└────────────┘    └──────────────┘    └──────┬──────┘
                                              │
                        ┌────────────────────┴────────────────┐
                        │                                     │
                        ▼                                     ▼
                Uses DynamicArray                  Uses DoublyLinkedList
                for Bucket Storage                  for Collision Chains

### Memory Management Strategy
Since Redis Lite is implemented without STL containers, all memory is managed manually using dynamic allocation. Proper memory management ensures efficient resource utilization and prevents memory leaks.
Constructor Responsibilities
DynamicArray starts with an initial capacity of 4 elements and grows dynamically when full.
4 → 8 → 16 → 32 → 64
Dynamic Growth and Rehashing
HashMap continuously monitors its load factor:
Load Factor = Elements / Buckets
When the load factor exceeds 0.75, rehashing is triggered:
16 → 32 → 64 → 128 Buckets
A new bucket array is allocated, all entries are reinserted, and the old bucket array is released.
Destructor Responsibilities
Before destruction, all dynamically allocated memory is released:
HashMap:
Delete All Nodes
      ↓
Delete Bucket Array

LinkedList:
Delete Nodes One By One

DynamicArray:
delete[] data
Rule of Three
Since the data structures manage dynamic memory manually, classes that allocate memory implement the Rule of Three:
Destructor 
Copy Constructor 
Copy Assignment Operator 
These functions ensure proper ownership of dynamically allocated memory and perform deep copying when objects are copied. This prevents memory leaks, dangling pointers, shallow-copy issues, and double-free errors.
### Memory Optimization
Redis Lite uses LFU (Least Frequently Used) eviction to control memory usage. When the cache reaches its maximum capacity, the least frequently used entry is removed before inserting new data, ensuring efficient memory utilization and preventing uncontrolled memory growth.
### Future Optimizations
#### Time-Based Expiration (TTL Cache)
Future versions of Redis Lite will support Time-To-Live (TTL) functionality similar to Redis. Each cache node will store an expiration timestamp in addition to key, value, and frequency information. During access operations, Redis Lite will verify whether the entry has expired and automatically remove invalid data. Expired entries will be prioritized for eviction before applying the LFU policy. This enhancement will allow Redis Lite to support temporary caching, session management, and automatic memory cleanup while maintaining efficient lookup performance.
### Conclusion
Redis Lite presents a comprehensive implementation of an in-memory key-value database using entirely custom-built data structures. The system combines generic programming, custom hashing techniques, linked-list-based collision handling, load-factor-based rehashing, and LFU cache eviction to achieve high performance and scalability.
The architecture closely resembles real-world cache systems while remaining educational and fully implementable without STL containers. Through this project, advanced concepts such as memory management, hashing, cache optimization, and system architecture are explored in depth, making Redis Lite both a practical and academically valuable software engineering project.

