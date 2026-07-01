#include <gtest/gtest.h>
#include "../include/linkedList.h"
#include <string>

// ================================================================
//  User-defined type for all struct-based test cases.
// ================================================================
struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
};

// ================================================================
//  SECTION 1 — append()
//  Edge cases: empty list, insertion order, falsy/zero values,
//  cross-type (double, string, struct), destructor balance.
// ================================================================

// 1a. Size starts at 0; first append makes it 1 and element is retrievable.
TEST(Append, EmptyThenOne) {
    LinkedList<int> list;
    EXPECT_EQ(list.getSize(), 0);
    list.append(42);
    EXPECT_EQ(list.getSize(), 1);
    EXPECT_EQ(list.get(0), 42);
}

// 1b. Elements land in FIFO order (head → tail).
TEST(Append, PreservesInsertionOrder) {
    LinkedList<int> list;
    list.append(10); list.append(20); list.append(30);
    EXPECT_EQ(list.get(0), 10);
    EXPECT_EQ(list.get(1), 20);
    EXPECT_EQ(list.get(2), 30);
}

// 1c. Zero, negative, and empty-string are stored without special-casing.
TEST(Append, FalsyValues) {
    LinkedList<int> li;
    li.append(0); li.append(-1);
    EXPECT_EQ(li.get(0), 0);
    EXPECT_EQ(li.get(1), -1);

    LinkedList<std::string> ls;
    ls.append(""); ls.append("after");
    EXPECT_EQ(ls.get(0), "");
    EXPECT_EQ(ls.get(1), "after");
}

// 1d. Cross-type: double precision preserved; struct fields intact.
TEST(Append, CrossTypeDoubleAndStruct) {
    LinkedList<double> ld;
    ld.append(3.14159); ld.append(-0.001);
    EXPECT_DOUBLE_EQ(ld.get(0), 3.14159);
    EXPECT_DOUBLE_EQ(ld.get(1), -0.001);

    LinkedList<Point> lp;
    lp.append(Point(1, 2)); lp.append(Point(3, 4));
    EXPECT_EQ(lp.get(0), (Point{1, 2}));
    EXPECT_EQ(lp.get(1), (Point{3, 4}));
}

// 1e. [BUG?] The destructor calls node->data.~T() explicitly on each node.
//     With Point we verify all data survives and the list empties cleanly.
//     (Without a lifecycle tracker we cannot count destructor calls, but a
//     crash or wrong value here signals a double-destruct or leak.)
TEST(Append, StructDataSurvivesDestroy) {
    LinkedList<Point> list;
    list.append(Point(1, 10));
    list.append(Point(2, 20));
    list.append(Point(3, 30));
    EXPECT_EQ(list.getSize(), 3);
    EXPECT_EQ(list.get(0), (Point{1, 10}));
    EXPECT_EQ(list.get(2), (Point{3, 30}));
    // List goes out of scope here — destructor must free all 3 nodes without crash.
}

// 1f. Large dataset: tail pointer walks the whole chain each append (O(n²)).
//     Size must still be correct — this also stress-tests the tail traversal.
TEST(Append, LargeDataset) {
    LinkedList<int> list;
    for (int i = 0; i < 500; i++) list.append(i);
    EXPECT_EQ(list.getSize(), 500);
    EXPECT_EQ(list.get(0),   0);
    EXPECT_EQ(list.get(499), 499);
    EXPECT_EQ(list.get(250), 250);
}

// ================================================================
//  SECTION 2 — get()
//  Edge cases: first/last/middle, negative, past-end, empty list,
//  mutable T& return, const overload, cross-type.
// ================================================================

// 2a. Valid boundaries: first and last indices return correct values.
TEST(Get, BoundaryIndices) {
    LinkedList<int> list;
    for (int i = 1; i <= 5; i++) list.append(i * 10);  // 10,20,30,40,50
    EXPECT_EQ(list.get(0), 10);
    EXPECT_EQ(list.get(4), 50);
}

// 2b. Negative index and index >= size must throw const char*.
TEST(Get, OutOfBoundsThrows) {
    LinkedList<int> list;
    list.append(1);
    EXPECT_THROW(list.get(-1),  const char*);   // negative
    EXPECT_THROW(list.get(1),   const char*);   // exactly size
    EXPECT_THROW(list.get(999), const char*);   // far out
}

