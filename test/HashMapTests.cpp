#include <gtest/gtest.h>
#include "../include/hashMap.h"
#include <string>

// ================================================================
//  User-defined VALUE type (no hashCode needed — used as value only).
// ================================================================
struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
    bool operator!=(const Point& o) const { return !(*this == o); }
};

// ================================================================
//  User-defined KEY type.
//  hashFunction.generate(T) calls key.hashCode() — so any struct
//  used as a KEY must implement hashCode() AND operator==.
//  [BUG?] Without hashCode() the code won't compile at all —
//         there is no fallback for arbitrary structs as keys.
// ================================================================
struct PointKey {
    int x, y;
    PointKey(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const PointKey& o) const { return x == o.x && y == o.y; }
    // hashCode() required by HashFunction::generate<T>()
    int hashCode() const { return x * 31 + y; }
};

// ================================================================
//  SECTION 1 — set() and get()
//  Edge cases: int/string/char/bool keys, struct value, negative
//  key, zero key, empty-string key, mutable reference return.
// ================================================================

// 1a. Basic int key → int value round-trip.
TEST(SetGet, IntKeyIntValue) {
    HashMap<int, int> map;
    map.set(1, 100);
    EXPECT_EQ(map.get(1), 100);
}

// 1b. Negative int key must be handled (getBucketIndex negates hash).
TEST(SetGet, NegativeKeyAndZeroKey) {
    HashMap<int, int> map;
    map.set(-5, 42);
    map.set(0,  999);
    EXPECT_EQ(map.get(-5), 42);
    EXPECT_EQ(map.get(0),  999);
}

// 1c. String key — relies on the polynomial hash overload.
TEST(SetGet, StringKeyVariousValues) {
    HashMap<std::string, std::string> map;
    map.set("name", "yash");
    map.set("city", "delhi");
    EXPECT_EQ(map.get("name"), "yash");
    EXPECT_EQ(map.get("city"), "delhi");
    // Empty string as key: hash(0-length) == 0 → always bucket 0.
    map.set("", "empty");
    EXPECT_EQ(map.get(""), "empty");
}

// 1d. Struct (Point) as VALUE with int key — verifies template expansion.
TEST(SetGet, StructValue) {
    HashMap<int, Point> map;
    map.set(1, Point(10, 20));
    map.set(2, Point(0, 0));
    EXPECT_EQ(map.get(1), Point(10, 20));
    EXPECT_EQ(map.get(2), Point(0, 0));
}

// 1e. get() returns V& — in-place mutation must persist.
TEST(SetGet, MutableReferenceReturn) {
    HashMap<std::string, int> map;
    map.set("score", 0);
    map.get("score") = 100;
    EXPECT_EQ(map.get("score"), 100);

    HashMap<int, Point> pm;
    pm.set(1, Point(0, 0));
    pm.get(1).x = 99;
    EXPECT_EQ(pm.get(1).x, 99);
}

// 1f. [BUG?] PointKey as KEY: requires hashCode() and operator==.
//     Without hashCode(), the template generate(const T&) won't compile.
//     This test documents that the requirement IS met when both are provided.
TEST(SetGet, ObjectAsKey) {
    HashMap<PointKey, std::string> map;
    map.set(PointKey(1, 2), "A");
    map.set(PointKey(3, 4), "B");
    EXPECT_EQ(map.get(PointKey(1, 2)), "A");
    EXPECT_EQ(map.get(PointKey(3, 4)), "B");
    EXPECT_TRUE(map.exist(PointKey(1, 2)));
    EXPECT_FALSE(map.exist(PointKey(9, 9)));
}

// ================================================================
//  SECTION 2 — Overwrite (set() on an existing key)
//  Edge cases: size must NOT grow, new value visible, struct value,
//  overwrite then remove leaves size==0, multiple overwrites.
// ================================================================

// 2a. Overwriting an existing key replaces value and keeps size==1.
TEST(Overwrite, ValueUpdatedSizeStable) {
    HashMap<int, int> map;
    map.set(1, 10);
    map.set(1, 99);
    EXPECT_EQ(map.get(1), 99);
    EXPECT_EQ(map.getSize(), 1);
}

