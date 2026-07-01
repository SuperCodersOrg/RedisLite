# Date: June 30 (Session 1)
Duration: 90 minutes

Goal:
Write a focused, non-redundant test suite for `DynamicArray<T>` using Google Test. The goal was not just to confirm the happy path, but to actively find real bugs in the implementation. The strategy was: 5–6 tests per public function, cross-type coverage (int, double, std::string, struct) consolidated inside single tests to keep the file small, and targeted edge-case tests that would expose hidden bugs — particularly around memory management and destructor correctness.

## Test Sections Written:

| Section | Function Tested | Tests Written |
|---|---|---|
| Section 1 | `append()` | 6 |
| Section 2 | `get()` | 6 |
| Section 3 | `insert()` | 6 |
| Section 4 | `remove()` | 6 |
| Section 5 | `getSize()` | 6 |
| Section 6 | Copy Constructor & Assignment Operator (Rule of Three) | 6 |
| **Total** | | **36 tests** |

A custom `Tracker` struct was introduced to count live object instances using a static counter (`liveCount`). This allowed tests to verify that destructors were being called at the right time — not just that values were correct.

Also updated `CMakeLists.txt` to split the single `RunTests` target into three separate executables: `DynamicArrayTests.exe`, `LinkedListTests.exe`, and `HashMapTests.exe`, so each data structure can be built and run independently.

## Problem Encountered:

**1 test failed: `Remove.LastElementDestructorCalled`**

**First run result: 35 passed, 1 failed.**

```
[  FAILED  ] Remove.LastElementDestructorCalled
```

The test appended two `Tracker` objects, then called `remove(1)` (the last element). It then checked that `Tracker::liveCount == 1`, meaning the removed element's destructor must have been called. The check failed — `liveCount` was still `2`.

Root cause was found in `remove()` inside `dynamicArray.cpp`:

```cpp
// BEFORE (buggy)
void DynamicArray<T>::remove(int idx) {
    if(idx < 0 || idx >= size) throw "Index out of bounds";
    for(int i = idx; i < size-1; i++) {
        new(&arr[i]) T(arr[i+1]);   // shift left
    }
    size--;                         // ← destructor of last slot NEVER called
}
```

After the loop shifts elements left, `arr[size-1]` still holds a live object in raw memory. The code just hides it by decrementing `size`, leaking the destructor call. For primitive types like `int` this is harmless, but for any type that owns resources (heap strings, file handles, reference counts), this silently leaks memory.

## What We Fixed:

Added an explicit destructor call on the last slot before decrementing `size`:

```cpp
// AFTER (fixed)
void DynamicArray<T>::remove(int idx) {
    if(idx < 0 || idx >= size) throw "Index out of bounds";
    for(int i = idx; i < size-1; i++) {
        new(&arr[i]) T(arr[i+1]);
    }
    arr[size - 1].~T();   // ← explicitly destroy the now-abandoned last slot
    size--;
}
```

## Outcome:

After the fix, re-ran `DynamicArrayTests.exe`:

```
[==========] Running 36 tests from 6 test suites.
[  PASSED  ] 36 tests.
```

All 36 tests passed. 