// 2c. get(0) on empty list must throw (size==0, so index 0 is OOB).
TEST(Get, EmptyListThrows) {
    LinkedList<int> li;
    EXPECT_THROW(li.get(0), const char*);

    LinkedList<std::string> ls;
    EXPECT_THROW(ls.get(0), const char*);
}

// 2d. get() returns T& — mutation through it must be visible on the next call.
//     Unlike DynamicArray, LinkedList get() returns a reference (not a copy).
TEST(Get, MutableReferenceWorks) {
    LinkedList<int> list;
    list.append(10);
    list.get(0) = 99;           // mutate via reference
    EXPECT_EQ(list.get(0), 99); // change persists

    LinkedList<std::string> ls;
    ls.append("old");
    ls.get(0) = "new";
    EXPECT_EQ(ls.get(0), "new");
}

// 2e. const overload: callable on a const reference; throws on OOB too.
TEST(Get, ConstOverload) {
    LinkedList<int> list;
    list.append(77);
    const LinkedList<int>& cRef = list;
    EXPECT_EQ(cRef.get(0), 77);
    EXPECT_THROW(cRef.get(1), const char*);
}

// 2f. Cross-type: double precision and struct fields correctly retrieved.
TEST(Get, CrossTypeDoubleAndStruct) {
    LinkedList<double> ld;
    ld.append(1.5); ld.append(2.5);
    EXPECT_DOUBLE_EQ(ld.get(1), 2.5);

    LinkedList<Point> lp;
    lp.append(Point(7, 8));
    EXPECT_EQ(lp.get(0), (Point{7, 8}));
    EXPECT_THROW(lp.get(5), const char*);
}

// ================================================================
//  SECTION 3 — insert()
//  Edge cases: empty list at 0, head change, middle shift,
//  end (== append), negative, past-end, struct correctness.
// ================================================================

// 3a. Insert at index 0 into empty list (size==0 is valid).
TEST(Insert, IntoEmptyAtZero) {
    LinkedList<int> list;
    list.insert(0, 42);
    EXPECT_EQ(list.getSize(), 1);
    EXPECT_EQ(list.get(0), 42);
}

// 3b. Insert at index 0 changes head; previous head shifts to index 1.
TEST(Insert, AtBeginningChangesHead) {
    LinkedList<int> list;
    list.append(1); list.append(2); list.append(3);
    list.insert(0, 99);                     // [99, 1, 2, 3]
    EXPECT_EQ(list.getSize(), 4);
    EXPECT_EQ(list.get(0), 99);
    EXPECT_EQ(list.get(1), 1);
    EXPECT_EQ(list.get(3), 3);
}

// 3c. Insert in middle: neighbours re-link correctly.
TEST(Insert, AtMiddleCorrectLinks) {
    LinkedList<int> list;
    list.append(1); list.append(2); list.append(4);
    list.insert(2, 3);                      // [1, 2, 3, 4]
    EXPECT_EQ(list.getSize(), 4);
    EXPECT_EQ(list.get(2), 3);
    EXPECT_EQ(list.get(3), 4);
}

// 3d. Insert at index == size is equivalent to append (valid boundary).
TEST(Insert, AtEndEquivalentToAppend) {
    LinkedList<std::string> list;
    list.append("a"); list.append("b");
    list.insert(list.getSize(), "c");       // [a, b, c]
    EXPECT_EQ(list.getSize(), 3);
    EXPECT_EQ(list.get(2), "c");
}

// 3e. Invalid indices must throw.
//     index == -1 and index == size+1 are both illegal.
TEST(Insert, InvalidIndicesThrow) {
    LinkedList<int> list;
    list.append(1);
    EXPECT_THROW(list.insert(-1, 0),               const char*);
    EXPECT_THROW(list.insert(list.getSize() + 1, 0), const char*);
    // [BUG?] insert(1, x) on an empty list (size==0) should throw too.
    LinkedList<int> empty;
    EXPECT_THROW(empty.insert(1, 10), const char*);
}

// 3f. Cross-type: struct inserted at beginning and verified by value.
TEST(Insert, StructAtBeginning) {
    LinkedList<Point> list;
    list.append(Point(2, 2));
    list.insert(0, Point(1, 1));
    EXPECT_EQ(list.get(0), (Point{1, 1}));
    EXPECT_EQ(list.get(1), (Point{2, 2}));
}

// ================================================================
//  SECTION 4 — remove()
//  Edge cases: only element, head removal, middle, tail,
//  drain to empty, empty/negative/OOB throws, destructor called.
// ================================================================