// 2b. String key overwrite — also verifies string hash doesn't cache stale.
TEST(Overwrite, StringKeyOverwrite) {
    HashMap<std::string, std::string> map;
    map.set("city", "delhi");
    map.set("city", "mumbai");
    EXPECT_EQ(map.get("city"), "mumbai");
    EXPECT_EQ(map.getSize(), 1);
}

// 2c. Multiple successive overwrites — size stays 1; last value wins.
TEST(Overwrite, ChainedOverwrites) {
    HashMap<int, int> map;
    map.set(5, 1); map.set(5, 2); map.set(5, 3);
    EXPECT_EQ(map.get(5), 3);
    EXPECT_EQ(map.getSize(), 1);
}

// 2d. [BUG?] Overwrite then remove: set() does remove-then-append internally.
//     If the index after re-append differs from where it was, a subsequent
//     remove() must still find and erase the single remaining entry.
TEST(Overwrite, OverwriteThenRemoveLeavesMapEmpty) {
    HashMap<int, int> map;
    map.set(42, 1);
    map.set(42, 2);   // internal: removes old, appends new to end of chain
    map.set(42, 3);   // overwrite again
    map.remove(42);
    EXPECT_FALSE(map.exist(42));
    EXPECT_THROW(map.get(42), const char*);
    EXPECT_EQ(map.getSize(), 0);  // must be 0, not -1 (size drifts bug)
}

// 2e. PointKey overwrite: same key object (equal by operator==) overwrites.
TEST(Overwrite, ObjectKeyOverwrite) {
    HashMap<PointKey, int> map;
    map.set(PointKey(1, 1), 10);
    map.set(PointKey(1, 1), 99);  // same key by operator==
    EXPECT_EQ(map.get(PointKey(1, 1)), 99);
    EXPECT_EQ(map.getSize(), 1);
}

// ================================================================
//  SECTION 3 — remove()
//  Edge cases: removes value, size decrements, missing key throws,
//  collision chain integrity, PointKey remove, remove-all then refill.
// ================================================================

// 3a. Remove existing key: gone from exist(), get() throws.
TEST(Remove, ExistingKeyRemoved) {
    HashMap<int, int> map;
    map.set(1, 10);
    map.remove(1);
    EXPECT_FALSE(map.exist(1));
    EXPECT_EQ(map.getSize(), 0);
    EXPECT_THROW(map.get(1), const char*);
}

// 3b. Removing a non-existent key must throw const char*.
TEST(Remove, MissingKeyThrows) {
    HashMap<int, int> map;
    EXPECT_THROW(map.remove(99), const char*);

    HashMap<std::string, int> smap;
    EXPECT_THROW(smap.remove("nope"), const char*);
}

// 3c. Keys 0 and 4 collide in bucket 0 (capacity=4).
//     Removing one must leave the other intact.
TEST(Remove, CollisionChainIntegrity) {
    HashMap<int, int> map;
    map.set(0, 100); map.set(4, 400);   // same bucket initially
    map.remove(0);
    EXPECT_FALSE(map.exist(0));
    EXPECT_TRUE(map.exist(4));
    EXPECT_EQ(map.get(4), 400);
}

// 3d. Remove all entries one by one; map must reach size==0 and be reusable.
TEST(Remove, DrainThenRefill) {
    HashMap<int, int> map;
    for (int i = 0; i < 8; i++) map.set(i, i * 5);
    for (int i = 0; i < 8; i++) { map.remove(i); }
    EXPECT_EQ(map.getSize(), 0);
    EXPECT_TRUE(map.empty());
    // Refill after empty
    for (int i = 0; i < 5; i++) map.set(i, i * 100);
    for (int i = 0; i < 5; i++) EXPECT_EQ(map.get(i), i * 100);
}

// 3e. PointKey as key: remove must use operator== to match, not address.
TEST(Remove, ObjectKeyRemove) {
    HashMap<PointKey, std::string> map;
    map.set(PointKey(1, 2), "A");
    map.set(PointKey(3, 4), "B");
    map.remove(PointKey(1, 2));
    EXPECT_FALSE(map.exist(PointKey(1, 2)));
    EXPECT_TRUE(map.exist(PointKey(3, 4)));
    EXPECT_EQ(map.get(PointKey(3, 4)), "B");
}

