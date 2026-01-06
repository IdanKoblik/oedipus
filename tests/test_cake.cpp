#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <cstdio>
#include "cake.h"

using namespace cake;


class CakeTest : public ::testing::Test {
protected:
    void SetUp() override {
        char tmpl[] = "/tmp/cake_test_XXXXXX";
        const int fd = mkstemp(tmpl);
        ASSERT_NE(fd, -1);

        close(fd);
        test_file = tmpl;
    }

    void TearDown() override {
        if (std::filesystem::exists(test_file))
            std::filesystem::remove(test_file);
    }

    std::string test_file;
};

TEST_F(CakeTest, LoadFromFile_EmptyFile) {
    std::ofstream file(test_file);
    file.close();

    Cake cake;
    cake.loadFromFile(test_file);

    EXPECT_EQ(cake.lineCount(), 1);
    EXPECT_EQ(cake.renderLine(0), "");
    EXPECT_EQ(cake.lineLength(0), 0);
}

TEST_F(CakeTest, LoadFromFile_SingleLine) {
    std::ofstream file(test_file);
    file << "Hello World";
    file.close();

    Cake cake;
    cake.loadFromFile(test_file);

    EXPECT_EQ(cake.lineCount(), 1);
    EXPECT_EQ(cake.renderLine(0), "Hello World");
    EXPECT_EQ(cake.lineLength(0), 11);
}

TEST_F(CakeTest, LoadFromFile_MultipleLines) {
    std::ofstream file(test_file);
    file << "Line 1\nLine 2\nLine 3";
    file.close();

    Cake cake;
    cake.loadFromFile(test_file);

    EXPECT_EQ(cake.lineCount(), 3);
    EXPECT_EQ(cake.renderLine(0), "Line 1");
    EXPECT_EQ(cake.renderLine(1), "Line 2");
    EXPECT_EQ(cake.renderLine(2), "Line 3");
}

TEST_F(CakeTest, LoadFromFile_EmptyLines) {
    std::ofstream file(test_file);
    file << "Line 1\n\nLine 3\n";
    file.close();

    Cake cake;
    cake.loadFromFile(test_file);

    EXPECT_EQ(cake.lineCount(), 4);
    EXPECT_EQ(cake.renderLine(0), "Line 1");
    EXPECT_EQ(cake.renderLine(1), "");
    EXPECT_EQ(cake.renderLine(2), "Line 3");
    EXPECT_EQ(cake.renderLine(3), "");
}

TEST_F(CakeTest, InsertChar_BeginningOfLine) {
    Cake cake;
    cake.loadFromFile(test_file);
    
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    cake.loadFromFile(test_file);

    cake.insertChar(0, 0, 'X');

    EXPECT_EQ(cake.renderLine(0), "XHello");
    EXPECT_EQ(cake.lineLength(0), 6);
}

TEST_F(CakeTest, InsertChar_MiddleOfLine) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    cake.loadFromFile(test_file);

    cake.insertChar(3, 0, 'X');

    EXPECT_EQ(cake.renderLine(0), "HelXlo");
    EXPECT_EQ(cake.lineLength(0), 6);
}

TEST_F(CakeTest, InsertChar_EndOfLine) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    cake.loadFromFile(test_file);

    cake.insertChar(5, 0, 'X');

    EXPECT_EQ(cake.renderLine(0), "HelloX");
    EXPECT_EQ(cake.lineLength(0), 6);
}

TEST_F(CakeTest, InsertChar_MultipleInserts) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    cake.loadFromFile(test_file);

    cake.insertChar(0, 0, 'A');
    cake.insertChar(1, 0, 'B');
    cake.insertChar(2, 0, 'C');

    EXPECT_EQ(cake.renderLine(0), "ABCHello");
    EXPECT_EQ(cake.lineLength(0), 8);
}

TEST_F(CakeTest, DeleteChar_BeginningOfLine) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    cake.loadFromFile(test_file);

    cake.deleteChar(0, 0);

    EXPECT_EQ(cake.renderLine(0), "Hello");
}

TEST_F(CakeTest, DeleteChar_MiddleOfLine) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    cake.loadFromFile(test_file);

    cake.deleteChar(3, 0);

    EXPECT_EQ(cake.renderLine(0), "Helo");
    EXPECT_EQ(cake.lineLength(0), 4);
}

TEST_F(CakeTest, DeleteChar_EndOfLine) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    cake.loadFromFile(test_file);

    cake.deleteChar(5, 0);

    EXPECT_EQ(cake.renderLine(0), "Hell");
    EXPECT_EQ(cake.lineLength(0), 4);
}

TEST_F(CakeTest, DeleteChar_MultipleDeletes) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    cake.loadFromFile(test_file);

    cake.deleteChar(5, 0);
    cake.deleteChar(4, 0);
    cake.deleteChar(3, 0);

    EXPECT_EQ(cake.renderLine(0), "He");
    EXPECT_EQ(cake.lineLength(0), 2);
}

TEST_F(CakeTest, InsertNewLine_Beginning) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    cake.loadFromFile(test_file);

    cake.insertNewLine(0, 0);

    EXPECT_EQ(cake.lineCount(), 2);
    EXPECT_EQ(cake.renderLine(0), "");
    EXPECT_EQ(cake.renderLine(1), "Hello");
}

TEST_F(CakeTest, InsertNewLine_Middle) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    cake.loadFromFile(test_file);

    cake.insertNewLine(3, 0);

    EXPECT_EQ(cake.lineCount(), 2);
    EXPECT_EQ(cake.renderLine(0), "Hel");
    EXPECT_EQ(cake.renderLine(1), "lo");
}

