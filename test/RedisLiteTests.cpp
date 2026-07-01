#include <gtest/gtest.h>
#include "../include/redisLite.h"
#include <string>

using namespace std;


// ================================================================
// SECTION 1 — set()
// Edge cases:
//  • Normal insertion
//  • Overwrite existing key
//  • Empty key
//  • Empty value
//  • Numeric strings
//  • Special characters
//  • Long strings
// ================================================================

// ------------------------------------------------
// 1a. Basic insertion and retrieval.
// ------------------------------------------------
TEST(Set, BasicInsertion) {
    RedisLite redis;

    redis.set("name", "Yash");

    EXPECT_EQ(redis.get("name"), "Yash");
    EXPECT_TRUE(redis.exist("name"));
}

// ------------------------------------------------
// 1b. Overwriting an existing key.
// Size should remain unchanged.
// ------------------------------------------------
TEST(Set, OverwriteExistingKey) {
    RedisLite redis;

    redis.set("city", "Delhi");
    redis.set("city", "Mumbai");

    EXPECT_EQ(redis.get("city"), "Mumbai");
    EXPECT_EQ(redis.size(), 1);
}

// ------------------------------------------------
// 1c. Empty key.
// Redis itself allows empty keys.
// ------------------------------------------------
TEST(Set, EmptyKey) {
    RedisLite redis;

    redis.set("", "emptyKey");

    EXPECT_TRUE(redis.exist(""));
    EXPECT_EQ(redis.get(""), "emptyKey");
}

// ------------------------------------------------
// 1d. Empty value.
// ------------------------------------------------
TEST(Set, EmptyValue) {
    RedisLite redis;

    redis.set("language", "");

    EXPECT_TRUE(redis.exist("language"));
    EXPECT_EQ(redis.get("language"), "");
}

// ------------------------------------------------
// 1e. Numeric strings.
// ------------------------------------------------
TEST(Set, NumericStrings) {
    RedisLite redis;

    redis.set("123", "456");

    EXPECT_TRUE(redis.exist("123"));
    EXPECT_EQ(redis.get("123"), "456");
}

// ------------------------------------------------
// 1f. Special characters.
// ------------------------------------------------
TEST(Set, SpecialCharacters) {
    RedisLite redis;

    redis.set("@#$%^&*", "!)(*&^%$#@");

    EXPECT_TRUE(redis.exist("@#$%^&*"));
    EXPECT_EQ(redis.get("@#$%^&*"), "!)(*&^%$#@");
}

// ------------------------------------------------
// 1g. Key containing spaces.
// ------------------------------------------------
TEST(Set, KeyWithSpaces) {
    RedisLite redis;

    redis.set("first name", "Yash");

    EXPECT_EQ(redis.get("first name"), "Yash");
}

// ------------------------------------------------
// 1h. Very long key.
// ------------------------------------------------
TEST(Set, LongKey) {
    RedisLite redis;

    string key(10000, 'K');

    redis.set(key, "value");

    EXPECT_EQ(redis.get(key), "value");
}

// ------------------------------------------------
// 1i. Very long value.
// ------------------------------------------------
TEST(Set, LongValue) {
    RedisLite redis;

    string value(10000, 'A');

    redis.set("large", value);

    EXPECT_EQ(redis.get("large"), value);
}

// ------------------------------------------------
// 1j. Multiple insertions.
// ------------------------------------------------
TEST(Set, MultipleInsertions) {
    RedisLite redis;

    for (int i = 0; i < 100; i++) {
        redis.set("key" + to_string(i),
                  "value" + to_string(i));
    }

    EXPECT_EQ(redis.size(), 100);

    for (int i = 0; i < 100; i++) {
        EXPECT_EQ(redis.get("key" + to_string(i)),
                  "value" + to_string(i));
    }
}

// ------------------------------------------------
// 1k. [BUG?]
// Multiple overwrites should never increase size.
// ------------------------------------------------
TEST(Set, MultipleOverwrites) {
    RedisLite redis;

    for (int i = 0; i < 20; i++) {
        redis.set("sameKey", to_string(i));
    }

    EXPECT_EQ(redis.size(), 1);
    EXPECT_EQ(redis.get("sameKey"), "19");
}

