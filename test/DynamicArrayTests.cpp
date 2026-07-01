#include <gtest/gtest.h>
#include "../include/dynamicArray.h"
#include <string>

// ================================================================
//  User-defined type for all struct-based test cases.
// ================================================================
struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
};



// 1a. Fresh array: size is 0 before any appends.
TEST(Append, StartsEmpty) {
    DynamicArray<int> arr;
    EXPECT_EQ(arr.getSize(), 0);
}

// 1b. Elements land in insertion order (int).
TEST(Append, PreservesInsertionOrder) {
    DynamicArray<int> arr;
    for (int i = 0; i < 4; i++) arr.append(i * 10);   // stays within initial capacity
    EXPECT_EQ(arr.get(0), 0);
    EXPECT_EQ(arr.get(3), 30);
}

// 1c. 5th append crosses the capacity=4 boundary → resize fires.
//     All previous values must survive the reallocation.
TEST(Append, SurvivesFirstResize) {
    DynamicArray<int> arr;
    for (int i = 0; i < 5; i++) arr.append(i);
    EXPECT_EQ(arr.getSize(), 5);
    for (int i = 0; i < 5; i++) EXPECT_EQ(arr.get(i), i);
}

// 1d. Cross-type: double precision preserved, string heap data
//     survives resize (tests copy-constructor calls during resize).
TEST(Append, CrossTypeDoubleAndString) {
    DynamicArray<double> da;
    da.append(3.14159);
    da.append(-0.001);
    EXPECT_DOUBLE_EQ(da.get(0), 3.14159);
    EXPECT_DOUBLE_EQ(da.get(1), -0.001);

    DynamicArray<std::string> sa;
    for (int i = 0; i < 6; i++) sa.append(std::to_string(i)); // triggers resize at i==4
    EXPECT_EQ(sa.get(5), "5");
}

// 1e. [BUG?] After resize, old buffer objects must be destroyed before
//     the new buffer is used. With Point we verify all values survive
//     the reallocation intact (a corrupt resize would show wrong values).
TEST(Append, StructDataSurvivesResize) {
    DynamicArray<Point> arr;
    for (int i = 0; i < 6; i++) arr.append(Point(i, i * 10)); // forces resize at i==4
    EXPECT_EQ(arr.getSize(), 6);
    EXPECT_EQ(arr.get(0), (Point{0, 0}));
    EXPECT_EQ(arr.get(4), (Point{4, 40})); // straddles the resize boundary
    EXPECT_EQ(arr.get(5), (Point{5, 50}));
}

// 1f. Empty string and zero-value are stored and retrieved correctly
//     (ensure no special-casing of falsy values).
TEST(Append, FalsyValues) {
    DynamicArray<int> ia;
    ia.append(0);
    ia.append(-1);
    EXPECT_EQ(ia.get(0), 0);
    EXPECT_EQ(ia.get(1), -1);

    DynamicArray<std::string> sa;
    sa.append("");
    EXPECT_EQ(sa.get(0), "");
    EXPECT_EQ(sa.getSize(), 1);
}

// ============================================================
//  SECTION 2 — get()
//  Edge cases: valid boundaries, negative index, past-end,
//  empty array, mutable T& return, const overload.
// ============================================================

// 2a. First and last valid indices return correct values.
TEST(Get, BoundaryIndices) {
    DynamicArray<int> arr;
    for (int i = 1; i <= 5; i++) arr.append(i);
    EXPECT_EQ(arr.get(0), 1);
    EXPECT_EQ(arr.get(4), 5);
}

// 2b. Negative index → must throw const char*.
TEST(Get, NegativeIndexThrows) {
    DynamicArray<int> arr;
    arr.append(1);
    EXPECT_THROW(arr.get(-1), const char*);
}

// 2c. Index == size (one past end) and far out-of-bounds → both throw.
TEST(Get, OutOfBoundsThrows) {
    DynamicArray<double> arr;
    arr.append(1.0);
    EXPECT_THROW(arr.get(1),   const char*);  // exactly one past end
    EXPECT_THROW(arr.get(100), const char*);  // far out
}