TEST_F(CakeTest, InsertNewLine_End) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    cake.loadFromFile(test_file);

    cake.insertNewLine(5, 0);

    EXPECT_EQ(cake.lineCount(), 2);
    EXPECT_EQ(cake.renderLine(0), "Hello");
    EXPECT_EQ(cake.renderLine(1), "");
}

TEST_F(CakeTest, RemoveLine_SingleLine) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    cake.loadFromFile(test_file);

    cake.removeLine(0);

    EXPECT_EQ(cake.lineCount(), 1);
    EXPECT_EQ(cake.renderLine(0), "");
}

TEST_F(CakeTest, RemoveLine_FirstLine) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Line 1\nLine 2\nLine 3";
    file.close();
    cake.loadFromFile(test_file);

    cake.removeLine(0);

    EXPECT_EQ(cake.lineCount(), 2);
    EXPECT_EQ(cake.renderLine(0), "Line 1Line 2");
    EXPECT_EQ(cake.renderLine(1), "Line 3");
}

TEST_F(CakeTest, RemoveLine_MiddleLine) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Line 1\nLine 2\nLine 3";
    file.close();
    cake.loadFromFile(test_file);

    cake.removeLine(1);

    EXPECT_EQ(cake.lineCount(), 2);
    EXPECT_EQ(cake.renderLine(0), "Line 1Line 2");
    EXPECT_EQ(cake.renderLine(1), "Line 3");
}

TEST_F(CakeTest, RemoveLine_LastLine) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Line 1\nLine 2\nLine 3";
    file.close();
    cake.loadFromFile(test_file);

    cake.removeLine(2);

    EXPECT_EQ(cake.lineCount(), 2);
    EXPECT_EQ(cake.renderLine(0), "Line 1");
    EXPECT_EQ(cake.renderLine(1), "Line 2Line 3");
}

TEST_F(CakeTest, SaveToFile_SimpleContent) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Hello World";
    file.close();
    cake.loadFromFile(test_file);

    char tmpl[] = "/tmp/cake_save_test_XXXXXX";
    const int fd = mkstemp(tmpl);
    ASSERT_NE(fd, -1);

    close(fd);
    std::string save_file = tmpl;
    cake.saveToFile(save_file);

    std::ifstream saved(save_file);
    std::string content((std::istreambuf_iterator<char>(saved)),
                        std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "Hello World");

    std::filesystem::remove(save_file);
}

TEST_F(CakeTest, SaveToFile_ModifiedContent) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    cake.loadFromFile(test_file);

    cake.insertChar(5, 0, ' ');
    cake.insertChar(6, 0, 'W');
    cake.insertChar(7, 0, 'o');
    cake.insertChar(8, 0, 'r');
    cake.insertChar(9, 0, 'l');
    cake.insertChar(10, 0, 'd');

    char tmpl[] = "/tmp/cake_save_test_XXXXXX";
    const int fd = mkstemp(tmpl);
    ASSERT_NE(fd, -1);

    close(fd);
    std::string save_file = tmpl;
    cake.saveToFile(save_file);

    std::ifstream saved(save_file);
    std::string content((std::istreambuf_iterator<char>(saved)),
                        std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "Hello World");

    std::filesystem::remove(save_file);
}

TEST_F(CakeTest, SaveToFile_MultipleLines) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Line 1\nLine 2\nLine 3";
    file.close();
    cake.loadFromFile(test_file);

    char tmpl[] = "/tmp/cake_save_test_XXXXXX";
    const int fd = mkstemp(tmpl);
    ASSERT_NE(fd, -1);

    close(fd);
    std::string save_file = tmpl;
    cake.saveToFile(save_file);

    std::ifstream saved(save_file);
    std::string content((std::istreambuf_iterator<char>(saved)),
                        std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "Line 1\nLine 2\nLine 3");

    std::filesystem::remove(save_file);
}

TEST_F(CakeTest, GetLines_ReturnsAllLines) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Line 1\nLine 2\nLine 3";
    file.close();
    cake.loadFromFile(test_file);

    auto lines = cake.getLines();
    EXPECT_EQ(lines.size(), 3);
    EXPECT_EQ(lines[0], "Line 1");
    EXPECT_EQ(lines[1], "Line 2");
    EXPECT_EQ(lines[2], "Line 3");
}

TEST_F(CakeTest, LineLength_CalculatesCorrectly) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Hello\nWorld\nTest";
    file.close();
    cake.loadFromFile(test_file);

    EXPECT_EQ(cake.lineLength(0), 5);
    EXPECT_EQ(cake.lineLength(1), 5);
    EXPECT_EQ(cake.lineLength(2), 4);
}

TEST_F(CakeTest, ComplexEditSequence) {
    Cake cake;
    std::ofstream file(test_file);
    file << "Hello\nWorld";
    file.close();
    cake.loadFromFile(test_file);

    cake.insertChar(0, 0, 'X');
    EXPECT_EQ(cake.renderLine(0), "XHello");

    cake.deleteChar(3, 0);
    EXPECT_EQ(cake.renderLine(0), "XHllo");

    cake.insertNewLine(4, 0);
    EXPECT_EQ(cake.lineCount(), 3);
    EXPECT_EQ(cake.renderLine(0), "XHll");
    EXPECT_EQ(cake.renderLine(1), "o");
    EXPECT_EQ(cake.renderLine(2), "World");
}