// ------------------------------------------------
// 1l. [BUG?]
// Reinsert after delete.
// ------------------------------------------------
TEST(Set, ReinsertAfterDelete) {
    RedisLite redis;

    redis.set("A", "1");

    redis.del("A");

    redis.set("A", "2");

    EXPECT_EQ(redis.size(), 1);
    EXPECT_EQ(redis.get("A"), "2");
}

// ================================================================
// SECTION 2 — get()
// Edge cases:
//  • Existing key
//  • Missing key
//  • Empty key
//  • Empty value
//  • Long value
//  • After overwrite
//  • After delete
//  • After clear
// ================================================================

// ------------------------------------------------
// 2a. Existing key.
// ------------------------------------------------
TEST(Get, ExistingKey) {
    RedisLite redis;

    redis.set("framework", "RedisLite");

    EXPECT_EQ(redis.get("framework"), "RedisLite");
}

// ------------------------------------------------
// 2b. Missing key.
//
// [BUG?]
// Current implementation returns
// "key doesn't exist"
// instead of throwing.
// ------------------------------------------------
TEST(Get, MissingKey) {
    RedisLite redis;

    EXPECT_EQ(redis.get("unknown"),
              "key doesn't exist");
}

// ------------------------------------------------
// 2c. Empty key.
// ------------------------------------------------
TEST(Get, EmptyKey) {
    RedisLite redis;

    redis.set("", "works");

    EXPECT_EQ(redis.get(""), "works");
}

// ------------------------------------------------
// 2d. Empty value.
// ------------------------------------------------
TEST(Get, EmptyValue) {
    RedisLite redis;

    redis.set("empty", "");

    EXPECT_EQ(redis.get("empty"), "");
}

// ------------------------------------------------
// 2e. Long value retrieval.
// ------------------------------------------------
TEST(Get, LongValue) {
    RedisLite redis;

    string value(15000, 'X');

    redis.set("blob", value);

    EXPECT_EQ(redis.get("blob"), value);
}

// ------------------------------------------------
// 2f. Retrieval after overwrite.
// ------------------------------------------------
TEST(Get, AfterOverwrite) {
    RedisLite redis;

    redis.set("version", "1");
    redis.set("version", "2");

    EXPECT_EQ(redis.get("version"), "2");
}

// ------------------------------------------------
// 2g. Retrieval after delete.
// ------------------------------------------------
TEST(Get, AfterDelete) {
    RedisLite redis;

    redis.set("A", "B");

    redis.del("A");

    EXPECT_EQ(redis.get("A"),
              "key doesn't exist");
}

// ------------------------------------------------
// 2h. Retrieval after clear.
// ------------------------------------------------
TEST(Get, AfterClear) {
    RedisLite redis;

    redis.set("one", "1");
    redis.set("two", "2");

    redis.clear();

    EXPECT_EQ(redis.get("one"),
              "key doesn't exist");

    EXPECT_EQ(redis.get("two"),
              "key doesn't exist");
}

// ------------------------------------------------
// 2i. Stress retrieval.
// ------------------------------------------------
TEST(Get, StressRetrieval) {
    RedisLite redis;

    for (int i = 0; i < 500; i++) {
        redis.set(
            "K" + to_string(i),
            "V" + to_string(i)
        );
    }

    for (int i = 0; i < 500; i++) {
        EXPECT_EQ(
            redis.get("K" + to_string(i)),
            "V" + to_string(i)
        );
    }
}

// ------------------------------------------------
// 2j. [BUG?]
// Sequential overwrite and retrieval.
// ------------------------------------------------
TEST(Get, ContinuousOverwrite) {
    RedisLite redis;

    for (int i = 0; i < 100; i++) {
        redis.set("counter",
                  to_string(i));

        EXPECT_EQ(redis.get("counter"),
                  to_string(i));
    }

    EXPECT_EQ(redis.size(), 1);
}
// ================================================================
// SECTION 3 — del()
// Edge cases:
//  • Existing key
//  • Missing key
//  • Delete twice
//  • Delete after overwrite
//  • Empty key
//  • Long key
// ================================================================

// ------------------------------------------------
// 3a. Delete an existing key.
// ------------------------------------------------
TEST(Delete, ExistingKey) {
    RedisLite redis;

    redis.set("name", "Yash");
    redis.del("name");

    EXPECT_FALSE(redis.exist("name"));
    EXPECT_EQ(redis.get("name"), "key doesn't exist");
    EXPECT_EQ(redis.size(), 0);
}

