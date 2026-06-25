# Date: June 25 
Duration: 60 minutes 

Goal: 
Implement a custom, generic `HashMap` data structure from scratch, utilizing the previously built `DynamicArray` and `DoublyLinkedList` for Separate Chaining, without relying on STL libraries.

Problem Encountered: 
1. **Naming Mismatch in Types**: The internal structure was defined as `Node<K, V>`, but the `set`, `get`, and `remove` methods attempted to use an undeclared `Entry<K, V>` struct. 
2. **Missing Custom Hash Function**: Since `std::hash` cannot be used due to zero-STL constraints, the implementation depends on a custom `hashFunction.cpp` (`HashFunction hashGenerator;`) which needs to be carefully designed to prevent excessive collisions.
3. **Resizing Logic**: Ensuring that `rehash()` safely transfers memory by utilizing the copy constructors of the underlying custom data structures without causing memory leaks.

## error
      |                               ^~~~~~~
hashmap.cpp:122:20: error: expected primary-expression before ',' token [-Wtemplate-body]
  122 |             Entry<K, V> entry = chain.get(i);        
      |                    ^
hashmap.cpp:122:23: error: expected primary-expression before '>' token [-Wtemplate-body]
  122 |             Entry<K, V> entry = chain.get(i);        
      |                       ^
hashmap.cpp:122:25: error: 'entry' was not declared in this scope [-Wtemplate-body]
  122 |             Entry<K, V> entry = chain.get(i);        
      |                         ^~~~~
hashmap.cpp:122:39: error: request for member 'get' in 'chai 
', which is of non-class type 'int' [-Wtemplate-body]        
  122 |             Entry<K, V> entry = chain.get(i);        
      |                                       ^~~
hashmap.cpp: In member function 'void HashMap<K, V>::remove(K)':
hashmap.cpp:133:26: error: 'Entry' was not declared in this scope [-Wtemplate-body]
  133 |         DoublyLinkedList<Entry<K, V>>& chain = buckets.get(idx);
      |                          ^~~~~
hashmap.cpp:133:36: error: wrong number of template arguments (2, should be 1) [-Wtemplate-body]
  133 |         DoublyLinkedList<Entry<K, V>>& chain = buckets.get(idx);
      |                                    ^~
doublyLinkedList.cpp:15:7: note: provided for 'template<class T> class DoublyLinkedList'
   15 | class DoublyLinkedList {
      |       ^~~~~~~~~~~~~~~~
hashmap.cpp:135:31: error: request for member 'getSize' in 'chain', which is of non-class type 'int' [-Wtemplate-body]    
  135 |         int chainSize = chain.getSize();
      |                               ^~~~~~~
hashmap.cpp:137:20: error: expected primary-expression before ',' token [-Wtemplate-body]
  137 |             Entry<K, V> entry = chain.get(i);        
      |                    ^
hashmap.cpp:137:23: error: expected primary-expression before '>' token [-Wtemplate-body]
  137 |             Entry<K, V> entry = chain.get(i);        
      |                       ^
hashmap.cpp:137:25: error: 'entry' was not declared in this scope [-Wtemplate-body]
  137 |             Entry<K, V> entry = chain.get(i);        
      |                         ^~~~~
hashmap.cpp:137:39: error: request for member 'get' in 'chai 
', which is of non-class type 'int' [-Wtemplate-body]        
hashmap.cpp:137:23: error: expected primary-expression before '>' token [-Wtemplate-body]
  137 |             Entry<K, V> entry = chain.get(i);        
      |                       ^
hashmap.cpp:137:25: error: 'entry' was not declared in this scope [-Wtemplate-body]
  137 |             Entry<K, V> entry = chain.get(i);        
      |                         ^~~~~
hashmap.cpp:137:39: error: request for member 'get' in 'chai 
', which is of non-class type 'int' [-Wtemplate-body]        
cope [-Wtemplate-body]
  137 |             Entry<K, V> entry = chain.get(i);        
      |                         ^~~~~
hashmap.cpp:137:39: error: request for member 'get' in 'chai 
', which is of non-class type 'int' [-Wtemplate-body]        
hashmap.cpp:137:39: error: request for member 'get' in 'chai 
', which is of non-class type 'int' [-Wtemplate-body]        
', which is of non-class type 'int' [-Wtemplate-body]        
  137 |             Entry<K, V> entry = chain.get(i);
      |                                       ^~~
hashmap.cpp:139:23: error: request for member 'remove' in 'chain', which is of non-class type 'int' [-Wtemplate-body]
  139 |                 chain.remove(i);

## What I Tried: 
1. Set up the generic template `template<typename K, typename V>` for the `HashMap` class.
2. Initialized a `DynamicArray<DoublyLinkedList<Node<K, V>>>` to act as the bucket array.
3. Implemented `set`, `get`, and `remove` methods which hash the key, jump to the correct index in O(1) time, and iterate over the `DoublyLinkedList` to handle collisions.
4. Added a `rehash()` method that doubles capacity and safely re-maps all existing key-value pairs when the load factor exceeds 0.75.
5. Cleaned up excessive comments from `hashMap.cpp` to keep the codebase looking clean and professional.

## Outcome: 
The foundational core of the `HashMap` is successfully written! The logic for O(1) amortized insertion, retrieval, and separate chaining is fully in place. **Next immediate step:** Rename `Entry` to `Node` inside the `set`, `get`, and `remove` methods to fix the current compilation error, and finalize the `HashFunction` implementation to generate deterministic indices.
