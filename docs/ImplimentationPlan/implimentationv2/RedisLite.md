# RedisLite Implementation Plan
# Section 1 — Public API
## Proposed Methods
- `set(const std::string& key, const std::string& value)`
  - Inserts a new key-value pair into the database or updates the value if the key already exists.
- `get(const std::string& key) const`
  - Returns the value associated with the given key.
- `del(const std::string& key)`
  - Removes the specified key and its associated value from the database.
- `exist(const std::string& key) const`
  - Checks whether a given key exists.
- `clear()`
  - Removes all key-value pairs from the database.
- `size() const`
  - Returns the total number of stored key-value pairs.
- `empty() const`
  - Returns whether the database currently contains any elements.

## Why this decomposition is better

RedisLite acts as a lightweight wrapper around the generic `HashMap` implementation.

Instead of exposing the complete HashMap interface, it provides only the operations required by a basic Redis-like key-value store. This keeps the public interface simple while allowing all storage responsibilities to remain inside the underlying HashMap.

Each public function directly delegates its work to the corresponding HashMap operation, resulting in a clean separation between command processing and data storage.

# Section 2 — Internal Representation
RedisLite stores all data inside a single data member:

```cpp
HashMap<std::string, std::string> store;
```

The HashMap manages all memory allocation, collision handling(internally by linked list), insertion, deletion, searching, and resizing.

RedisLite itself does not implement any storage algorithm. Instead, every public member function forwards the request to the underlying HashMap.
### `set()`
Calls

```cpp
store.set(key, value);
```
to insert a new key or update an existing value.

### `get()`

First checks
```cpp
store.exist(key)
```

If the key exists,

```cpp
store.get(key)
```

returns the associated value.

Otherwise,

```cpp
"key doesn't exist"
```

is returned.

### `del()`

First verifies that the key exists using

```cpp
store.exist(key)
```

If found,

```cpp
store.remove(key)
```

is executed.

### `exist()`

Directly forwards the request to

```cpp
store.exist(key);
```

### `clear()`

Removes every key-value pair by calling

```cpp
store.clear();
```

### `size()`

Returns

```cpp
store.getSize();
```

### `empty()`

Returns

```cpp
store.empty();
```

RedisLite maintains no additional data members beyond the HashMap, so the compiler-generated copy constructor, assignment operator, and destructor are sufficient.

---

# Section 3 — Complexity Estimates

The following complexities depend on the underlying HashMap implementation.

## `set(const std::string&, const std::string&)`

| Case | Complexity |
|------|------------|
| Best | **O(1)** |
| Average | **O(1)** |
| Worst | **O(N)** |

**Reason**

- Best/Average: Direct bucket insertion.
- Worst: All keys collide or table resizing occurs.

---

## `get(const std::string&)`

| Case | Complexity |
|------|------------|
| Best | **O(1)** |
| Average | **O(1)** |
| Worst | **O(N)** |

**Reason**

- Best/Average: Desired key is found immediately.
- Worst: Entire collision chain must be searched.

---

## `del(const std::string&)`

| Case | Complexity |
|------|------------|
| Best | **O(1)** |
| Average | **O(1)** |
| Worst | **O(N)** |

**Reason**

Deletion first searches for the key and then removes it from the collision chain.

---

## `exist(const std::string&)`

| Case | Complexity |
|------|------------|
| Best | **O(1)** |
| Average | **O(1)** |
| Worst | **O(N)** |

**Reason**
Searching depends on the collision chain length.
---

## `clear()`

| Case | Complexity |
|------|------------|
| Best | **O(1)** |
| Average | **O(N)** |
| Worst | **O(N)** |

**Reason**

Every stored node must be deleted.
---

## `size()`

| Case | Complexity |
|------|------------|
| Best | **O(1)** |
| Average | **O(1)** |
| Worst | **O(1)** |

**Reason**

The HashMap maintains the current number of stored elements.

---

## `empty()`

| Case | Complexity |
|------|------------|
| Best | **O(1)** |
| Average | **O(1)** |
| Worst | **O(1)** |

**Reason**

Only the current element count is checked.

---

# Section 4 — Design Decisions

##  use HashMap internally
A hash table provides expected constant-time insertion, searching, and deletion, making it well suited for implementing a Redis-style key-value database.

## separate RedisLite from HashMap
HashMap is a generic data structure that can store any key-value type.
RedisLite specializes it into a string-based key-value database while hiding implementation details from the user.