// ------------------------------------------------
// 3b. Delete a missing key.
//
// Should not throw or crash.
// ------------------------------------------------
TEST(Delete, MissingKey) {
    RedisLite redis;

    EXPECT_NO_THROW(redis.del("unknown"));

    EXPECT_EQ(redis.size(), 0);
    EXPECT_TRUE(redis.empty());
}

// ------------------------------------------------
// 3c. Delete the same key twice.
//
// [BUG?]
// Second deletion should not corrupt state.
// ------------------------------------------------
TEST(Delete, DeleteTwice) {
    RedisLite redis;

    redis.set("A", "1");

    redis.del("A");
    redis.del("A");

    EXPECT_EQ(redis.size(), 0);
    EXPECT_FALSE(redis.exist("A"));
}

// ------------------------------------------------
// 3d. Delete after overwrite.
// ------------------------------------------------
TEST(Delete, AfterOverwrite) {
    RedisLite redis;

    redis.set("city", "Delhi");
    redis.set("city", "Mumbai");

    redis.del("city");

    EXPECT_FALSE(redis.exist("city"));
    EXPECT_EQ(redis.get("city"),
              "key doesn't exist");
}

// ------------------------------------------------
// 3e. Delete empty key.
// ------------------------------------------------
TEST(Delete, EmptyKey) {
    RedisLite redis;

    redis.set("", "works");

    redis.del("");

    EXPECT_FALSE(redis.exist(""));
    EXPECT_EQ(redis.size(), 0);
}

// ------------------------------------------------
// 3f. Delete long key.
// ------------------------------------------------
TEST(Delete, LongKey) {
    RedisLite redis;

    string key(10000, 'K');

    redis.set(key, "value");

    redis.del(key);

    EXPECT_FALSE(redis.exist(key));
}

// ------------------------------------------------
// 3g. Delete multiple keys.
// ------------------------------------------------
TEST(Delete, MultipleKeys) {
    RedisLite redis;

    for (int i = 0; i < 50; i++) {
        redis.set(
            "K" + to_string(i),
            "V" + to_string(i));
    }

    for (int i = 0; i < 50; i++) {
        redis.del("K" + to_string(i));
    }

    EXPECT_EQ(redis.size(), 0);
    EXPECT_TRUE(redis.empty());
}

// ------------------------------------------------
// 3h. Delete then reinsert.
// ------------------------------------------------
TEST(Delete, ReinsertAfterDelete) {
    RedisLite redis;

    redis.set("A", "1");

    redis.del("A");

    redis.set("A", "2");

    EXPECT_EQ(redis.size(), 1);
    EXPECT_EQ(redis.get("A"), "2");
}

// ------------------------------------------------
// 3i. [BUG?]
// Size consistency after mixed deletes.
// ------------------------------------------------
TEST(Delete, SizeConsistency) {
    RedisLite redis;

    for (int i = 0; i < 20; i++) {
        redis.set(
            "K" + to_string(i),
            "V");
    }

    for (int i = 0; i < 10; i++) {
        redis.del("K" + to_string(i));
    }

    EXPECT_EQ(redis.size(), 10);

    for (int i = 10; i < 20; i++) {
        EXPECT_TRUE(
            redis.exist("K" + to_string(i)));
    }
}

// ------------------------------------------------
// 3j. [BUG?]
// Delete all then reuse database.
// ------------------------------------------------
TEST(Delete, DeleteAllReuse) {
    RedisLite redis;

    for (int i = 0; i < 30; i++) {
        redis.set(
            "K" + to_string(i),
            "V");
    }

    for (int i = 0; i < 30; i++) {
        redis.del("K" + to_string(i));
    }

    redis.set("new", "value");

    EXPECT_EQ(redis.size(), 1);
    EXPECT_EQ(redis.get("new"), "value");
}

// ================================================================
// SECTION 4 — exist()
// Edge cases:
//  • Existing key
//  • Missing key
//  • Empty key
//  • After delete
//  • After clear
//  • Long key
// ================================================================

// ------------------------------------------------
// 4a. Existing key.
// ------------------------------------------------
TEST(Exist, ExistingKey) {
    RedisLite redis;

    redis.set("framework", "RedisLite");

    EXPECT_TRUE(redis.exist("framework"));
}

// ------------------------------------------------
// 4b. Missing key.
// ------------------------------------------------
TEST(Exist, MissingKey) {
    RedisLite redis;

    EXPECT_FALSE(redis.exist("missing"));
}

