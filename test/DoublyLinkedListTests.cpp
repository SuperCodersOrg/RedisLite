#include <gtest/gtest.h>
#include "../include/doublyLinkedList.h"
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
//  DLL-specific: tail pointer must update on every append.
//  Edge cases: empty list, insertion order, falsy values,
//  O(1) tail append correctness, cross-type, struct fields.
// ================================================================

// 1a. Fresh list is empty; first append makes it size==1 and readable.
TEST(DLL_Append, EmptyThenOne) {
    DoublyLinkedList<int> list;
    EXPECT_EQ(list.getSize(), 0);
    list.append(42);
    EXPECT_EQ(list.getSize(), 1);
    EXPECT_EQ(list.get(0), 42);
}

// 1b. Elements land in FIFO order; both head and tail are correct.
TEST(DLL_Append, PreservesInsertionOrder) {
    DoublyLinkedList<int> list;
    list.append(10); list.append(20); list.append(30);
    EXPECT_EQ(list.get(0), 10);
    EXPECT_EQ(list.get(1), 20);
    EXPECT_EQ(list.get(2), 30);
}

// 1c. Zero, negative, and empty-string are stored without special-casing.
TEST(DLL_Append, FalsyValues) {
    DoublyLinkedList<int> li;
    li.append(0); li.append(-1);
    EXPECT_EQ(li.get(0), 0);
    EXPECT_EQ(li.get(1), -1);

    DoublyLinkedList<std::string> ls;
    ls.append(""); ls.append("after");
    EXPECT_EQ(ls.get(0), "");
    EXPECT_EQ(ls.get(1), "after");
}

// 1d. DLL tail pointer: last appended element must be reachable via
//     get(size-1) with backward traversal (index >= size/2 path).
TEST(DLL_Append, TailPointerUpdates) {
    DoublyLinkedList<int> list;
    for (int i = 0; i < 6; i++) list.append(i * 5);  // 0,5,10,15,20,25
    // index 5 >= size/2 (3) → backward traversal from tail
    EXPECT_EQ(list.get(5), 25);
    EXPECT_EQ(list.get(3), 15);
    // index 0,1,2 → forward traversal from head
    EXPECT_EQ(list.get(0), 0);
    EXPECT_EQ(list.get(2), 10);
}

// 1e. Cross-type: double precision preserved; Point fields intact after append.
TEST(DLL_Append, CrossTypeDoubleAndStruct) {
    DoublyLinkedList<double> ld;
    ld.append(3.14159); ld.append(-0.001);
    EXPECT_DOUBLE_EQ(ld.get(0), 3.14159);
    EXPECT_DOUBLE_EQ(ld.get(1), -0.001);

    DoublyLinkedList<Point> lp;
    lp.append(Point(1, 2)); lp.append(Point(3, 4));
    EXPECT_EQ(lp.get(0), (Point{1, 2}));
    EXPECT_EQ(lp.get(1), (Point{3, 4}));
}

// 1f. Large dataset: DLL append is O(1) via tail pointer.
//     All values must be correct at boundaries and midpoint.
TEST(DLL_Append, LargeDataset) {
    DoublyLinkedList<int> list;
    for (int i = 0; i < 500; i++) list.append(i);
    EXPECT_EQ(list.getSize(), 500);
    EXPECT_EQ(list.get(0),   0);
    EXPECT_EQ(list.get(499), 499);
    EXPECT_EQ(list.get(250), 250);
}

// ================================================================
//  SECTION 2 — get()
//  DLL-specific: bidirectional traversal (front half vs back half).
//  Edge cases: midpoint boundary, negative, past-end, empty,
//  mutable T& return, const overload, cross-type.
// ================================================================

// 2a. Bidirectional traversal: index < size/2 uses forward path,
//     index >= size/2 uses backward path — both must return same value.
TEST(DLL_Get, BidirectionalTraversal) {
    DoublyLinkedList<int> list;
    for (int i = 0; i < 6; i++) list.append(i * 10);  // [0,10,20,30,40,50]
    // Forward half (indices 0,1,2)
    EXPECT_EQ(list.get(0), 0);
    EXPECT_EQ(list.get(2), 20);
    // Backward half (indices 3,4,5)
    EXPECT_EQ(list.get(3), 30);
    EXPECT_EQ(list.get(5), 50);
}

// 2b. [BUG?] Exact midpoint (index == size/2) goes to backward path.
//     Verify the integer division boundary is handled correctly.
TEST(DLL_Get, MidpointBoundary) {
    DoublyLinkedList<int> list;
    for (int i = 1; i <= 5; i++) list.append(i);  // [1,2,3,4,5], size=5
    // size/2 = 2 → indices 0,1 forward; indices 2,3,4 backward
    EXPECT_EQ(list.get(1), 2);  // last forward index
    EXPECT_EQ(list.get(2), 3);  // first backward index
    EXPECT_EQ(list.get(4), 5);  // tail
}

