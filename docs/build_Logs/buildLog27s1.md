# Date: June 27
Duration: 100 minutes 

Goal: 
Refactor the custom collections library into a modular architecture (separating declarations into `include` and implementations into `src`) and setup a scalable `CMakeLists.txt` build system to compile the entire project.

Problem Encountered: 
1. **The C++ Template Limitation**: Standard C++ doesn't allow template definitions to be easily compiled into independent `.cpp` libraries because templates must be completely visible to the compiler at the time they are instantiated. Separating them completely results in fatal "Undefined Reference" linker errors.
2. **One Definition Rule (ODR) Violations**: If a regular, non-template method is defined in a file that is included across multiple places, the compiler creates duplicates, crashing the linker with a "Multiple Definition Error".
3. **Build Automation**: Compiling multiple folders and linking libraries manually via `g++` is tedious and prone to errors.

## What I Tried: 
1. **Modularizing the Codebase**: Created specific header files (`dynamicArray.h`, `hashMap.h`, etc.) in the `include` folder containing only the class declarations. Moved the actual implementation logic into corresponding `.cpp` files in `src/customLibrery/`.
2. **Bypassing the Template Linking Issue**: To solve the C++ template limitation without breaking our new folder structure, I cleverly added an `#include "../src/customLibrery/filename.cpp"` at the very bottom of each `.h` file. This securely provides the implementation to the compiler on-demand while maintaining visual separation.
3. **Fixing ODR with `inline`**: Used the `inline` keyword on the non-template `generate` functions inside `hashFunction.cpp`. This instructed the compiler to safely merge identical definitions instead of crashing the linker.
4. **Setting up CMake**: Built a `CMakeLists.txt` that utilizes `add_library` to bundle the source files into `CustomLibrary`, exposes the `include` directory via `target_include_directories`, and successfully links the library to a new executable testing suite in `main.cpp`.
5. **Modernizing Headers**: Added `#pragma once` to the top of every file to optimize compilation speed and cleanly prevent duplicate file processing.

## Outcome: 
The codebase has successfully transitioned from a manual script-based build to a professional, scalable CMake architecture. Our custom C++ templated collections (`DynamicArray`, `LinkedList`, `DoublyLinkedList`, `HashMap`) are now perfectly separated and globally accessible simply by writing `#include "SuperCodersCollections.h"`. A full test suite was executed via CMake, verifying that every data structure hashes, inserts, appends, and removes data flawlessly!