## check existence before get() and del()

Calling `exist()` prevents unnecessary retrieval or deletion attempts on missing keys.

This allows RedisLite to return an appropriate message when a key is not present.

## Why use string keys and values?

Redis stores keys and values as strings.

Restricting RedisLite to

```cpp
HashMap<std::string, std::string>
```

makes the interface closely resemble the behavior of the original Redis database.

## Generic Storage

RedisLite performs no storage management itself.

All collision handling, hashing, dynamic resizing, memory management, and bucket organization are delegated to the generic HashMap implementation.

This separation improves maintainability and allows future changes to the HashMap without modifying the RedisLite interface.

# CommandParser Implementation Plan

---

# Section 1 — Public API

## Proposed Methods

- `startREPL()`
  - Starts an interactive Read-Eval-Print Loop (REPL) that continuously accepts user commands until the user exits.

- `parseAndExecute(const std::string& input)`
  - Parses a command entered by the user and executes the corresponding RedisLite operation.

- `trim(std::string& s)` *(Private)*
  - Removes leading and trailing whitespace from the input string.

---

## Why this decomposition is better

The parser separates user interaction from database operations.

- `startREPL()` manages console interaction.
- `parseAndExecute()` interprets commands.
- `trim()` ensures clean input before parsing.

This separation keeps each function focused on a single responsibility and simplifies maintenance.

---

# Section 2 — Internal Representation

The parser maintains one private member:

```cpp
RedisLite redis;
```

which represents the in-memory key-value database.

## `trim()`

Manually removes leading and trailing spaces, tabs, carriage returns, and newline characters.

No standard library trimming functions are used.

---

## `startREPL()`

Implements a continuous loop that:

1. Displays a prompt.
2. Reads one complete line.
3. Trims whitespace.
4. Detects EXIT or QUIT commands.
5. Calls `parseAndExecute()` for all remaining commands.

The loop terminates only when EXIT or QUIT is entered.

---

## `parseAndExecute()`

The parser manually scans the input string character by character.

It extracts

- command
- key
- value

without using string stream utilities.

The command is converted to uppercase manually to enable case-insensitive command recognition.

Supported commands are

- SET
- GET
- DEL
- EXIST
- EXISTS
- CLEAR
- SIZE
- EMPTY
- EXIT
- QUIT

After parsing, the appropriate RedisLite function is called.

If the command is invalid or required arguments are missing, an appropriate error message is displayed.

---

# Section 3 — Complexity Estimates

Let **L** denote the length of the input command.

## `trim()`

| Case | Complexity |
|------|------------|
| Best | **O(1)** |
| Average | **O(L)** |
| Worst | **O(L)** |

Reason:

Leading and trailing whitespace may require scanning the string.

---

## `parseAndExecute()`

| Case | Complexity |
|------|------------|
| Best | **O(L)** |
| Average | **O(L)** + Redis operation |
| Worst | **O(L + N)** |

Reason:

- Input parsing requires one linear scan.
- Redis operations depend on HashMap complexity.
- Worst case occurs when HashMap operations degrade because of collisions.

---

## `startREPL()`

| Case | Complexity |
|------|------------|
| Best | **O(L)** per command |
| Average | **O(L)** + Redis operation |
| Worst | **O(L + N)** per command |

Reason:

Each iteration performs trimming, parsing, and one Redis operation.

Since the loop continues until EXIT is entered, the total running time depends on the number of commands processed.

---

# Section 4 — Design Decisions

##  perform manual parsing
The implementation avoids using `stringstream` and instead parses the input character by character.

This provides greater control over parsing and demonstrates string-processing logic.


##  convert commands to uppercase
Users can enter commands in either uppercase or lowercase.

Converting commands to uppercase ensures case-insensitive command recognition.

## keep parsing separate from RedisLite
RedisLite is responsible only for data storage.

CommandParser handles user interaction, input validation, and command interpretation.

Separating these responsibilities improves modularity and maintainability.

## implement a REPL

The Read-Eval-Print Loop provides an interactive interface similar to the original Redis command-line client.

Each command is processed immediately, allowing continuous interaction until the user explicitly exits.

## Error Handling
Before executing any Redis operation, the parser validates the number of supplied arguments.

Invalid commands or missing arguments produce descriptive error messages without terminating the application, improving usability.