// ------------------------------------------------
// 4c. Empty key.
// ------------------------------------------------
TEST(Exist, EmptyKey) {
    RedisLite redis;

    redis.set("", "works");

    EXPECT_TRUE(redis.exist(""));
}

// ------------------------------------------------
// 4d. After delete.
// ------------------------------------------------
TEST(Exist, AfterDelete) {
    RedisLite redis;

    redis.set("A", "1");

    redis.del("A");

    EXPECT_FALSE(redis.exist("A"));
}

// ------------------------------------------------
// 4e. After clear.
// ------------------------------------------------
TEST(Exist, AfterClear) {
    RedisLite redis;

    redis.set("one", "1");
    redis.set("two", "2");

    redis.clear();

    EXPECT_FALSE(redis.exist("one"));
    EXPECT_FALSE(redis.exist("two"));
}

// ------------------------------------------------
// 4f. Long key.
// ------------------------------------------------
TEST(Exist, LongKey) {
    RedisLite redis;

    string key(15000, 'X');

    redis.set(key, "value");

    EXPECT_TRUE(redis.exist(key));
}

// ------------------------------------------------
// 4g. Numeric string.
// ------------------------------------------------
TEST(Exist, NumericStringKey) {
    RedisLite redis;

    redis.set("123456", "number");

    EXPECT_TRUE(redis.exist("123456"));
}

// ------------------------------------------------
// 4h. Special characters.
// ------------------------------------------------
TEST(Exist, SpecialCharacters) {
    RedisLite redis;

    redis.set("@#$%^", "&*()");

    EXPECT_TRUE(redis.exist("@#$%^"));
}

// ------------------------------------------------
// 4i. [BUG?]
// Multiple overwrites should never
// affect exist().
// ------------------------------------------------
TEST(Exist, AfterMultipleOverwrites) {
    RedisLite redis;

    for (int i = 0; i < 100; i++) {
        redis.set(
            "counter",
            to_string(i));
    }

    EXPECT_TRUE(redis.exist("counter"));
    EXPECT_EQ(redis.size(), 1);
}

// ------------------------------------------------
// 4j. [BUG?]
// Stress existence checking.
// ------------------------------------------------
TEST(Exist, StressExist) {
    RedisLite redis;

    for (int i = 0; i < 500; i++) {
        redis.set(
            "K" + to_string(i),
            "V");
    }

    for (int i = 0; i < 500; i++) {
        EXPECT_TRUE(
            redis.exist(
                "K" + to_string(i)));
    }

    EXPECT_FALSE(redis.exist("Unknown"));
}
// ================================================================
// SECTION 5 — clear()
// Edge cases:
//  • Clear populated database
//  • Clear empty database
//  • Multiple clear cycles
//  • Reuse after clear
//  • Stress clear
// ================================================================

// ------------------------------------------------
// 5a. Clear a populated database.
// ------------------------------------------------
TEST(Clear, PopulatedDatabase) {
    RedisLite redis;

    redis.set("A", "1");
    redis.set("B", "2");
    redis.set("C", "3");

    redis.clear();

    EXPECT_EQ(redis.size(), 0);
    EXPECT_TRUE(redis.empty());

    EXPECT_FALSE(redis.exist("A"));
    EXPECT_FALSE(redis.exist("B"));
    EXPECT_FALSE(redis.exist("C"));
}

// ------------------------------------------------
// 5b. Clear an already empty database.
// ------------------------------------------------
TEST(Clear, EmptyDatabase) {
    RedisLite redis;

    EXPECT_NO_THROW(redis.clear());

    EXPECT_EQ(redis.size(), 0);
    EXPECT_TRUE(redis.empty());
}

// ------------------------------------------------
// 5c. Reuse after clear.
// ------------------------------------------------
TEST(Clear, ReuseAfterClear) {
    RedisLite redis;

    redis.set("A", "1");
    redis.clear();

    redis.set("B", "2");

    EXPECT_EQ(redis.size(), 1);
    EXPECT_TRUE(redis.exist("B"));
    EXPECT_EQ(redis.get("B"), "2");
}

