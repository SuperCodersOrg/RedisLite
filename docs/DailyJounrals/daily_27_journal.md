# Daily Design Journal · daily_27_journal.md

## Section 1 — Specific Bug
Encountered "Undefined Reference" linker errors and "Multiple Definition" errors when separating class template declarations (`.h`) from their implementations (`.cpp`) to compile them into a static library via CMake. Standard C++ compilers strictly require template definitions to be visible during compilation wherever they are instantiated.
**Fix:** Modified the `.h` files to `#include "../src/customLibrery/filename.cpp"` at the very bottom. For non-template functions inside `hashFunction.cpp`, I added the `inline` keyword to instruct the linker to merge duplicates and bypass One Definition Rule (ODR) violations.

## Section 2 — Failed Attempt
Initially, my thought process was to completely isolate the `.cpp` files in a `src/` directory and instruct `CMakeLists.txt` to compile them into an entirely independent library archive (`libCustomLibrary.a`). This approach completely failed conceptually for templates. Because the compiler processes each `.cpp` file sequentially and doesn't see any template instantiations within those library files, it produces functionally "empty" object files. When `main.cpp` later tried to link against the library, the necessary machine code was missing. I abandoned the strict separation in favor of the C++ header-inclusion strategy.

## Section 3 — Memory Diagram (Architecture Diagram)

```text
[ Architecture Diagram: C++ Template Inclusion ]

File: include/dynamicArray.h
+-----------------------------------+
| #pragma once                      |
| template<typename T>              |
| class DynamicArray {              |
|   void append(T val);             |
| };                                |
|                                   |
| // Template Instantiation trick   |
| #include "../src/dynamicArray.cpp"|
+-----------------------------------+
                 |
                 v
File: src/customLibrery/dynamicArray.cpp
+-----------------------------------+
| #pragma once                      |
| template<typename T>              |
| void DynamicArray<T>::append(...) |
|   ...                             |
| }                                 |
+-----------------------------------+
```

## Section 4 — Code Reference
- **Commit Hash:** N/A (Pending Local Commit)
- **Filenames & Lines:**
  - `include/*.h` (Added `#include "../src/customLibrery/...cpp"` at the bottom of all headers to dynamically expose template definitions).
  - `src/customLibrery/hashFunction.cpp` (Lines 7-24: Added `inline` keyword to non-template `generate` functions to prevent ODR linker crashes).
  - `CMakeLists.txt` (Complete build system automation exposing `include/` publicly and linking `main.cpp`).

## Section 5 — Learning Reflection
Today's session completely demystified the often confusing relationship between C++ templates, header files, and the compilation process. 

I learned that C++ templates are not actual code—they are blueprints. The compiler only generates the actual machine code when it sees a specific instantiation (like `DynamicArray<int>`). This completely flips the standard "header for declarations, cpp for implementations" rule on its head, forcing you to ensure the implementation is visible *anywhere* the template is used.

Additionally, encountering the One Definition Rule (ODR) error taught me the immense value of the `inline` keyword. Instead of just being a speed optimization hint for the compiler, it acts as a critical signal to the linker, explicitly authorizing it to safely merge identical function definitions that stem from widespread header inclusions. Moving from manual compilation to a scalable `CMakeLists.txt` tied it all together, showing how a modern, modular C++ project is professionally orchestrated from source code to final executable.