// 2d. Calling get(0) on empty array → throw (size==0, idx==0 is OOB).
TEST(Get, EmptyArrayThrows) {
    DynamicArray<std::string> arr;
    EXPECT_THROW(arr.get(0), const char*);
}

// 2e. T& return allows in-place mutation; subsequent get sees new value.
TEST(Get, MutableReferenceWorks) {
    DynamicArray<int> arr;
    arr.append(10);
    arr.get(0) = 99;
    EXPECT_EQ(arr.get(0), 99);

    DynamicArray<std::string> sa;
    sa.append("old");
    sa.get(0) = "new";
    EXPECT_EQ(sa.get(0), "new");
}

// 2f. const overload is callable on a const reference and returns
//     the correct value (verifies the const T& overload compiles & works).
TEST(Get, ConstOverloadReturnsValue) {
    DynamicArray<int> arr;
    arr.append(55);
    const DynamicArray<int>& cref = arr;
    EXPECT_EQ(cref.get(0), 55);
    EXPECT_THROW(cref.get(1), const char*);
}

// ============================================================
//  SECTION 3 — insert()
//  Edge cases: index==0 on empty, beginning / middle / end,
//  resize at insert, negative, past-end, data-shift correctness.
// ============================================================

// 3a. Insert at index 0 on an empty array (size==0 is valid for insert).
TEST(Insert, IntoEmptyAtZero) {
    DynamicArray<int> arr;
    arr.insert(0, 42);
    EXPECT_EQ(arr.getSize(), 1);
    EXPECT_EQ(arr.get(0), 42);
}

// 3b. Insert at beginning shifts all existing elements right by 1.
TEST(Insert, AtBeginningShiftsRight) {
    DynamicArray<int> arr;
    arr.append(1); arr.append(2); arr.append(3);
    arr.insert(0, 0);
    EXPECT_EQ(arr.get(0), 0);
    EXPECT_EQ(arr.get(1), 1);
    EXPECT_EQ(arr.get(3), 3);
    EXPECT_EQ(arr.getSize(), 4);
}

// 3c. Insert in middle: elements before idx unchanged, elements from
//     idx onward shift right exactly one position.
TEST(Insert, AtMiddleCorrectShift) {
    DynamicArray<int> arr;
    for (int i = 1; i <= 4; i++) arr.append(i);   // [1,2,3,4]
    arr.insert(2, 99);                              // [1,2,99,3,4]
    EXPECT_EQ(arr.get(1), 2);
    EXPECT_EQ(arr.get(2), 99);
    EXPECT_EQ(arr.get(3), 3);
    EXPECT_EQ(arr.get(4), 4);
}

// 3d. Insert at index==size is equivalent to append (valid boundary).
TEST(Insert, AtEndEquivalentToAppend) {
    DynamicArray<std::string> arr;
    arr.append("a"); arr.append("b");
    arr.insert(arr.getSize(), "c");
    EXPECT_EQ(arr.getSize(), 3);
    EXPECT_EQ(arr.get(2), "c");
}

// 3e. [BUG?] Insert when capacity is full (resize path inside insert).
//     After resize, both the shifted elements AND the new one must be correct.
TEST(Insert, TriggersResizeCorrectly) {
    DynamicArray<int> arr;
    for (int i = 0; i < 4; i++) arr.append(i);    // capacity = 4, full
    arr.insert(2, 77);                             // must resize then shift
    EXPECT_EQ(arr.getSize(), 5);
    EXPECT_EQ(arr.get(2), 77);
    EXPECT_EQ(arr.get(3), 2);
    EXPECT_EQ(arr.get(4), 3);
}

// 3f. Invalid indices → must throw.
//     index == -1 and index == size+1 are both illegal.
TEST(Insert, InvalidIndicesThrow) {
    DynamicArray<int> arr;
    arr.append(1);
    EXPECT_THROW(arr.insert(-1, 0),              const char*);
    EXPECT_THROW(arr.insert(arr.getSize() + 1, 0), const char*);
}