// ------------------------------------------------
// 5d. Multiple clear cycles.
//
// [BUG?]
// Repeated clear() should never corrupt state.
// ------------------------------------------------
TEST(Clear, MultipleClearCycles) {
    RedisLite redis;

    for (int cycle = 0; cycle < 5; cycle++) {

        redis.set("A", "1");
        redis.set("B", "2");

        redis.clear();

        EXPECT_EQ(redis.size(), 0);
        EXPECT_TRUE(redis.empty());
    }
}

// ------------------------------------------------
// 5e. Stress clear.
// ------------------------------------------------
TEST(Clear, StressClear) {
    RedisLite redis;

    for (int i = 0; i < 1000; i++) {
        redis.set(
            "K" + to_string(i),
            "V" + to_string(i));
    }

    redis.clear();

    EXPECT_EQ(redis.size(), 0);
    EXPECT_TRUE(redis.empty());

    EXPECT_FALSE(redis.exist("K500"));
}

// ================================================================
// SECTION 6 — size()
// Edge cases:
//  • Initially zero
//  • After insertion
//  • After overwrite
//  • After delete
//  • Mixed operations
// ================================================================

// ------------------------------------------------
// 6a. Initial size.
// ------------------------------------------------
TEST(Size, InitiallyZero) {
    RedisLite redis;

    EXPECT_EQ(redis.size(), 0);
}

// ------------------------------------------------
// 6b. Size after insertion.
// ------------------------------------------------
TEST(Size, AfterInsertion) {
    RedisLite redis;

    for (int i = 0; i < 20; i++) {
        redis.set(
            "K" + to_string(i),
            "V");
    }

    EXPECT_EQ(redis.size(), 20);
}

// ------------------------------------------------
// 6c. Size should not increase after overwrite.
// ------------------------------------------------
TEST(Size, OverwriteDoesNotIncreaseSize) {
    RedisLite redis;

    redis.set("A", "1");

    for (int i = 0; i < 20; i++) {
        redis.set("A", to_string(i));
    }

    EXPECT_EQ(redis.size(), 1);
}

// ------------------------------------------------
// 6d. Size after delete.
// ------------------------------------------------
TEST(Size, AfterDelete) {
    RedisLite redis;

    redis.set("A", "1");
    redis.set("B", "2");

    redis.del("A");

    EXPECT_EQ(redis.size(), 1);

    redis.del("B");

    EXPECT_EQ(redis.size(), 0);
}

// ------------------------------------------------
// 6e. Mixed operations.
//
// [BUG?]
// Size should remain consistent.
// ------------------------------------------------
TEST(Size, MixedOperations) {
    RedisLite redis;

    for (int i = 0; i < 50; i++) {
        redis.set(
            "K" + to_string(i),
            "V");
    }

    for (int i = 0; i < 25; i++) {
        redis.del(
            "K" + to_string(i));
    }

    for (int i = 0; i < 10; i++) {
        redis.set(
            "K30",
            to_string(i));
    }

    EXPECT_EQ(redis.size(), 25);
}

// ================================================================
// SECTION 7 — empty()
// Edge cases:
//  • Initially empty
//  • After insert
//  • After delete
//  • After clear
//  • Mixed operations
// ================================================================

// ------------------------------------------------
// 7a. Initially empty.
// ------------------------------------------------
TEST(Empty, InitiallyTrue) {
    RedisLite redis;

    EXPECT_TRUE(redis.empty());
}

// ------------------------------------------------
// 7b. After insertion.
// ------------------------------------------------
TEST(Empty, AfterInsertion) {
    RedisLite redis;

    redis.set("A", "1");

    EXPECT_FALSE(redis.empty());
}

// ------------------------------------------------
// 7c. After deleting all keys.
// ------------------------------------------------
TEST(Empty, AfterDeleteAll) {
    RedisLite redis;

    redis.set("A", "1");
    redis.set("B", "2");

    redis.del("A");
    redis.del("B");

    EXPECT_TRUE(redis.empty());
}

// ------------------------------------------------
// 7d. After clear.
// ------------------------------------------------
TEST(Empty, AfterClear) {
    RedisLite redis;

    redis.set("A", "1");
    redis.set("B", "2");

    redis.clear();

    EXPECT_TRUE(redis.empty());
}

