# Date: 30 june (Session 2)
Duration: 85 minutes

Goal:
Write focused, edge-case-driven test suites for both `LinkedList<T>` (singly linked) and `DoublyLinkedList<T>` using Google Test. The goal was not just to verify the happy path, but to expose real bugs and design limitations in both implementations. Strategy: 5–6 tests per public function, all data types (int, double, std::string, struct) consolidated inside single tests to keep files concise, `[BUG?]` annotations for every edge case that reveals a real limitation, and DLL-specific tests targeting the unique behaviour of `prev/next` pointers and bidirectional traversal.


## LinkedListTests.cpp — Test Sections Written

| Section | Function Tested | Tests | Key Edge Cases |
|---|---|---|---|
| Section 1 | `append()` | 6 | FIFO order, falsy values (0, -1, empty string), cross-type double + Point, struct data survives destructor, large dataset (O(n²) stress) |
| Section 2 | `get()` | 6 | First/last boundary, negative index throws, index==size throws, empty list throws, mutable T& return, const overload |
| Section 3 | `insert()` | 6 | Empty list at index 0, head change, middle re-link, index==size (≡ append), negative + OOB throws, struct at head |
| Section 4 | `remove()` | 6 | Single-element drain, head removal (new head), middle gap close, struct data intact after remove, OOB/empty/negative throws, cross-type string + struct |
| Section 5 | `getSize()` | 6 | Fresh list, incremental appends, draining removes, insert tracking, mixed ops net size, const reference call |
| Section 6 | Copy Constructor | 6 | Deep copy values, independence (mutation isolation), empty list copy, cross-type string + Point, missing `operator=` documented, both destruct safely |
| **Total** | | **36 tests** | |

---

## DoublyLinkedListTests.cpp — Test Sections Written

| Section | Function Tested | Tests | DLL-Specific Edge Cases |
|---|---|---|---|
| Section 1 | `append()` | 6 | Tail pointer updates on every append, O(1) tail correctness verified via backward `get()`, large dataset boundary checks |
| Section 2 | `get()` | 6 | **Bidirectional traversal** (index < size/2 → forward, index ≥ size/2 → backward), midpoint integer-division boundary, mutable T& return, const overload |
| Section 3 | `insert()` | 6 | **prev pointer correctness** on head insert, append-after-insert to verify tail, middle re-link (4 surrounding pointers), index==size delegates to append, struct at middle |
| Section 4 | `remove()` | 6 | **3 distinct branches**: only-element (head==tail→both null), head removal (new head prev=null), tail removal (tail pointer updates, verified by subsequent append), middle close (double remove to stress prev), OOB/empty/negative throws |
| Section 5 | `getSize()` | 6 | Incremental append tracking, drain to zero, insert tracking, mixed ops, const reference call |
| Section 6 | Copy Constructor & `operator=` | 6 | Deep copy independence, empty list copy, **`operator=` IS defined** (unlike SLL), self-assignment guard, assign smaller over larger (old tail freed; subsequent append must attach to correct new tail) |
| **Total** | | **36 tests** | |

---

## Bugs and Limitations Found:

**1. `LinkedList` — No `operator=` (Rule of Three violation)**

`LinkedList<T>` defines a copy constructor and a destructor but has no `operator=`. The compiler-generated assignment operator does a shallow pointer copy, meaning:

```cpp
LinkedList<int> a, b;
a.append(1);
b = a;  // ← UNSAFE: both a and b now share the same node chain
// When b destructs → frees the chain
// When a destructs → double-free on the same memory → undefined behaviour
```

Test `CopyConstructor.AssignmentOperatorMissing` documents this: the `b = a` line is commented out with an explanation, and the test instead uses the copy constructor as a safe proxy to confirm the gap. `DoublyLinkedList<T>` does **not** have this problem — its `operator=` is explicitly defined and tested in `DLL_RuleOfThree.AssignmentOperatorIsDeep`.

**2. `LinkedList::append()` — O(n²) tail traversal**

`LinkedList` has no tail pointer. Every `append()` walks the entire chain from head to find the last node:

```cpp
SNode<T>* current = head;
while (current->next) current = current->next;  // O(n) every time
```

For a list of n elements this makes building the list O(n²) overall. The `Append.LargeDataset` test (500 elements) measured ~31 ms vs `DLL_Append.LargeDataset` (also 500 elements) which completed in ~0 ms because `DoublyLinkedList` appends in O(1) via the `tail` pointer.

**3. `LinkedList::get()` returns `T&` — mutation persists**

The old version of the tests (before this session) treated `get()` as returning by value, which masked an important behaviour: `get()` actually returns `T&`. Test `Get.MutableReferenceWorks` explicitly confirms this:

```cpp
list.append(10);
list.get(0) = 99;           // mutate via reference
EXPECT_EQ(list.get(0), 99); // change persists ✓
```

**4. `DoublyLinkedList::get()` — integer division midpoint boundary**

The bidirectional traversal uses `index < size/2` to decide direction. Because C++ integer division truncates, for an odd-sized list the exact middle index goes to the **backward** path. Test `DLL_Get.MidpointBoundary` explicitly probes this for a 5-element list where `size/2 == 2`:

- Indices 0, 1 → forward from head
- Indices 2, 3, 4 → backward from tail

This is correct behaviour but is a subtle trap: if the condition were `index <= size/2` the midpoint would be traversed forward and backward inconsistently. The test documents and confirms the division boundary.

**5. `DoublyLinkedList::remove()` — tail branch must update `tail` pointer**

Remove at `index == size - 1` uses the dedicated tail branch. If the `tail` pointer is not updated, the next `append()` writes to the freed node, causing heap corruption. Test `DLL_Remove.TailRemovalUpdatesTail` catches this by appending after a tail removal and checking the new element lands at the correct index.

---

## Outcome:

Both test suites compiled and all tests passed on first run:

```
[==========] Running 36 tests from 6 test suites.
[  PASSED  ] 36 tests.   ← LinkedListTests.exe

[==========] Running 36 tests from 6 test suites.
[  PASSED  ] 36 tests.   ← DoublyLinkedListTests.exe
```

No failures, but three `[BUG?]`-annotated tests document real limitations that should be addressed in future iterations:
- `CopyConstructor.AssignmentOperatorMissing` → SLL needs `operator=`
- `Append.StructDataSurvivesDestroy` → explicit `~T()` in destructor is fragile for non-trivial types
- `DLL_Remove.TailRemovalUpdatesTail` → tail pointer must be verified after every tail remove