// 2c. Negative index, index == size, and far OOB must all throw const char*.
TEST(DLL_Get, OutOfBoundsThrows) {
    DoublyLinkedList<int> list;
    list.append(1);
    EXPECT_THROW(list.get(-1),  const char*);   // negative
    EXPECT_THROW(list.get(1),   const char*);   // exactly size
    EXPECT_THROW(list.get(999), const char*);   // far out
}

// 2d. get(0) on empty list must throw (no head node to dereference).
TEST(DLL_Get, EmptyListThrows) {
    DoublyLinkedList<int> li;
    EXPECT_THROW(li.get(0), const char*);

    DoublyLinkedList<std::string> ls;
    EXPECT_THROW(ls.get(0), const char*);
}

// 2e. get() returns T& — in-place mutation must persist on next call.
TEST(DLL_Get, MutableReferenceWorks) {
    DoublyLinkedList<int> list;
    list.append(10);
    list.get(0) = 99;
    EXPECT_EQ(list.get(0), 99);

    DoublyLinkedList<Point> lp;
    lp.append(Point(1, 2));
    lp.get(0) = Point(9, 9);
    EXPECT_EQ(lp.get(0), (Point{9, 9}));
}

// 2f. const overload callable on const reference; OOB still throws.
TEST(DLL_Get, ConstOverload) {
    DoublyLinkedList<int> list;
    list.append(77); list.append(88);
    const DoublyLinkedList<int>& cRef = list;
    EXPECT_EQ(cRef.get(0), 77);
    EXPECT_EQ(cRef.get(1), 88);
    EXPECT_THROW(cRef.get(2), const char*);
}

// ================================================================
//  SECTION 3 — insert()
//  DLL-specific: prev/next pointers must be set on both sides.
//  Edge cases: empty list at 0, head change (prev=null), tail path,
//  middle re-link, end == append, invalid indices.
// ================================================================

// 3a. Insert at index 0 into empty list sets both head and tail.
TEST(DLL_Insert, IntoEmptyAtZero) {
    DoublyLinkedList<int> list;
    list.insert(0, 42);
    EXPECT_EQ(list.getSize(), 1);
    EXPECT_EQ(list.get(0), 42);
    // A second append must go to tail correctly after this insert.
    list.append(99);
    EXPECT_EQ(list.get(1), 99);
}

// 3b. Insert at index 0: new node's prev must be null; old head's prev
//     must point to new node (DLL-specific invariant).
TEST(DLL_Insert, AtBeginningFixesPrevPointer) {
    DoublyLinkedList<int> list;
    list.append(1); list.append(2); list.append(3);
    list.insert(0, 99);                 // [99, 1, 2, 3]
    EXPECT_EQ(list.getSize(), 4);
    EXPECT_EQ(list.get(0), 99);
    EXPECT_EQ(list.get(1), 1);
    EXPECT_EQ(list.get(3), 3);
    // Removing the new head must leave a consistent list.
    list.remove(0);
    EXPECT_EQ(list.get(0), 1);
}

// 3c. Insert in middle: both prev and next pointers of neighbours must
//     be updated (walk the chain after insert to verify).
TEST(DLL_Insert, AtMiddleCorrectLinks) {
    DoublyLinkedList<int> list;
    list.append(1); list.append(2); list.append(4);
    list.insert(2, 3);                  // [1, 2, 3, 4]
    EXPECT_EQ(list.getSize(), 4);
    EXPECT_EQ(list.get(2), 3);
    EXPECT_EQ(list.get(3), 4);
    // Traverse forward then backward via get() to stress both paths.
    EXPECT_EQ(list.get(0), 1);
    EXPECT_EQ(list.get(3), 4);
}

// 3d. Insert at index == size delegates to append (valid tail boundary).
TEST(DLL_Insert, AtEndEquivalentToAppend) {
    DoublyLinkedList<std::string> list;
    list.append("a"); list.append("b");
    list.insert(list.getSize(), "c");   // [a, b, c]
    EXPECT_EQ(list.getSize(), 3);
    EXPECT_EQ(list.get(2), "c");
}

// 3e. Invalid indices must throw.
TEST(DLL_Insert, InvalidIndicesThrow) {
    DoublyLinkedList<int> list;
    list.append(1);
    EXPECT_THROW(list.insert(-1, 0),                 const char*);
    EXPECT_THROW(list.insert(list.getSize() + 1, 0), const char*);

    DoublyLinkedList<int> empty;
    EXPECT_THROW(empty.insert(1, 10), const char*);  // size==0, only 0 is valid
}