// ------------------------------------------------
// 7e. Mixed operations.
//
// [BUG?]
// empty() should always reflect
// the current database state.
// ------------------------------------------------
TEST(Empty, MixedOperations) {
    RedisLite redis;

    EXPECT_TRUE(redis.empty());

    redis.set("A", "1");
    EXPECT_FALSE(redis.empty());

    redis.del("A");
    EXPECT_TRUE(redis.empty());

    redis.set("B", "2");
    EXPECT_FALSE(redis.empty());

    redis.clear();
    EXPECT_TRUE(redis.empty());
}

// ================================================================
// SECTION 8 — Integration & Stress Tests
//
// Goal:
// Simulate real Redis usage by combining multiple operations.
// These tests are intended to expose hidden bugs such as:
//
// • Size drift
// • Incorrect overwrite behaviour
// • Lost entries after many operations
// • Reuse after clear()
// • Long string handling
// • Database consistency
// ================================================================

// ------------------------------------------------
// 8a. Complete Redis workflow.
// ------------------------------------------------
TEST(Integration, CompleteWorkflow) {
    RedisLite redis;

    redis.set("name", "Yash");
    redis.set("city", "Delhi");

    EXPECT_TRUE(redis.exist("name"));
    EXPECT_TRUE(redis.exist("city"));

    EXPECT_EQ(redis.get("name"), "Yash");

    redis.del("city");

    EXPECT_FALSE(redis.exist("city"));

    redis.set("city", "Mumbai");

    EXPECT_EQ(redis.get("city"), "Mumbai");

    EXPECT_EQ(redis.size(), 2);
}

// ------------------------------------------------
// 8b. Mixed operations.
//
// [BUG?]
// Size should always remain correct.
// ------------------------------------------------
TEST(Integration, MixedOperations) {
    RedisLite redis;

    for (int i = 0; i < 100; i++) {
        redis.set(
            "K" + std::to_string(i),
            "V");
    }

    for (int i = 0; i < 50; i++) {
        redis.del(
            "K" + std::to_string(i));
    }

    for (int i = 50; i < 100; i++) {
        redis.set(
            "K" + std::to_string(i),
            "Updated");
    }

    EXPECT_EQ(redis.size(), 50);

    for (int i = 50; i < 100; i++) {
        EXPECT_EQ(
            redis.get(
                "K" + std::to_string(i)),
            "Updated");
    }
}

// ------------------------------------------------
// 8c. Stress insertion.
//
// [BUG?]
// Large number of keys.
// ------------------------------------------------
TEST(Integration, StressInsertion) {
    RedisLite redis;

    const int COUNT = 5000;

    for (int i = 0; i < COUNT; i++) {
        redis.set(
            "Key" + std::to_string(i),
            "Value" + std::to_string(i));
    }

    EXPECT_EQ(redis.size(), COUNT);

    EXPECT_EQ(
        redis.get("Key0"),
        "Value0");

    EXPECT_EQ(
        redis.get("Key4999"),
        "Value4999");
}

// ------------------------------------------------
// 8d. Stress overwrite.
//
// Size should remain constant.
// ------------------------------------------------
TEST(Integration, StressOverwrite) {
    RedisLite redis;

    redis.set("counter", "0");

    for (int i = 1; i <= 1000; i++) {
        redis.set(
            "counter",
            std::to_string(i));
    }

    EXPECT_EQ(redis.size(), 1);

    EXPECT_EQ(
        redis.get("counter"),
        "1000");
}

// ------------------------------------------------
// 8e. Stress delete.
//
// [BUG?]
// Database should become empty.
// ------------------------------------------------
TEST(Integration, StressDelete) {
    RedisLite redis;

    for (int i = 0; i < 1000; i++) {
        redis.set(
            "K" + std::to_string(i),
            "V");
    }

    for (int i = 0; i < 1000; i++) {
        redis.del(
            "K" + std::to_string(i));
    }

    EXPECT_TRUE(redis.empty());

    EXPECT_EQ(redis.size(), 0);
}

// ------------------------------------------------
// 8f. Long key and long value.
//
// [BUG?]
// Checks string handling.
// ------------------------------------------------
TEST(Integration, LongKeyLongValue) {
    RedisLite redis;

    std::string key(20000, 'K');
    std::string value(30000, 'V');

    redis.set(key, value);

    EXPECT_TRUE(redis.exist(key));

    EXPECT_EQ(
        redis.get(key),
        value);
}