// ================================================================
//  SECTION 4 — exist() and get() throws
//  Edge cases: present/absent, after remove, after clear,
//  const overload, object key, missing-key get() throw.
// ================================================================

// 4a. exist() returns true for present key, false for absent.
TEST(Exist, PresentAndAbsent) {
    HashMap<int, int> map;
    map.set(42, 1);
    EXPECT_TRUE(map.exist(42));
    EXPECT_FALSE(map.exist(0));
    EXPECT_FALSE(map.exist(99));
}

// 4b. After remove, exist() returns false; get() throws.
TEST(Exist, AfterRemoveAndAfterClear) {
    HashMap<int, int> map;
    map.set(7, 7);
    map.remove(7);
    EXPECT_FALSE(map.exist(7));
    EXPECT_THROW(map.get(7), const char*);

    map.set(8, 8);
    map.clear();
    EXPECT_FALSE(map.exist(8));
}

// 4c. const overload: exist() and get() callable on const reference.
TEST(Exist, ConstRefExistAndGet) {
    HashMap<int, int> map;
    map.set(1, 100);
    const HashMap<int, int>& cmap = map;
    EXPECT_TRUE(cmap.exist(1));
    EXPECT_FALSE(cmap.exist(9));
    EXPECT_EQ(cmap.get(1), 100);
    EXPECT_THROW(cmap.get(9), const char*);
}

// 4d. [BUG?] bool key: only 2 buckets used (hash 0 or 1).
//     Setting both true and false causes a 2-slot collision if capacity≤2.
//     Verify both are independently accessible.
TEST(Exist, BoolKeyBothValues) {
    HashMap<bool, int> map;
    map.set(true,  1);
    map.set(false, 0);
    EXPECT_TRUE(map.exist(true));
    EXPECT_TRUE(map.exist(false));
    EXPECT_EQ(map.get(true),  1);
    EXPECT_EQ(map.get(false), 0);
}

// 4e. PointKey exist(): two distinct keys with different hashCode must
//     not collide (unless hash wraps to the same bucket).
TEST(Exist, ObjectKeyExist) {
    HashMap<PointKey, int> map;
    map.set(PointKey(1, 0), 10);
    map.set(PointKey(0, 1), 20);
    EXPECT_TRUE(map.exist(PointKey(1, 0)));
    EXPECT_TRUE(map.exist(PointKey(0, 1)));
    EXPECT_FALSE(map.exist(PointKey(5, 5)));
}

// ================================================================
//  SECTION 5 — Rehash
//  Initial capacity=4, load factor threshold=0.75 (triggers at size>3).
//  Edge cases: all data survives, size correct, PointKey survives,
//  reference invalidation after rehash, multiple rehash cycles.
// ================================================================

// 5a. All int key-value pairs survive one rehash (size 4 → triggers).
TEST(Rehash, DataIntactAfterRehash) {
    HashMap<int, int> map;
    for (int i = 0; i < 20; i++) map.set(i, i * 10);
    for (int i = 0; i < 20; i++) {
        EXPECT_TRUE(map.exist(i));
        EXPECT_EQ(map.get(i), i * 10);
    }
    EXPECT_EQ(map.getSize(), 20);
}

// 5b. String keys all survive multiple rehash cycles.
TEST(Rehash, StringKeysAfterRehash) {
    HashMap<std::string, int> map;
    map.set("alpha", 1); map.set("beta",  2);
    map.set("gamma", 3); map.set("delta", 4);   // triggers rehash
    map.set("epsilon", 5);
    EXPECT_EQ(map.get("alpha"),   1);
    EXPECT_EQ(map.get("beta"),    2);
    EXPECT_EQ(map.get("gamma"),   3);
    EXPECT_EQ(map.get("delta"),   4);
    EXPECT_EQ(map.get("epsilon"), 5);
}

// 5c. Struct values survive rehash without corruption.
TEST(Rehash, StructValuesAfterRehash) {
    HashMap<int, Point> map;
    for (int i = 0; i < 15; i++) map.set(i, Point(i, i * 2));
    for (int i = 0; i < 15; i++) EXPECT_EQ(map.get(i), Point(i, i * 2));
}