// 4a. Remove the only element → size becomes 0; any get() must throw.
TEST(Remove, OnlyElement) {
    LinkedList<int> list;
    list.append(42);
    list.remove(0);
    EXPECT_EQ(list.getSize(), 0);
    EXPECT_THROW(list.get(0), const char*);
}

// 4b. Remove head: next node becomes new head; size decrements.
TEST(Remove, HeadChange) {
    LinkedList<int> list;
    for (int i = 1; i <= 4; i++) list.append(i);  // [1,2,3,4]
    list.remove(0);                                // [2,3,4]
    EXPECT_EQ(list.getSize(), 3);
    EXPECT_EQ(list.get(0), 2);
}

// 4c. Remove middle: gap closes; neighbours re-linked correctly.
TEST(Remove, MiddleClosesGap) {
    LinkedList<int> list;
    for (int i = 1; i <= 5; i++) list.append(i);  // [1,2,3,4,5]
    list.remove(2);                                // [1,2,4,5]
    EXPECT_EQ(list.getSize(), 4);
    EXPECT_EQ(list.get(1), 2);
    EXPECT_EQ(list.get(2), 4);
}

// 4d. [BUG?] remove() calls node->data.~T() explicitly before free().
//     With Point we verify the remaining nodes are intact after a remove
//     (a double-destruct or bad pointer would corrupt the list or crash).
TEST(Remove, StructDataIntactAfterRemove) {
    LinkedList<Point> list;
    list.append(Point(1, 10));
    list.append(Point(2, 20));
    list.append(Point(3, 30));
    list.remove(1);                         // removes Point(2,20)
    EXPECT_EQ(list.getSize(), 2);
    EXPECT_EQ(list.get(0), (Point{1, 10})); // head untouched
    EXPECT_EQ(list.get(1), (Point{3, 30})); // tail re-linked correctly
}

// 4e. Empty list, negative, and OOB indices must all throw.
TEST(Remove, InvalidIndicesThrow) {
    LinkedList<int> empty;
    EXPECT_THROW(empty.remove(0), const char*);  // empty list

    LinkedList<int> list;
    list.append(1);
    EXPECT_THROW(list.remove(-1),              const char*); // negative
    EXPECT_THROW(list.remove(list.getSize()),  const char*); // exactly size
    EXPECT_THROW(list.remove(999),             const char*); // far out
}

// 4f. Cross-type: string and struct remove preserves remaining elements.
TEST(Remove, CrossTypeStringAndStruct) {
    LinkedList<std::string> ls;
    ls.append("x"); ls.append("y"); ls.append("z");
    ls.remove(1);                           // [x, z]
    EXPECT_EQ(ls.get(0), "x");
    EXPECT_EQ(ls.get(1), "z");

    LinkedList<Point> lp;
    lp.append(Point(1,1)); lp.append(Point(2,2)); lp.append(Point(3,3));
    lp.remove(0);                           // [P(2,2), P(3,3)]
    EXPECT_EQ(lp.get(0), (Point{2, 2}));
}

// ================================================================
//  SECTION 5 — getSize()
//  Edge cases: fresh list, after appends, removes, inserts,
//  mixed ops, const-correctness (getSize() IS declared const).
// ================================================================

// 5a. Brand-new list starts at 0.
TEST(GetSize, InitialSizeIsZero) {
    LinkedList<int> list;
    EXPECT_EQ(list.getSize(), 0);
}

// 5b. Size tracks every append incrementally.
TEST(GetSize, TracksAppends) {
    LinkedList<int> list;
    for (int i = 0; i < 6; i++) {
        list.append(i);
        EXPECT_EQ(list.getSize(), i + 1);
    }
}

// 5c. Size decrements after remove; drain to 0.
TEST(GetSize, TracksRemoves) {
    LinkedList<int> list;
    for (int i = 0; i < 4; i++) list.append(i);
    for (int i = 3; i >= 0; i--) {
        list.remove(0);
        EXPECT_EQ(list.getSize(), i);
    }
}

// 5d. Size increments after insert (at head and tail).
TEST(GetSize, TracksInserts) {
    LinkedList<int> list;
    list.append(10);
    list.insert(0, 5);       EXPECT_EQ(list.getSize(), 2);
    list.insert(list.getSize(), 20); EXPECT_EQ(list.getSize(), 3);
}