// ------------------------------------------------
// 8g. Clear then reuse.
//
// [BUG?]
// Internal HashMap should remain usable.
// ------------------------------------------------
TEST(Integration, ClearAndReuse) {
    RedisLite redis;

    for (int i = 0; i < 200; i++) {
        redis.set(
            "K" + std::to_string(i),
            "V");
    }

    redis.clear();

    EXPECT_TRUE(redis.empty());

    for (int i = 0; i < 100; i++) {
        redis.set(
            "New" + std::to_string(i),
            "Data");
    }

    EXPECT_EQ(redis.size(), 100);

    EXPECT_EQ(
        redis.get("New50"),
        "Data");
}

// ------------------------------------------------
// 8h. Repeated clear cycles.
//
// [BUG?]
// Memory corruption may appear here.
// ------------------------------------------------
TEST(Integration, MultipleClearCycles) {
    RedisLite redis;

    for (int cycle = 0; cycle < 20; cycle++) {

        for (int i = 0; i < 100; i++) {
            redis.set(
                "K" + std::to_string(i),
                "V");
        }

        redis.clear();

        EXPECT_TRUE(redis.empty());
        EXPECT_EQ(redis.size(), 0);
    }
}

// ------------------------------------------------
// 8i. Database consistency.
//
// [BUG?]
// Random-looking operation sequence.
// ------------------------------------------------
TEST(Integration, DatabaseConsistency) {
    RedisLite redis;

    redis.set("A", "1");
    redis.set("B", "2");
    redis.set("C", "3");

    redis.del("B");

    redis.set("D", "4");

    redis.set("A", "10");

    redis.del("C");

    EXPECT_EQ(redis.size(), 2);

    EXPECT_TRUE(redis.exist("A"));
    EXPECT_TRUE(redis.exist("D"));

    EXPECT_FALSE(redis.exist("B"));
    EXPECT_FALSE(redis.exist("C"));

    EXPECT_EQ(redis.get("A"), "10");
    EXPECT_EQ(redis.get("D"), "4");
}

// ------------------------------------------------
// 8j. Repeated insert-delete.
//
// [BUG?]
// Size should never drift.
// ------------------------------------------------
TEST(Integration, RepeatedInsertDelete) {
    RedisLite redis;

    for (int i = 0; i < 500; i++) {

        redis.set("temp", "1");

        EXPECT_EQ(redis.size(), 1);

        redis.del("temp");

        EXPECT_EQ(redis.size(), 0);

        EXPECT_TRUE(redis.empty());
    }
}

// ------------------------------------------------
// 8k. Missing keys after stress.
//
// [BUG?]
// Existing data should remain unaffected.
// ------------------------------------------------
TEST(Integration, MissingKeyAfterStress) {
    RedisLite redis;

    for (int i = 0; i < 1000; i++) {
        redis.set(
            "K" + std::to_string(i),
            "V");
    }

    EXPECT_EQ(
        redis.get("Unknown"),
        "key doesn't exist");

    EXPECT_EQ(redis.size(), 1000);
}

// ------------------------------------------------
// 8l. Special character stress.
// ------------------------------------------------
TEST(Integration, SpecialCharacterKeys) {
    RedisLite redis;

    redis.set("@#$%", "1");
    redis.set("!@#$%^&*()", "2");
    redis.set(" key ", "3");
    redis.set("\tTab", "4");
    redis.set("New\nLine", "5");

    EXPECT_EQ(redis.get("@#$%"), "1");
    EXPECT_EQ(redis.get("!@#$%^&*()"), "2");
    EXPECT_EQ(redis.get(" key "), "3");
    EXPECT_EQ(redis.get("\tTab"), "4");
    EXPECT_EQ(redis.get("New\nLine"), "5");
}

// ------------------------------------------------
// 8m. Massive mixed workload.
//
// [BUG?]
// Attempts to expose hidden synchronization
// or bookkeeping issues.
// ------------------------------------------------
TEST(Integration, MassiveMixedWorkload) {
    RedisLite redis;

    for (int round = 0; round < 50; round++) {

        for (int i = 0; i < 200; i++) {
            redis.set(
                "K" + std::to_string(i),
                std::to_string(round));
        }

        for (int i = 0; i < 100; i++) {
            redis.del(
                "K" + std::to_string(i));
        }

        for (int i = 100; i < 200; i++) {
            EXPECT_TRUE(
                redis.exist(
                    "K" + std::to_string(i)));
        }

        redis.clear();

        EXPECT_TRUE(redis.empty());
    }
}