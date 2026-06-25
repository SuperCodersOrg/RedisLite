# Date: June 25 
Duration: 45 minutes 

Goal: 
Implement a custom, stateless `HashFunction` utility class from scratch to support the `HashMap`, completely avoiding the STL `<functional>` library and `std::hash`.

Problem Encountered: 
1. **Handling Multiple Data Types**: The HashMap needs to accept keys of various types (like `int`, `char`, `std::string`), requiring a way to route them to the correct hashing algorithm without slowing down performance.
2. **The "User-Defined Object" Problem**: What happens if the user creates a completely custom class (like `struct Player`) and tries to use it as a HashMap key? The C++ compiler has no idea how to convert a `Player` into a number. If the `HashFunction` doesn't know how to handle it, the program will crash during compilation because it can't find a matching method.


## What I Tried: 
1. Used **Method Overloading** to create separate, optimized `generate()` methods for basic primitives (`int`, `char`, `bool`). Since these are already numbers under the hood, they just return themselves in instant O(1) time.
2. Created a string hasher using a "Polynomial Rolling Hash" (looping through characters and multiplying the running total by the prime number `31`) to ensure an even distribution and prevent collisions for words with the same letters.
3. **Solved the User-Defined Object Problem in two simple steps:**
   - **Step A:** Created a clever template that politely checks if the user wrote their own `hashCode()` method inside their custom class. If they did, we use theirs!
   - **Step B (The Fallback):** If the user didn't write a `hashCode()` method, I built a `generateFallback` template. This forcefully breaks their custom object down into raw computer memory bytes (`unsigned char*`). It then loops through those raw memory bytes and hashes them directly. This guarantees that *any* object can be hashed, even if the user didn't prepare it!

## Outcome: 
The `HashFunction` is successfully implemented and fully integrated into the project. It correctly generates deterministic integers for primitives, strings, and completely custom objects without crashing. The `HashMap` now has the mathematical engine it needs to instantly calculate array buckets and resolve keys!
