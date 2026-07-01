# HashMap Implementation Plan

## Section 1 — Public API

List of proposed methods:

- `set(const K& key, const V& value)`: Inserts a new key-value pair, or updates the value if the key already exists.
- `get(const K& key)`: Retrieves the value associated with the key. Throws an error if the key doesn't exist.
- `remove(const K& key)`: Deletes a key-value pair from the map.
- `getSize()`: Returns the total number of key-value pairs stored.
- `empty()`: Returns true if the map is empty.
- `clear()`: Removes all key-value pairs from the map.
- `rehash()`: Resizes the hash table and rehashes all key-value pairs.
- `exists(const K& key)`: Checks if a specific key exists in the map.

**Why this decomposition is better:**
The `set` method handles both inserting brand new items and updating existing ones, which mirrors how standard maps work and keeps the API simple for the user. Because our map is generic, `get` cannot easily return a "null" value for primitive types like `int` if a key is missing. Therefore, `get` will throw an error for missing keys, making the `exists` method absolutely necessary so users can safely check for existence first. We pass keys and values by reference (`const K&`) to prevent the program from slowing down by copying large user-defined objects. 

## Section 2 — Internal Representation

The HashMap is a composition of our custom collections. The main storage is a `DynamicArray` which acts as our bucket array. Inside each bucket, we store a `DoublyLinkedList` that holds `Node<K, V>` objects. This structure directly handles collisions through a method called "separate chaining."

**Rule of Three (Memory Management):**
- **Destructor:** Because our HashMap is built purely out of our custom `DynamicArray` and `DoublyLinkedList` classes, we actually get memory safety for free. When the `HashMap` goes out of scope, the `DynamicArray`'s destructor is automatically called, which automatically calls the destructor of every `DoublyLinkedList` in its buckets, freeing all the heap-allocated nodes. We do not have to write manual `delete` statements inside the HashMap's destructor.
- **Copy Constructor and Copy Assignment:** We will rely on **deep copying**. Since our `DynamicArray` and `DoublyLinkedList` both have robust deep copy constructors already built, copying a `HashMap` simply invokes those existing deep copies. 
  - *Consequence of Deep Copy:* Complete safety. Two HashMaps can have the exact same data, but entirely different memory addresses for their buckets and nodes.
  - *Consequence of Shallow Copy (Rejected):* A shallow copy would make two HashMaps point to the exact same dynamic array of buckets. When destroyed, the system would attempt to delete the array twice, causing a fatal crash.

## Section 3 — Complexity Estimates

- **`set(const K& key, const V& value)`:**
  - **Best Case:** O(1)
  - **Why:** The hash function instantly calculates the bucket index. If the bucket is empty, we just append the node to the linked list immediately.
  - **Worst Case:** O(N)
  - **Why:** If all keys hash to the exact same bucket (a terrible hash function), the linked list becomes $N$ elements long. We have to traverse the entire list to ensure the key doesn't already exist before adding it. A resize operation (rehashing) also takes O(N) time.
  - **Average Case:** O(1) amortized
  - **Why:** Because we resize when the map gets 75% full, chains stay very short (usually 1 or 2 items). The O(N) cost of rehashing is spread out logarithmically, so the average cost is O(1).

- **`get(const K& key)` and `containsKey(const K& key)`:**
  - **Best Case:** O(1)
  - **Why:** The bucket index is calculated instantly, and the key is the very first item in that bucket's linked list.
  - **Worst Case:** O(N)
  - **Why:** Severe collisions mean we might have to scan a long linked list of $N$ items to find the key.
  - **Average Case:** O(1)
  - **Why:** Proper hashing keeps the linked list chains extremely short, so we typically only check 1 or 2 items.

- **`remove(const K& key)`:**
  - **Best Case:** O(1)
  - **Why:** The item to delete is the first one in the bucket, so the linked list pointer is rewired instantly.
  - **Worst Case:** O(N)
  - **Why:** We must traverse a long list of $N$ items to find the key to remove it.
  - **Average Case:** O(1)-(for all cases)
  - **Why:** Short bucket chains make removal near instant.

- **`size()`:** O(1)-(for all cases)
  - **Why:** The `size` is just an integer variable we increment/decrement that is updated on insertion and deletion. Returning it happens instantly.

## Section 4 — Design Decisions

**Why Choose Separate Chaining over Open Addressing?**
We chose **Separate Chaining** using our `DoublyLinkedList` to handle hash collisions. When two keys end up at the same bucket, we simply add the new entry to the linked list in that bucket. 

**Why Use DynamicArray and DoublyLinkedList?**
   main storage is a `DynamicArray` which acts as our bucket array. Inside each bucket, we store a `DoublyLinkedList` that holds `Node<K, V>` objects. This structure directly handles collisions through a method called "separate chaining."

**Why a Load Factor of 0.75?**
We chose a load factor threshold of 0.75. This means when the table is 75% full (e.g., 6 items in 8 buckets), we trigger a rehash, double the buckets, and redistribute the keys.
- *Alternatives Rejected:* A factor of 1.0 (waiting until it is 100% full) causes far too many collisions before resizing, significantly slowing down lookups. A factor of 0.5 (resizing at 50% full) wastes too much memory by keeping half the array empty at all times. 0.75 perfectly balances memory efficiency and lookup speed.

**Generic implementation:** because it allows the same hashMap to store different data types without modifying the underlying code and also works for user-defined data types.

# HashFunction Implementation Plan
# Section 1 — Public API
- `generate(int key)`  Generates a hash value for an integer.

- `generate(char key)`  Generates a hash value for a character.

- `generate(bool key)`  Generates a hash value for a boolean value.