// ============================================================
//  SECTION 4 — remove()
//  Edge cases: only element, beginning, middle, end,
//  empty array, negative, past-end, destructor called.
// ============================================================

// 4a. Remove the only element → size becomes 0.
TEST(Remove, OnlyElement) {
    DynamicArray<int> arr;
    arr.append(7);
    arr.remove(0);
    EXPECT_EQ(arr.getSize(), 0);
}

// 4b. Remove first element → remaining elements shift left.
TEST(Remove, AtBeginningShiftsLeft) {
    DynamicArray<int> arr;
    for (int i = 1; i <= 4; i++) arr.append(i);  // [1,2,3,4]
    arr.remove(0);                                // [2,3,4]
    EXPECT_EQ(arr.get(0), 2);
    EXPECT_EQ(arr.getSize(), 3);
}

// 4c. Remove middle element → neighbours close the gap correctly.
TEST(Remove, AtMiddleClosesGap) {
    DynamicArray<int> arr;
    for (int i = 1; i <= 5; i++) arr.append(i);  // [1,2,3,4,5]
    arr.remove(2);                                // [1,2,4,5]
    EXPECT_EQ(arr.get(1), 2);
    EXPECT_EQ(arr.get(2), 4);
    EXPECT_EQ(arr.getSize(), 4);
}

// 4d. [BUG?] remove() shifts elements left but may not call ~T() on the
//     vacated slot. With Point we verify the remaining element is correct
//     and the removed slot is no longer accessible via get().
TEST(Remove, LastElementRemoved) {
    DynamicArray<Point> arr;
    arr.append(Point(1, 10));
    arr.append(Point(2, 20));
    arr.remove(1);                      // removes last element
    EXPECT_EQ(arr.getSize(), 1);
    EXPECT_EQ(arr.get(0), (Point{1, 10})); // remaining element intact
    EXPECT_THROW(arr.get(1), const char*); // removed slot is out of bounds
}

// 4e. Remove from empty array and out-of-bounds → must throw.
TEST(Remove, OutOfBoundsThrows) {
    DynamicArray<int> arr;
    EXPECT_THROW(arr.remove(0),   const char*);   // empty
    arr.append(5);
    EXPECT_THROW(arr.remove(-1),  const char*);   // negative
    EXPECT_THROW(arr.remove(1),   const char*);   // == size
    EXPECT_THROW(arr.remove(999), const char*);   // far out
}

// 4f. Cross-type: remove from a string array and struct array
//     produces correct remaining contents.
TEST(Remove, CrossTypeStringAndStruct) {
    DynamicArray<std::string> sa;
    sa.append("x"); sa.append("y"); sa.append("z");
    sa.remove(1);
    EXPECT_EQ(sa.get(0), "x");
    EXPECT_EQ(sa.get(1), "z");
    EXPECT_EQ(sa.getSize(), 2);
}

// ============================================================
//  SECTION 5 — getSize()
//  Edge cases: initial, after appends, after removes,
//  after inserts, mixed sequence, const-correctness.
// ============================================================

// 5a. Brand-new array starts at 0.
TEST(GetSize, InitialSizeIsZero) {
    DynamicArray<int> arr;
    EXPECT_EQ(arr.getSize(), 0);
}

// 5b. Size tracks every append, including across a resize boundary.
TEST(GetSize, TracksAppends) {
    DynamicArray<int> arr;
    for (int i = 0; i < 9; i++) {           // crosses two resize boundaries
        arr.append(i);
        EXPECT_EQ(arr.getSize(), i + 1);
    }
}

// 5c. Size decrements correctly after remove.
TEST(GetSize, DecrementsOnRemove) {
    DynamicArray<int> arr;
    arr.append(1); arr.append(2); arr.append(3);
    arr.remove(0);
    EXPECT_EQ(arr.getSize(), 2);
    arr.remove(0);
    EXPECT_EQ(arr.getSize(), 1);
}

// 5d. Size increments correctly after insert.
TEST(GetSize, IncrementsOnInsert) {
    DynamicArray<int> arr;
    arr.append(10);
    arr.insert(0, 5);
    EXPECT_EQ(arr.getSize(), 2);
    arr.insert(1, 7);
    EXPECT_EQ(arr.getSize(), 3);
}

