# Daily Design Journal · daily_30_journal.md

## Section 1 — Specific Bug

**Session 1 (DynamicArray Tests):** Found a real memory bug in `DynamicArray::remove()`. When you remove an element from the middle or end of the array, the code shifts all elements to the left to fill the gap. But after the shift, the last slot in the array still holds a live object — the code was just hiding it by decrementing `size`, never calling its destructor:

```cpp
// BUGGY — last slot's destructor never called
void DynamicArray<T>::remove(int idx) {
    for (int i = idx; i < size - 1; i++) {
        new(&arr[i]) T(arr[i + 1]);  // shift elements left
    }
    size--;  // ← the object at arr[size] is still alive, leaking
}
```

For `int` this doesn't matter. But for a `std::string` or any type that holds heap memory, this silently leaks. The fix was to explicitly destroy the now-abandoned last slot before decrementing size:

```cpp
// FIXED — destructor called on the vacated last slot
void DynamicArray<T>::remove(int idx) {
    arr[idx].~T();
    for (int i = idx; i < size - 1; i++) {
        new(&arr[i]) T(arr[i + 1]);
        arr[i + 1].~T();
    }
    size--;
}
```

**Session 2 (LinkedList Tests):** Found that `LinkedList` is missing `operator=`. Every C++ class that has a destructor and a copy constructor also needs `operator=` — this is called the Rule of Three. Without it, the compiler generates a shallow assignment that just copies the head pointer, so both lists point to the same nodes. When one destructs, it frees the chain. The other then destructs and tries to free the same memory — that's a double-free crash. `DoublyLinkedList` does define `operator=` correctly, but `LinkedList` does not, and that gap is now documented in the tests.

---

## Section 2 — Failed Attempt

**Session 1:** The very first version of the `DynamicArray` test file used a custom `Tracker` struct that counted how many objects were alive at any moment via a static counter (`liveCount`). This was useful for proving destructor correctness — if a destructor was skipped, `liveCount` stayed higher than expected and the test failed.

The problem was that `Tracker` was complex and unfamiliar. Someone reading the test file has to understand how `static int liveCount`, the copy constructor, and the `TrackerGuard` reset helper all interact before they can understand what a single test is even checking. It made the file harder to read than the actual data structure being tested.

In Session 2, we switched to `Point` — just two integers with `operator==`. Tests that previously checked `Tracker::liveCount` were rewritten to check data values and sizes instead. A crash or wrong value still signals a problem; we just can't pinpoint exactly which destructor call was missed. That's an acceptable trade-off for a much cleaner, more readable test file.

**Session 2:** The original `LinkedListTests.cpp` (written in an earlier session) had tests split by type — separate test groups for `int`, `double`, `std::string`, and `struct` for every single function. So `append()` alone had four test groups with 2-3 tests each. That's 10+ small tests for one function, and most of them were nearly identical. It made the file 529 lines long and redundant.

The rewrite consolidated everything: one test group per function, all types covered inside the same 5-6 tests. The file went from 529 lines to 399 lines — 25% shorter — while actually covering more edge cases because the freed space was used for things like the mutable reference test, the const overload test, and the large dataset test.


#### Test Sections — Session 1 (`DynamicArrayTests.cpp`)
| Section | Function | Tests |
|---|---|---|
| 1 | `append()` | 6 — including resize survival and struct data integrity |
| 2 | `get()` | 6 — including mutable T& return and const overload |
| 3 | `insert()` | 6 — including resize-at-insert correctness |
| 4 | `remove()` | 6 — including destructor call on last element |
| 5 | `getSize()` | 6 — including const reference access |
| 6 | Copy Constructor & `operator=` | 6 — including self-assignment and assign-smaller-over-larger |

#### Test Sections — Session 2 (`LinkedListTests.cpp` + `DoublyLinkedListTests.cpp`)
| Section | Function | LL Tests | DLL Tests | Key DLL-Specific Edge Case |
|---|---|---|---|---|
| 1 | `append()` | 6 | 6 | Tail pointer updates; O(1) vs O(n²) |
| 2 | `get()` | 6 | 6 | Bidirectional traversal midpoint boundary |
| 3 | `insert()` | 6 | 6 | `prev` pointer set on head insert |
| 4 | `remove()` | 6 | 6 | Three branches: head / tail / middle |
| 5 | `getSize()` | 6 | 6 | Drain to zero, const reference |
| 6 | Copy / `operator=` | 6 | 6 | `operator=` missing in LL, defined in DLL |

---

## Section 3 — Learning Reflection

**Session 1** was a good reminder that writing tests is not just about confirming that the code works — it's about finding exactly where and how it breaks. The destructor bug in `remove()` was invisible during normal int-based testing because `int` doesn't have a meaningful destructor. It only showed up when a lifecycle-tracking struct was introduced. That's the whole point of using a custom type in tests: primitives hide resource management bugs.

The other thing that stood out was the `append()` and `insert()` signature mismatches in `dynamicArray.cpp` — the definitions had `T& val` while the header declared `T val`. The code still worked for simple cases (the compiler silently bound the reference), but it was quietly wrong and would have broken on temporaries. Finding it through a failed build was far better than finding it as a subtle runtime bug later.

**Session 2** made the difference between a singly linked list and a doubly linked list feel very concrete. On paper they look similar — both have nodes, both support insert and remove. But writing tests for each revealed how much more state the DLL manages:

- Every `append()` must update both `head` and `tail`.
- Every `insert()` at the head must set the new node's `prev` to null and the old head's `prev` to the new node.
- Every `remove()` has three completely separate code paths — head, tail, and middle — and each one must leave the `prev`/`next` chain consistent for the surrounding nodes.
- `get()` runs two different traversal loops depending on which half of the list the index is in, and the midpoint boundary (`index < size/2`) can easily be off by one.

The singly linked list, by comparison, only has one pointer per node and one traversal direction, which makes it simpler but slower (`append()` is O(n²) because there's no tail pointer) and less complete (no `operator=`). Writing the tests side by side made those trade-offs obvious in a way that just reading the code never would have.
