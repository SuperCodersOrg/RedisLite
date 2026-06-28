# Daily Design Journal · daily_25_journal.md

## Section 1 — Specific Bug
Encountered a compile-time routing issue when trying to use custom, user-defined objects (like a `struct Player`) as keys in the `HashMap`. Because I am strictly avoiding the standard library (`std::hash`), the compiler threw an error when attempting to hash an unknown object:
```text
error: no matching function for call to 'HashFunction::generate(const Player&)'
```
**Fix:** Implemented a two-step fallback mechanism using C++ templates. First, I used SFINAE (`decltype`) to check if the custom object has its own `hashCode()` method. If not, I implemented `generateFallback()`, which forcefully casts the object's memory address into a raw byte array (`unsigned char*`) and hashes the physical memory block directly.

## Section 2 — Failed Attempt
Initially, my thought process was to write a single, massive generic template `template <typename T> int generate(T key)` to handle all possible data types. This approach completely failed conceptually. A single mathematical formula cannot efficiently handle both an `int` (which should just return itself in O(1) time) and a `std::string` (which requires looping over every character). I realized that trying to force a "one-size-fits-all" solution would ruin the performance of primitive types. I abandoned the single template in favor of **Method Overloading**, allowing the C++ compiler to automatically route the data type to a highly specialized algorithm at compile-time.

## Section 3 — Memory Diagram

```text
[ Memory Diagram: The Byte-Wise Fallback Hash ]

User-Defined Object (e.g., struct Player { int id; char rank; })
Located at Memory Address: 0x4000

Step 1: Cast the object pointer to (unsigned char*)
0x4000: [ 0A ]  (Byte 0)
0x4001: [ 00 ]  (Byte 1)
0x4002: [ 00 ]  (Byte 2)
0x4003: [ 00 ]  (Byte 3)
0x4004: [ 42 ]  (Byte 4)

Step 2: Loop through sizeof(T) and apply Polynomial Rolling Hash
hash = 0
hash = (0 * 31) + 0x0A
hash = (10 * 31) + 0x00 ...
```

## Section 4 — Code Reference
- **Commit Hash:** N/A (Pending Local Commit)
- **Filenames & Lines:**
  - `hashFunction.cpp` (Lines 9-19: Method overloading for primitives `int`, `char`, `bool` to return O(1) instant hashes).
  - `hashFunction.cpp` (Lines 21-30: The `std::string` polynomial rolling hash using multiplier 31).
  - `hashFunction.cpp` (Lines 41-54: The `generateFallback` template interpreting custom objects as raw memory bytes).

## Section 5 — Learning Reflection
Today's session on implementing a custom `HashFunction` completely shifted how I view data types in C++. 

By using Method Overloading, I learned how to let the compiler do the heavy lifting. Instead of writing complex `if-else` logic to check variable types at runtime (which slows down the HashMap), overloading determines the correct path during compilation. Primitives get instant conversions, while strings get a careful polynomial distribution to avoid anagram collisions.

The biggest breakthrough, however, was writing the fallback method for User-Defined Objects. It taught me a fundamental truth about computer science: *everything* is just a block of bytes in memory. It doesn't matter how complex an object is; if you cast its address to an `unsigned char*`, you can mathematically process its physical memory footprint. This guarantees that absolutely any object can be converted into a deterministic integer hash without crashing the program.