// 5d. [BUG?] PointKey as key survives rehash: getBucketIndex uses capacity
//     which doubles on rehash — keys must re-hash correctly to new buckets.
TEST(Rehash, ObjectKeyAfterRehash) {
    HashMap<PointKey, std::string> map;
    for (int i = 0; i < 8; i++) map.set(PointKey(i, i * 2), std::to_string(i));
    for (int i = 0; i < 8; i++)
        EXPECT_EQ(map.get(PointKey(i, i * 2)), std::to_string(i));
    EXPECT_EQ(map.getSize(), 8);
}

// 5e. [BUG?] Reference returned by get() is invalidated after rehash:
//     rehash() calls explicit ~DynamicArray() and replaces buckets,
//     so any V& obtained before rehash points to freed memory.
//     This test documents the hazard — the reference must NOT be used
//     after a set() that triggers rehash.
TEST(Rehash, RefInvalidatedAfterRehash) {
    HashMap<int, int> map;
    map.set(1, 10); map.set(2, 20); map.set(3, 30);
    // map.get(1) returns a reference into the current bucket array.
    // The NEXT set() will push load > 0.75 and trigger rehash,
    // reallocating buckets — the old reference becomes dangling.
    // We only verify that the value is still correct via a fresh get().
    map.set(4, 40);   // triggers rehash (size 4 / capacity 4 = 1.0 > 0.75)
    EXPECT_EQ(map.get(1), 10);   // fresh lookup must work after rehash
    EXPECT_EQ(map.get(4), 40);
}

// ================================================================
//  SECTION 6 — clear() and empty()
//  Edge cases: all keys gone, size zero, reuse after clear,
//  clear on empty map, multiple clear cycles, clear with struct keys.
// ================================================================

// 6a. After clear(), all previously set keys are absent.
TEST(Clear, AllKeysGone) {
    HashMap<int, int> map;
    map.set(1, 1); map.set(2, 2); map.set(3, 3);
    map.clear();
    EXPECT_EQ(map.getSize(), 0);
    EXPECT_TRUE(map.empty());
    EXPECT_FALSE(map.exist(1));
    EXPECT_FALSE(map.exist(2));
    EXPECT_FALSE(map.exist(3));
}

// 6b. clear() on already-empty map must not crash.
TEST(Clear, ClearOnEmptyMap) {
    HashMap<int, int> map;
    EXPECT_NO_THROW(map.clear());
    EXPECT_EQ(map.getSize(), 0);
}

// 6c. Map is fully reusable after clear (set, get, size all work).
TEST(Clear, ReuseAfterClear) {
    HashMap<std::string, std::string> map;
    map.set("k", "v1");
    map.clear();
    map.set("k", "v2");
    EXPECT_EQ(map.get("k"), "v2");
    EXPECT_EQ(map.getSize(), 1);
}

// 6d. [BUG?] clear() calls chain.~DoublyLinkedList() explicitly then
//     assigns a new empty DLL via operator=. If the explicit destructor
//     double-frees the chain's memory, the second call corrupts the heap.
//     Three clear() cycles stress-test this path.
TEST(Clear, MultipleClearCycles) {
    HashMap<int, int> map;
    for (int cycle = 0; cycle < 3; cycle++) {
        map.set(cycle * 10, cycle);
        map.set(cycle * 10 + 1, cycle + 1);
        EXPECT_EQ(map.getSize(), 2);
        map.clear();
        EXPECT_EQ(map.getSize(), 0);
    }
}

// 6e. clear() with PointKey entries: struct data properly destroyed.
TEST(Clear, ObjectKeyClear) {
    HashMap<PointKey, std::string> map;
    map.set(PointKey(1, 1), "A");
    map.set(PointKey(2, 2), "B");
    map.clear();
    EXPECT_FALSE(map.exist(PointKey(1, 1)));
    EXPECT_FALSE(map.exist(PointKey(2, 2)));
    EXPECT_EQ(map.getSize(), 0);
}

// ================================================================
//  SECTION 7 — Copy Constructor and operator=
//  Edge cases: deep copy independence, empty copy, self-assign,
//  assign smaller over larger, PointKey copy survives.
// ================================================================