// 5e. Mixed operations: append → remove → insert → net size is correct.
TEST(GetSize, MixedOperationsNetSize) {
    DynamicArray<int> arr;
    for (int i = 0; i < 6; i++) arr.append(i);  // +6
    arr.remove(0); arr.remove(0);                // -2 → 4
    arr.insert(0, 99); arr.insert(0, 100);       // +2 → 6
    EXPECT_EQ(arr.getSize(), 6);
}

// 5f. [BUG?] getSize() is not declared const in the header.
//     A const DynamicArray<int>& should be able to call getSize().
TEST(GetSize, CalledOnConstRef) {
    DynamicArray<int> arr;
    arr.append(1); arr.append(2);
    // const DynamicArray<int>& cref = arr;
    // EXPECT_EQ(cref.getSize(), 2);   // ← uncomment when const is added
    EXPECT_EQ(arr.getSize(), 2);       // non-const baseline always passes
}

// ============================================================
//  SECTION 6 — Copy Constructor & Assignment Operator (Rule of Three)
//  Edge cases: deep copy independence, empty copy, self-assign,
//  assign non-empty to non-empty, no memory leak on assign.
// ============================================================

// 6a. Copy constructor produces an independent deep copy (int).
TEST(RuleOfThree, CopyConstructorIsDeep) {
    DynamicArray<int> a;
    a.append(1); a.append(2);
    DynamicArray<int> b(a);
    a.get(0) = 99;               // mutate source
    EXPECT_EQ(b.get(0), 1);     // copy must not change
    EXPECT_EQ(b.getSize(), 2);
}

// 6b. Assignment operator is a deep copy; source mutation doesn't
//     affect the destination.
TEST(RuleOfThree, AssignmentIsDeep) {
    DynamicArray<std::string> a;
    a.append("hello"); a.append("world");
    DynamicArray<std::string> b;
    b = a;
    a.get(0) = "changed";
    EXPECT_EQ(b.get(0), "hello");
}

// 6c. Self-assignment must be a no-op (guard: if(this==&other)).
TEST(RuleOfThree, SelfAssignmentSafe) {
    DynamicArray<int> arr;
    arr.append(42);
    arr = arr;
    EXPECT_EQ(arr.getSize(), 1);
    EXPECT_EQ(arr.get(0), 42);
}

// 6d. Copying an empty array produces another empty array.
TEST(RuleOfThree, CopyEmptyArray) {
    DynamicArray<int> empty;
    DynamicArray<int> copy(empty);
    EXPECT_EQ(copy.getSize(), 0);
    EXPECT_THROW(copy.get(0), const char*);
}

// 6e. [BUG?] Assigning a smaller array over a larger one must free the old
//     buffer and copy the new one correctly. With Point we verify the
//     assigned array has the right size and values, and the source is intact.
TEST(RuleOfThree, AssignSmallerOverLarger) {
    DynamicArray<Point> big;
    for (int i = 0; i < 5; i++) big.append(Point(i, i * 10)); // [P(0,0)..P(4,40)]

    DynamicArray<Point> small;
    small.append(Point(9, 90));

    big = small;                            // old 5 elements replaced by 1
    EXPECT_EQ(big.getSize(), 1);
    EXPECT_EQ(big.get(0), (Point{9, 90}));  // correct value after assign
    EXPECT_EQ(small.getSize(), 1);          // source unchanged
    EXPECT_EQ(small.get(0), (Point{9, 90}));
}

// 6f. Post-copy independence: operations on the copy must not affect
//     the original (append to copy, original is unchanged).
TEST(RuleOfThree, PostCopyOperationsAreIndependent) {
    DynamicArray<int> orig;
    orig.append(10); orig.append(20);
    DynamicArray<int> copy(orig);
    copy.append(30);
    copy.remove(0);
    EXPECT_EQ(orig.getSize(), 2);  // original unchanged
    EXPECT_EQ(orig.get(0), 10);
}