// 3f. Struct inserted at middle: all four surrounding pointers remain valid.
TEST(DLL_Insert, StructAtMiddle) {
    DoublyLinkedList<Point> list;
    list.append(Point(1, 1)); list.append(Point(3, 3));
    list.insert(1, Point(2, 2));        // [P(1,1), P(2,2), P(3,3)]
    EXPECT_EQ(list.get(0), (Point{1, 1}));
    EXPECT_EQ(list.get(1), (Point{2, 2}));
    EXPECT_EQ(list.get(2), (Point{3, 3}));
}

// ================================================================
//  SECTION 4 — remove()
//  DLL-specific: THREE distinct branches (head / tail / middle).
//  Edge cases: only element (head==tail→both null), head removal
//  (new head's prev=null), tail removal (new tail's next=null),
//  middle (neighbours re-link), OOB throws, cross-type.
// ================================================================

// 4a. Remove only element → head AND tail must both become null.
//     Appending after must work cleanly (head==tail again).
TEST(DLL_Remove, OnlyElementClearsHeadAndTail) {
    DoublyLinkedList<int> list;
    list.append(42);
    list.remove(0);
    EXPECT_EQ(list.getSize(), 0);
    EXPECT_THROW(list.get(0), const char*);
    // Append after empty-remove must set head & tail correctly.
    list.append(7);
    EXPECT_EQ(list.getSize(), 1);
    EXPECT_EQ(list.get(0), 7);
}

// 4b. Remove head: next node's prev pointer must be set to null.
TEST(DLL_Remove, HeadRemovalFixesPrev) {
    DoublyLinkedList<int> list;
    for (int i = 1; i <= 4; i++) list.append(i);   // [1,2,3,4]
    list.remove(0);                                  // [2,3,4]
    EXPECT_EQ(list.getSize(), 3);
    EXPECT_EQ(list.get(0), 2);
    // New head's prev must be null; inserting at 0 must not corrupt chain.
    list.insert(0, 99);
    EXPECT_EQ(list.get(0), 99);
    EXPECT_EQ(list.get(1), 2);
}

// 4c. [BUG?] Remove tail: prev node's next pointer must be set to null,
//     and tail pointer must update. Check via a subsequent append.
TEST(DLL_Remove, TailRemovalUpdatesTail) {
    DoublyLinkedList<int> list;
    for (int i = 1; i <= 4; i++) list.append(i);   // [1,2,3,4]
    list.remove(list.getSize() - 1);                 // [1,2,3]
    EXPECT_EQ(list.getSize(), 3);
    EXPECT_EQ(list.get(2), 3);                      // new tail
    // Append must attach to the new tail (not the freed old node).
    list.append(99);
    EXPECT_EQ(list.get(3), 99);
}

// 4d. Remove middle: both surrounding prev/next pointers must be updated.
TEST(DLL_Remove, MiddleClosesGap) {
    DoublyLinkedList<int> list;
    for (int i = 1; i <= 5; i++) list.append(i);   // [1,2,3,4,5]
    list.remove(2);                                  // [1,2,4,5]
    EXPECT_EQ(list.getSize(), 4);
    EXPECT_EQ(list.get(1), 2);
    EXPECT_EQ(list.get(2), 4);
    // Remove again from the new middle to stress the prev pointer.
    list.remove(2);                                  // [1,2,5]
    EXPECT_EQ(list.get(2), 5);
}

// 4e. Empty list, negative, and OOB must all throw.
TEST(DLL_Remove, InvalidIndicesThrow) {
    DoublyLinkedList<int> empty;
    EXPECT_THROW(empty.remove(0), const char*);

    DoublyLinkedList<int> list;
    list.append(1);
    EXPECT_THROW(list.remove(-1),             const char*);
    EXPECT_THROW(list.remove(list.getSize()), const char*);
    EXPECT_THROW(list.remove(999),            const char*);
}

// 4f. Cross-type: Point remove preserves surrounding nodes' field values.
TEST(DLL_Remove, StructRemovePreservesNeighbours) {
    DoublyLinkedList<Point> list;
    list.append(Point(1, 10));
    list.append(Point(2, 20));
    list.append(Point(3, 30));
    list.remove(1);                         // removes Point(2,20)
    EXPECT_EQ(list.getSize(), 2);
    EXPECT_EQ(list.get(0), (Point{1, 10}));
    EXPECT_EQ(list.get(1), (Point{3, 30}));
}

// ================================================================
//  SECTION 5 — getSize()
//  Edge cases: fresh list, append/remove/insert tracking,
//  mixed ops, drain to zero, const-correctness.
// ================================================================

// 5a. Brand-new list starts at 0.
TEST(DLL_GetSize, InitialSizeIsZero) {
    DoublyLinkedList<int> list;
    EXPECT_EQ(list.getSize(), 0);
}

// 5b. Size increments on every append.
TEST(DLL_GetSize, TracksAppends) {
    DoublyLinkedList<int> list;
    for (int i = 0; i < 6; i++) {
        list.append(i);
        EXPECT_EQ(list.getSize(), i + 1);
    }
}

