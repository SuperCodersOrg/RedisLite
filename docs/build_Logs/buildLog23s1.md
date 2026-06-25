# Date: June 23 
Duration: 45 minutes 

Goal: 
Implement a generic Singly Linked List (`LinkedList`) in C++ for the `SuperCodersCollectionsLibrery` from scratch, manually managing memory with `malloc` and `free` instead of standard C++ object allocators (`new` and `delete`).

Problem Encountered: 
1. Managing memory manually for a generic template type `T` poses a problem: `malloc` only allocates raw memory bytes and does not invoke the constructor for the object `T`. This is a critical issue if `T` is a non-primitive type (like a custom class or string) that requires proper initialization.
2. Similarly, using `free` does not invoke the destructor for `T`. If the object stored inside the node holds its own internal heap memory, simply freeing the node's memory will cause severe memory leaks.

## error in the code : 
linkedlist.cpp: In destructor 'LinkedList<T>::~LinkedList()':     
linkedlist.cpp:48:9: error: '_size' was not declared in this scope
; did you mean 'size'? [-Wtemplate-body]
   48 |         _size = 0;
      |         ^~~~~
      |         size
linkedlist.cpp: In member function 'void LinkedList<T>::insert(int
, T)':
linkedlist.cpp:105:9: error: '_size' was not declared in this scop
e; did you mean 'size'? [-Wtemplate-body]
  105 |         _size++;
      |         ^~~~~
      |         size

D:\projects\RedisLite\src\SuperCodersCollectionsLibrery>g++ -c lin
kedlist.cpp
linkedlist.cpp: In member function 'void LinkedList<T>::insert(int
, T)':
  105 |         _size++;
      |         ^~

## What I Tried: 
1. Designed a lazy memory allocation strategy where the constructor simply initializes `head` to `nullptr` and `size` to `0` without allocating any node memory upfront.
2. Researched methods to bridge raw C-style memory allocation with C++ object lifecycles for templates.
3. Utilized **placement `new`** (`new (&newNode->data) T(val);`) to explicitly construct the generic object directly into the exact memory address dynamically allocated by `malloc`.
4. Before freeing a node, I explicitly invoked the destructor for the data object (`temp->data.~T();`) to ensure safe cleanup of the generic type, immediately followed by `free(temp);` to release the node.

## Outcome: 
Successfully built a fully generic Singly Linked List featuring `append` O(n), `get` O(n), `insert` O(n), `remove` O(n), and `getSize` O(1) operations. Memory is correctly managed using strict C-style `malloc` and `free`, while safely accommodating complex C++ objects via placement `new` and explicit destructor calls, keeping the structure robust and leak-free.