- `generate(const std::string& key)`  Generates a hash value for a string using a polynomial rolling hash.

- `generate(const T& key)`  Generates a hash value for any user-defined type.

- `generateFallback(const T& key)`  Computes a hash by processing the raw bytes of an object when no custom `hashCode()` function is available.

## Why this decomposition is better

Different data types require different hashing strategies.

Primitive data types such as `int`, `char`, and `bool` already represent small numeric values, so their hash values can be generated directly with almost no computation.

Strings require a dedicated hashing algorithm because every character contributes to the final hash value.

For user-defined classes, the implementation first checks at compile time whether the class provides a `hashCode()` member function using the `HasHashCode` helper structure.
- If a `hashCode()` function exists, it is used because the class designer knows which members uniquely identify the object.
- Otherwise, the implementation automatically falls back to a generic byte-wise hashing algorithm.
This design provides both flexibility and ease of use while keeping the `HashFunction` completely generic.

# Section 2 — Internal Representation
It is implemented as a **stateless utility class** whose only responsibility is to convert different key types into integer hash values.
The implementation consists of multiple overloaded `generate()` functions.
## Primitive Types
For primitive data types (`int`, `char`, and `bool`), the corresponding numeric value itself is returned as the hash value.
### String Hashing
For `std::string`, the implementation uses a **Polynomial Rolling Hash**.

Starting with an initial hash value of zero, each character is processed using the formula:

```text
hash = hash * 31 + currentCharacter
```
Multiplying by **31** before adding the next character ensures that both the character values and their positions influence the final hash, resulting in a much better distribution than simply summing the ASCII values.
## User-Defined Types
For generic user-defined types, the implementation relies on the helper template `HasHashCode`.
This template uses compile-time type inspection to determine whether the class defines a `hashCode()` member function.

- If the class provides `hashCode()`, the returned value becomes the object's hash.
- If no `hashCode()` function exists, `generateFallback()` is automatically invoked.

### Fallback Hashing

The fallback algorithm converts the object's address into a sequence of bytes using

```cpp
reinterpret_cast<const unsigned char*>
```

and processes every byte using the same polynomial accumulation strategy.

This allows the `HashMap` to support user-defined types even when no custom hashing function has been implemented.

Since the class stores no dynamic memory, the compiler-generated constructor, copy constructor, assignment operator, and destructor are sufficient.

---

# Section 3 — Complexity Estimates

## `generate(int)`, `generate(char)`, and `generate(bool)`

| Case | Complexity |
|------|------------|
| Best | **O(1)** |
| Worst | **O(1)** |
| Average | **O(1)** |

**Reason:**  
Each function performs only a single constant-time operation and returns immediately.

---

## `generate(const std::string&)`

| Case | Complexity |
|------|------------|
| Best | **O(1)** |
| Worst | **O(N)** |
| Average | **O(N)** |

**Reason**

- **Best Case:** An empty string requires no iteration.
- **Worst Case:** Every character of the string must be processed exactly once.
- **Average Case:** String hashing always depends on the length of the string.

---

## `generate(const T&)`

| Case | Complexity |
|------|------------|
| Best | **O(1)** |
| Worst | **O(sizeof(T))** |
| Average | **O(1)** |

**Reason**

- **Best Case:** If the class provides a `hashCode()` function, the hash value is obtained by a single function call.
- **Worst Case:** When `hashCode()` is unavailable, the fallback algorithm processes every byte of the object's memory.
- **Average Case:** Most user-defined objects either implement `hashCode()` or are relatively small, making hash generation very fast.

---

## `generateFallback(const T&)`

| Case | Complexity |
|------|------------|
| Best | **O(sizeof(T))** |
| Worst | **O(sizeof(T))** |
| Average | **O(sizeof(T))** |

**Reason:**  
The algorithm examines every byte of the object's memory exactly once.

---

# Section 4 — Design Decisions

## overload `generate()` for primitive types

Primitive types already represent numeric values.

Returning those values directly avoids unnecessary computations and produces constant-time hash generation.

---

## use Polynomial Rolling Hash for strings

The polynomial rolling technique

```text
hash = hash * 31 + currentCharacter
```
produces a much better distribution than simply adding character values together.
The multiplication ensures that both the characters and their positions influence the final hash value, significantly reducing collisions for similar strings.

---

## detect `hashCode()` at compile time

The helper template `HasHashCode` uses  (Substitution Failure Is Not An Error)** to determine whether a user-defined class implements a `hashCode()` member function.

- If `hashCode()` exists, the compiler automatically selects that implementation.
- If it does not exist, the compiler silently switches to `generateFallback()` without producing a compilation error.

This allows the `HashFunction` to support both kinds of user-defined classes while requiring no additional work from the `HashMap` implementation.

---

## Why provide `generateFallback()`?

Not every user-defined class will implement its own hashing function.

Rejecting such classes would reduce the generic nature of the `HashMap`.

Instead, the fallback algorithm hashes the raw bytes of the object so that any trivially copyable user-defined type can still be used as a key.

---

## Why use `reinterpret_cast<const unsigned char*>`?

Objects are stored in memory as a sequence of bytes.

Converting the object to an `unsigned char*` allows the algorithm to examine each byte individually without modifying the original object.

Using `unsigned char` guarantees that every byte is treated as an unsigned value between **0 and 255**, ensuring consistent hash computation across different platforms.


## Generic Implementation
The `HashFunction` is implemented using:

- Function overloading
- Templates
- Compile-time type inspection (SFINAE)
This enables the same hashing interface to support primitive types, strings, and user-defined classes without requiring changes to the `HashMap` implementation.
The result is a reusable, extensible, and fully generic hashing mechanism suitable for a generic data structure library.