// 5c. Size decrements on remove; drain all the way to 0.
TEST(DLL_GetSize, TracksRemoves) {
    DoublyLinkedList<int> list;
    for (int i = 0; i < 4; i++) list.append(i);
    for (int i = 3; i >= 0; i--) {
        list.remove(0);
        EXPECT_EQ(list.getSize(), i);
    }
}

// 5d. Size increments on insert (at head and tail).
TEST(DLL_GetSize, TracksInserts) {
    DoublyLinkedList<int> list;
    list.append(10);
    list.insert(0, 5);                   EXPECT_EQ(list.getSize(), 2);
    list.insert(list.getSize(), 20);     EXPECT_EQ(list.getSize(), 3);
}

// 5e. Mixed sequence: append → remove → insert → net size correct.
TEST(DLL_GetSize, MixedOperations) {
    DoublyLinkedList<int> list;
    for (int i = 0; i < 10; i++) list.append(i);   // +10
    list.remove(0); list.remove(0);                  // -2 → 8
    list.insert(0, 99); list.insert(0, 100);         // +2 → 10
    EXPECT_EQ(list.getSize(), 10);
}

// 5f. getSize() is declared const — callable on a const reference.
TEST(DLL_GetSize, CalledOnConstRef) {
    DoublyLinkedList<int> list;
    list.append(1); list.append(2);
    const DoublyLinkedList<int>& cRef = list;
    EXPECT_EQ(cRef.getSize(), 2);
}

// ================================================================
//  SECTION 6 — Copy Constructor & operator= (Rule of Three complete)
//  DLL-specific: operator= IS defined (unlike SLL).
//  Edge cases: copy independence, empty copy, self-assign,
//  assign smaller over larger, post-copy destruct safety.
// ================================================================

// 6a. Copy constructor produces a deep copy with correct values.
TEST(DLL_RuleOfThree, CopyConstructorIsDeep) {
    DoublyLinkedList<int> a;
    a.append(1); a.append(2); a.append(3);
    DoublyLinkedList<int> b(a);
    EXPECT_EQ(b.getSize(), 3);
    EXPECT_EQ(b.get(0), 1);
    EXPECT_EQ(b.get(2), 3);
}

// 6b. Mutating the copy must not affect the original (nodes are independent).
TEST(DLL_RuleOfThree, CopyIsIndependent) {
    DoublyLinkedList<int> a;
    a.append(10);
    DoublyLinkedList<int> b(a);
    b.get(0) = 99;
    EXPECT_EQ(a.get(0), 10);    // original untouched
    b.append(20);
    EXPECT_EQ(a.getSize(), 1);  // original size unchanged
}

// 6c. Copying an empty list → no crash; copy is also empty.
TEST(DLL_RuleOfThree, CopyEmptyList) {
    DoublyLinkedList<int> a;
    DoublyLinkedList<int> b(a);
    EXPECT_EQ(b.getSize(), 0);
    EXPECT_THROW(b.get(0), const char*);
}

// 6d. operator= (defined in DLL, missing in SLL): deep copy with source intact.
TEST(DLL_RuleOfThree, AssignmentOperatorIsDeep) {
    DoublyLinkedList<std::string> a;
    a.append("hello"); a.append("world");
    DoublyLinkedList<std::string> b;
    b = a;
    a.get(0) = "changed";       // mutate source after assign
    EXPECT_EQ(b.get(0), "hello");   // copy must not reflect change
    EXPECT_EQ(b.getSize(), 2);
}

// 6e. Self-assignment guard (if(this==&other)): must be a no-op.
TEST(DLL_RuleOfThree, SelfAssignmentSafe) {
    DoublyLinkedList<int> list;
    list.append(42); list.append(43);
    list = list;                // must not free-then-use own nodes
    EXPECT_EQ(list.getSize(), 2);
    EXPECT_EQ(list.get(0), 42);
    EXPECT_EQ(list.get(1), 43);
}

// 6f. [BUG?] Assign smaller over larger: old nodes must be freed and
//     new chain must be valid with correct head/tail pointers.
TEST(DLL_RuleOfThree, AssignSmallerOverLarger) {
    DoublyLinkedList<Point> big;
    for (int i = 0; i < 5; i++) big.append(Point(i, i * 10));

    DoublyLinkedList<Point> small;
    small.append(Point(9, 90));

    big = small;                            // discard 5 nodes, copy 1
    EXPECT_EQ(big.getSize(), 1);
    EXPECT_EQ(big.get(0), (Point{9, 90}));
    // Append after assign must use the correct new tail.
    big.append(Point(8, 80));
    EXPECT_EQ(big.getSize(), 2);
    EXPECT_EQ(big.get(1), (Point{8, 80}));
}
