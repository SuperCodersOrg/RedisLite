# Date: June 24 
Duration: 45 minutes 

Goal: 
Prepare the `SuperCodersCollectionsLibrary` so that `DynamicArray`, `LinkedList`, and `DoublyLinkedList` can be safely imported together into a single file to build the custom `HashMap` class.

Problem Encountered: 
1. Including multiple files in one header often leads to "redefinition" errors if the same file is evaluated more than once by the compiler.
2. Both `LinkedList` and `DoublyLinkedList` used an internal struct named `Node`, which caused a naming collision when imported together into the same project environment.

## error in the code : 

SuperCodersCollectionsLibrery\doublyLinkedList.cpp:5:8: error: redefinition of 'struct Node'
    5 | struct Node {
      |        ^~~~
SuperCodersCollectionsLibrery\linkedList.cpp:5:8: note: previous definition of 'struct Node'
    5 | struct Node {
      |        ^~~~
```

## What I Tried: 
1. Created a master header file `SuperCodersCollections.h` to act as a central hub, directly including the template implementation `.cpp` files.
2. Implemented Include Guards (`#ifndef`, `#define`, `#endif`) across all custom collection files to ensure the compiler reads each class structure only once.
3. Researched and resolved the `Node` collision by providing unique identifier prefixes (`SNode` for Singly Linked List, `DNode` for Doubly Linked List).

## Outcome: 
Successfully integrated the custom library elements into a single `SuperCodersCollections.h` master file without standard library helpers. All struct naming conflicts are permanently resolved, and include guards properly shield the code from multiple inclusion errors. The generic templates are now fully ready to be imported and used together via a single include statement for building the custom HashMap class using Separate Chaining.
