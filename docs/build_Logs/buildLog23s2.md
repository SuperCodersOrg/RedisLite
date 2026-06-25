# Date: June 23 
Session: 2
Duration: 45 minutes 

Goal: 
1. Implement a generic Doubly Linked List (`DoublyLinkedList`) in a separate file.
2. Ensure proper support for complex C++ objects in `DynamicArray` by resolving missing placement `new` requirements.

Problem Encountered: 
1. The Singly Linked List (`LinkedList`) had an O(n) `append` method and only supported forward traversal. We needed to implement a `DoublyLinkedList` with `prev` pointers and a `tail` pointer to enable O(1) appends and optimized bidirectional traversal.
2. In `DynamicArray`, we were already using placement `new` syntax (e.g., `new(&arr[i]) T(val);`) to correctly initialize generic objects within `malloc` allocated memory. However, we had omitted the `#include <new>` header, which is strictly required for placement `new` to compile and function correctly.


## What I Tried: 
1. Created `doublyLinkedList.cpp` as an independent class. Reusing the `LinkedList` class through inheritance would have required overriding almost all methods due to the structural differences (the addition of `prev` and `tail` pointers), providing no real code-reuse benefits.
2. Implemented the `Node` struct for the Doubly Linked List with both `prev` and `next` pointers.
3. Added a `tail` pointer to the `DoublyLinkedList` class, reducing the `append` time complexity from O(n) to O(1).
4. Optimized the `get(int index)` method to intelligently traverse from the `head` if the target index is in the first half of the list, or traverse backwards from the `tail` if the index is in the second half.
5. Updated `dynamicArray.cpp` to add `#include <new>` at the top of the file to ensure full compatibility and successful compilation of placement `new` operations.

## Outcome: 
Successfully built `DoublyLinkedList` with enhanced performance characteristics, including O(1) appends, bidirectional traversal, and faster average-case `get` operations. Furthermore, `DynamicArray` is now robustly configured with the correct `<new>` library to flawlessly construct and destruct non-primitive C++ types within manually allocated C-style memory blocks.