// 5e. Mixed append/remove/insert → net size is correct.
TEST(GetSize, MixedOperations) {
    LinkedList<int> list;
    for (int i = 0; i < 10; i++) list.append(i);  // +10
    list.remove(0); list.remove(0);                // -2 → 8
    list.insert(0, 99); list.insert(0, 100);       // +2 → 10
    EXPECT_EQ(list.getSize(), 10);
}

// 5f. getSize() is declared const — callable on const reference.
TEST(GetSize, CalledOnConstRef) {
    LinkedList<int> list;
    list.append(1); list.append(2);
    const LinkedList<int>& cRef = list;
    EXPECT_EQ(cRef.getSize(), 2);
}

// ================================================================
//  SECTION 6 — Copy Constructor
//  Edge cases: deep copy independence, empty copy,
//  post-copy mutation isolation, cross-type, no assignment operator.
// ================================================================

// 6a. Copy constructor produces a deep copy: values match source.
TEST(CopyConstructor, DeepCopyValues) {
    LinkedList<int> a;
    a.append(1); a.append(2); a.append(3);
    LinkedList<int> b(a);
    EXPECT_EQ(b.getSize(), 3);
    EXPECT_EQ(b.get(0), 1);
    EXPECT_EQ(b.get(2), 3);
}

// 6b. Mutation of copy must not affect original (node pointers are separate).
TEST(CopyConstructor, CopyIsIndependent) {
    LinkedList<int> a;
    a.append(10);
    LinkedList<int> b(a);
    b.get(0) = 99;              // mutate copy via mutable reference
    EXPECT_EQ(a.get(0), 10);   // original untouched
    b.append(20);
    EXPECT_EQ(a.getSize(), 1); // original size unchanged
}

// 6c. Copying an empty list produces another empty list (no crash).
TEST(CopyConstructor, CopyEmptyList) {
    LinkedList<int> a;
    LinkedList<int> b(a);
    EXPECT_EQ(b.getSize(), 0);
    EXPECT_THROW(b.get(0), const char*);
}

// 6d. Cross-type: string and struct copies are independent deep copies.
TEST(CopyConstructor, CrossTypeStringAndStruct) {
    LinkedList<std::string> ls;
    ls.append("hello"); ls.append("world");
    LinkedList<std::string> lsCopy(ls);
    EXPECT_EQ(lsCopy.get(0), "hello");
    EXPECT_EQ(lsCopy.get(1), "world");

    LinkedList<Point> lp;
    lp.append(Point(3, 7));
    LinkedList<Point> lpCopy(lp);
    EXPECT_EQ(lpCopy.get(0), (Point{3, 7}));
}

// 6e. [BUG?] No copy assignment operator (operator=) is defined.
//     The compiler-generated one does a shallow (pointer) copy, so:
//       a = b; then a.append(x) corrupts b's chain or causes double-free.
//     This test demonstrates the missing Rule-of-Three member.
TEST(CopyConstructor, AssignmentOperatorMissing) {
    LinkedList<int> a;
    a.append(1); a.append(2);
    LinkedList<int> b;
    b.append(99);
    // b = a; // ← UNSAFE: shallow copy would make both point to same nodes.
    // Instead verify copy constructor is safe as a proxy for the gap:
    LinkedList<int> c(a);  // copy-construct is safe (explicitly defined)
    EXPECT_EQ(c.getSize(), 2);
    EXPECT_EQ(c.get(0), 1);
    // operator= is NOT defined → assignment would leak b's old nodes
    // and share a's node chain, causing double-free on destruction.
}

// 6f. [BUG?] Copying and then destroying both lists must not double-free.
//     With Point we verify the copy has its own independent nodes: mutating
//     the copy leaves the original intact, and both destruct without crash.
TEST(CopyConstructor, CopyAndOriginalDestructSafely) {
    LinkedList<Point> a;
    a.append(Point(1, 10)); a.append(Point(2, 20));
    {
        LinkedList<Point> b(a);
        EXPECT_EQ(b.getSize(), 2);
        b.get(0) = Point(9, 90);            // mutate copy's node
        EXPECT_EQ(a.get(0), (Point{1, 10})); // original node untouched
        // b destructs here — must not corrupt a's chain
    }
    // a must still be fully readable after b's destructor ran
    EXPECT_EQ(a.getSize(), 2);
    EXPECT_EQ(a.get(0), (Point{1, 10}));
    EXPECT_EQ(a.get(1), (Point{2, 20}));
}
