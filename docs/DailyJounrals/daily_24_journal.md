# Daily Design Journal · daily_24_journal.md

## Section 1 — Specific Bug
Encountered an undeclared identifier compilation error when writing the `HashMap` class. The internal struct was defined as `Node<K, V>`, but the insertion logic attempted to instantiate an `Entry<K, V>` object:
```text
hashMap.cpp:114:22: error: 'Entry' was not declared in this scope
  114 |         chain.append(Entry<K, V>(key, value));
      |                      ^~~~~
hashMap.cpp:121:26: error: 'Entry' was not declared in this scope
  121 |         DoublyLinkedList<Entry<K, V>>& chain = buckets.get(idx);
```
**Fix:** Renamed all instances of `Entry` back to `Node` inside the `set`, `get`, and `remove` methods to align with the struct definition at the top of the file.

## Section 2 — Failed Attempt
When trying to bundle `linkedList.cpp` and `doublyLinkedList.cpp` into a single `SuperCodersCollections.h` master header, compilation failed massively due to a naming collision. Both linked list implementations independently used a struct named `Node`. My first thought was that putting them in separate files would keep them isolated. However, since C++ templates require implementations to be `#include`'d directly, the compiler pulled both `Node` definitions into the same global space, triggering a "redefinition of 'struct Node'" error. I had to explicitly rename them to `SNode` (Singly) and `DNode` (Doubly) so they could safely coexist.

## Section 3 — Memory Diagram
[ Memory Diagram: HashMap with Separate Chaining ]

DynamicArray (Buckets)
Capacity: 4, Size: 3

[ 0 ] ---> DoublyLinkedList <Node> 
            [ Prev: NULL | Key: "Bob", Val: 92 | Next: NULL ]

[ 1 ] ---> DoublyLinkedList (Empty)
            NULL

[ 2 ] ---> DoublyLinkedList <Node> (Collision handled via chaining!)
            [ Prev: NULL | Key: "Alice", Val: 85 | Next: 0x2048 ]
               <====>
            [ Prev: 0x2000 | Key: "Eve", Val: 99 | Next: NULL ]

[ 3 ] ---> DoublyLinkedList (Empty)
            NULL
`

## Section 4 — Code Reference
- **Commit Hash:** N/A (Pending Local Commit)
- **Filenames & Lines:**
  - `SuperCodersCollections.h` (Lines 1-9: Implementation of `#ifndef` header guards to prevent multiple inclusion errors).
  - `hashMap.cpp` (Lines 34-57: `rehash()` logic doubling capacity and safely deep-copying old chains into new buckets).
  - `hashMap.cpp` (Lines 91-116: The `set()` method utilizing O(1) array jumps and DoublyLinkedLists for collision resolution).

#### Implemented HashMap Functions
- **`set(K key, V value)`**: Amortized O(1) insertion and updates. Hashes the key, finds the bucket, and resolves collisions by traversing or appending to the Doubly Linked List.
- **`get(K key)`**: Amortized O(1) retrieval. Locates the bucket via hash and searches the chain for the corresponding key.
- **`remove(K key)`**: Amortized O(1) deletion. Removes the key-value node directly from the underlying linked list.
- **`getSize()`**: O(1) retrieval of the total active key-value pairs stored in the map.
- **`rehash()`**: O(n) operation triggered automatically when the load factor exceeds 0.75. Doubles the bucket capacity and redistributes all existing nodes to prevent heavy collisions.
- **Rule of Three (Memory Management)**:
  - **Copy Constructor**: Deep copies the entire `DynamicArray` of buckets, ensuring all `DoublyLinkedList` chains are fully replicated.
  - **Copy Assignment Operator**: Safely transfers deep copies between two existing HashMaps.
  - **Destructor**: Relies on `DynamicArray` to cascade deletion down through all `DoublyLinkedList` nodes, ensuring zero memory leaks.

## Section 5 — Learning Reflection
Today's deep dive into building a generic HashMap solidified my understanding of how advanced data structures are simply clever combinations of simpler ones. A Hash Map is just a dynamic array that uses a hash function + modulo operator to instantly jump to an index. When that trick results in a collision, it falls back on a linked list. 

Furthermore, I learned a lot about C++ preprocessor directives. I previously took `#include` for granted, not realizing it literally copies and pastes the entire contents of the target file into your current file before compiling. Learning to manually implement Header Guards (`#ifndef`, `#define`, `#endif`) was a crucial lesson in writing scalable, modular C++ libraries that don't implode when imported multiple times.