// 7a. Copy constructor: copied map has all keys; mutation is independent.
TEST(RuleOfThree, CopyConstructorIsDeep) {
    HashMap<int, int> a;
    a.set(1, 10); a.set(2, 20);
    HashMap<int, int> b(a);
    EXPECT_EQ(b.get(1), 10);
    EXPECT_EQ(b.get(2), 20);
    b.set(1, 999);
    EXPECT_EQ(a.get(1), 10);  // original untouched
    EXPECT_EQ(b.getSize(), 2);
}

// 7b. operator= deep copies; source mutation after assign doesn't affect dest.
TEST(RuleOfThree, AssignmentIsDeep) {
    HashMap<std::string, std::string> a;
    a.set("k", "v1");
    HashMap<std::string, std::string> b;
    b = a;
    a.set("k", "changed");
    EXPECT_EQ(b.get("k"), "v1");
}

// 7c. Self-assignment guard: must be a no-op.
TEST(RuleOfThree, SelfAssignmentSafe) {
    HashMap<int, int> map;
    map.set(1, 42);
    map = map;
    EXPECT_EQ(map.get(1), 42);
    EXPECT_EQ(map.getSize(), 1);
}

// 7d. Copying an empty map → no crash; copy is also empty.
TEST(RuleOfThree, CopyEmptyMap) {
    HashMap<int, int> a;
    HashMap<int, int> b(a);
    EXPECT_EQ(b.getSize(), 0);
    EXPECT_TRUE(b.empty());
}

// 7e. PointKey map copy: keys and values survive copy construction.
TEST(RuleOfThree, ObjectKeyCopy) {
    HashMap<PointKey, std::string> a;
    a.set(PointKey(1, 2), "hello");
    a.set(PointKey(3, 4), "world");
    HashMap<PointKey, std::string> b(a);
    EXPECT_EQ(b.get(PointKey(1, 2)), "hello");
    EXPECT_EQ(b.get(PointKey(3, 4)), "world");
    EXPECT_EQ(b.getSize(), 2);
}

// ================================================================
//  SECTION 8 — Size drift and getSize() / empty()
//  A common bug: size counter desynchronises under mixed operations.
// ================================================================

// 8a. Size tracks set, overwrite, remove, and clear correctly.
TEST(SizeAndEmpty, TracksMixedOps) {
    HashMap<int, int> map;
    for (int i = 0; i < 5; i++) map.set(i, i);
    EXPECT_EQ(map.getSize(), 5);
    map.set(0, 99); map.set(2, 99);    // overwrites — size stays 5
    EXPECT_EQ(map.getSize(), 5);
    map.remove(1); map.remove(3);      // removes — size → 3
    EXPECT_EQ(map.getSize(), 3);
    map.set(10, 10); map.set(11, 11);  // new keys — size → 5
    EXPECT_EQ(map.getSize(), 5);
}

// 8b. Remove then re-insert same key: size must reach 1, not 2.
TEST(SizeAndEmpty, RemoveAndReinsertSameKey) {
    HashMap<std::string, int> map;
    map.set("x", 1);
    map.remove("x");
    EXPECT_EQ(map.getSize(), 0);
    map.set("x", 2);
    EXPECT_EQ(map.getSize(), 1);
    EXPECT_EQ(map.get("x"), 2);
}

// 8c. empty() returns false while any key exists, true when all removed.
TEST(SizeAndEmpty, EmptyFlagAccurate) {
    HashMap<int, int> map;
    EXPECT_TRUE(map.empty());
    map.set(1, 1);
    EXPECT_FALSE(map.empty());
    map.remove(1);
    EXPECT_TRUE(map.empty());
}

// 8d. getSize() and empty() both callable on const reference.
TEST(SizeAndEmpty, ConstRefAccess) {
    HashMap<int, int> map;
    map.set(1, 1); map.set(2, 2);
    const HashMap<int, int>& cmap = map;
    EXPECT_EQ(cmap.getSize(), 2);
    EXPECT_FALSE(cmap.empty());
}

// 8e. [BUG?] size must never go negative (remove after clear / double remove).
TEST(SizeAndEmpty, SizeNeverNegative) {
    HashMap<int, int> map;
    map.set(1, 1);
    map.remove(1);
    EXPECT_GE(map.getSize(), 0);
    EXPECT_EQ(map.getSize(), 0);
    // Second remove must throw, not decrement size below 0.
    EXPECT_THROW(map.remove(1), const char*);
    EXPECT_EQ(map.getSize(), 0);
}
