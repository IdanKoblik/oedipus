#include "algorithem/search.h"

#include <gtest/gtest.h>

using namespace search;

TEST(SearchTest, KMP_EmptyPattern) {
    std::vector<size_t> result = kmp("Hello World", "");
    EXPECT_TRUE(result.empty());
}

TEST(SearchTest, KMP_EmptyText) {
    std::vector<size_t> result = kmp("", "pattern");
    EXPECT_TRUE(result.empty());
}

TEST(SearchTest, KMP_BothEmpty) {
    std::vector<size_t> result = kmp("", "");
    EXPECT_TRUE(result.empty());
}

TEST(SearchTest, KMP_SingleMatch) {
    std::vector<size_t> result = kmp("Hello World", "World");
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], 6);
}

TEST(SearchTest, KMP_MultipleMatches) {
    std::vector<size_t> result = kmp("abababab", "ab");
    ASSERT_EQ(result.size(), 4);
    EXPECT_EQ(result[0], 0);
    EXPECT_EQ(result[1], 2);
    EXPECT_EQ(result[2], 4);
    EXPECT_EQ(result[3], 6);
}

TEST(SearchTest, KMP_NoMatch) {
    std::vector<size_t> result = kmp("Hello World", "xyz");
    EXPECT_TRUE(result.empty());
}

TEST(SearchTest, KMP_MatchAtBeginning) {
    std::vector<size_t> result = kmp("Hello World", "Hello");
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], 0);
}

TEST(SearchTest, KMP_MatchAtEnd) {
    std::vector<size_t> result = kmp("Hello World", "World");
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], 6);
}

TEST(SearchTest, KMP_OverlappingMatches) {
    std::vector<size_t> result = kmp("aaaa", "aa");
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 0);
    EXPECT_EQ(result[1], 1);
    EXPECT_EQ(result[2], 2);
}

TEST(SearchTest, KMP_PatternLongerThanText) {
    std::vector<size_t> result = kmp("abc", "abcdef");
    EXPECT_TRUE(result.empty());
}

TEST(SearchTest, KMP_SingleCharacterPattern) {
    std::vector<size_t> result = kmp("Hello", "l");
    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 2);
    EXPECT_EQ(result[1], 3);
}

TEST(SearchTest, KMP_RepeatedPattern) {
    std::vector<size_t> result = kmp("abcabcabc", "abc");
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 0);
    EXPECT_EQ(result[1], 3);
    EXPECT_EQ(result[2], 6);
}

TEST(SearchTest, KMP_ComplexPattern) {
    std::vector<size_t> result = kmp("abacabacaba", "aba");
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 0);
    EXPECT_EQ(result[1], 4);
    EXPECT_EQ(result[2], 8);
}

TEST(SearchTest, KMP_AllSameCharacters) {
    std::vector<size_t> result = kmp("aaaaaaaa", "aaa");
    ASSERT_EQ(result.size(), 6);
    for (size_t i = 0; i < 6; ++i) {
        EXPECT_EQ(result[i], i);
    }
}

TEST(SearchTest, KMP_UnicodeLike) {
    std::vector<size_t> result = kmp("test123test456test", "test");
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 0);
    EXPECT_EQ(result[1], 7);
    EXPECT_EQ(result[2], 14);
}

TEST(SearchTest, KMP_PatternWithSpecialChars) {
    std::vector<size_t> result = kmp("hello\nworld\ntest", "\n");
    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 5);
    EXPECT_EQ(result[1], 11);
}

TEST(SearchTest, KMP_ExactMatch) {
    std::vector<size_t> result = kmp("test", "test");
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], 0);
}
