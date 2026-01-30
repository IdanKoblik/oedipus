#include "structs/piece_table.h"

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <cstdio>

using namespace pieceTable;

class TableTest : public ::testing::Test {
protected:
    std::string test_file;

    void SetUp() override
    {
        char tmpl[] = "/tmp/pt_test_XXXXXX";
        const int fd = mkstemp(tmpl);
        ASSERT_NE(fd, -1);

        close(fd);
        test_file = tmpl;
    }

    void TearDown() override
    {
        if (std::filesystem::exists(test_file))
            std::filesystem::remove(test_file);
    }
};

TEST_F(TableTest, LoadFromFile_EmptyFile)
{
    std::ofstream file(test_file);
    file.close();

    Table table;
    table.loadFromFile(test_file);

    EXPECT_EQ(table.lineCount(), 1);
    EXPECT_EQ(table.renderLine(0), "");
    EXPECT_EQ(table.lineLength(0), 0);
}

TEST_F(TableTest, LoadFromFile_SingleLine)
{
    std::ofstream file(test_file);
    file << "Hello World";
    file.close();

    Table table;
    table.loadFromFile(test_file);

    EXPECT_EQ(table.lineCount(), 1);
    EXPECT_EQ(table.renderLine(0), "Hello World");
    EXPECT_EQ(table.lineLength(0), 11);
}

TEST_F(TableTest, LoadFromFile_MultipleLines)
{
    std::ofstream file(test_file);
    file << "Line 1\nLine 2\nLine 3";
    file.close();

    Table table;
    table.loadFromFile(test_file);

    EXPECT_EQ(table.lineCount(), 3);
    EXPECT_EQ(table.renderLine(0), "Line 1");
    EXPECT_EQ(table.renderLine(1), "Line 2");
    EXPECT_EQ(table.renderLine(2), "Line 3");
}

TEST_F(TableTest, LoadFromFile_EmptyLines)
{
    std::ofstream file(test_file);
    file << "Line 1\n\nLine 3\n";
    file.close();

    Table table;
    table.loadFromFile(test_file);

    EXPECT_EQ(table.lineCount(), 4);
    EXPECT_EQ(table.renderLine(0), "Line 1");
    EXPECT_EQ(table.renderLine(1), "");
    EXPECT_EQ(table.renderLine(2), "Line 3");
    EXPECT_EQ(table.renderLine(3), "");
}

TEST_F(TableTest, InsertChar_BeginningOfLine)
{
    Table table;
    table.loadFromFile(test_file);

    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    table.loadFromFile(test_file);

    table.insertChar(0, 0, 'X');

    EXPECT_EQ(table.renderLine(0), "XHello");
    EXPECT_EQ(table.lineLength(0), 6);
}

TEST_F(TableTest, InsertChar_MiddleOfLine)
{
    Table table;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    table.loadFromFile(test_file);

    table.insertChar(3, 0, 'X');

    EXPECT_EQ(table.renderLine(0), "HelXlo");
    EXPECT_EQ(table.lineLength(0), 6);
}

TEST_F(TableTest, InsertChar_EndOfLine)
{
    Table table;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    table.loadFromFile(test_file);

    table.insertChar(5, 0, 'X');

    EXPECT_EQ(table.renderLine(0), "HelloX");
    EXPECT_EQ(table.lineLength(0), 6);
}

TEST_F(TableTest, InsertChar_MultipleInserts)
{
    Table table;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    table.loadFromFile(test_file);

    table.insertChar(0, 0, 'A');
    table.insertChar(1, 0, 'B');
    table.insertChar(2, 0, 'C');

    EXPECT_EQ(table.renderLine(0), "ABCHello");
    EXPECT_EQ(table.lineLength(0), 8);
}

TEST_F(TableTest, DeleteChar_BeginningOfLine)
{
    Table table;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    table.loadFromFile(test_file);

    table.deleteChar(0, 0);

    EXPECT_EQ(table.renderLine(0), "Hello");
}

TEST_F(TableTest, DeleteChar_MiddleOfLine)
{
    Table table;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    table.loadFromFile(test_file);

    table.deleteChar(3, 0);

    EXPECT_EQ(table.renderLine(0), "Helo");
    EXPECT_EQ(table.lineLength(0), 4);
}

TEST_F(TableTest, DeleteChar_EndOfLine)
{
    Table table;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    table.loadFromFile(test_file);

    table.deleteChar(5, 0);

    EXPECT_EQ(table.renderLine(0), "Hell");
    EXPECT_EQ(table.lineLength(0), 4);
}

TEST_F(TableTest, DeleteChar_MultipleDeletes)
{
    Table table;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    table.loadFromFile(test_file);

    table.deleteChar(5, 0);
    table.deleteChar(4, 0);
    table.deleteChar(3, 0);

    EXPECT_EQ(table.renderLine(0), "He");
    EXPECT_EQ(table.lineLength(0), 2);
}

TEST_F(TableTest, InsertNewLine_Beginning)
{
    Table table;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    table.loadFromFile(test_file);

    table.insertNewLine(0, 0);

    EXPECT_EQ(table.lineCount(), 2);
    EXPECT_EQ(table.renderLine(0), "");
    EXPECT_EQ(table.renderLine(1), "Hello");
}

TEST_F(TableTest, InsertNewLine_Middle)
{
    Table table;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    table.loadFromFile(test_file);

    table.insertNewLine(3, 0);

    EXPECT_EQ(table.lineCount(), 2);
    EXPECT_EQ(table.renderLine(0), "Hel");
    EXPECT_EQ(table.renderLine(1), "lo");
}

TEST_F(TableTest, InsertNewLine_End)
{
    Table table;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    table.loadFromFile(test_file);

    table.insertNewLine(5, 0);

    EXPECT_EQ(table.lineCount(), 2);
    EXPECT_EQ(table.renderLine(0), "Hello");
    EXPECT_EQ(table.renderLine(1), "");
}

TEST_F(TableTest, RemoveLine_SingleLine)
{
    Table table;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    table.loadFromFile(test_file);

    table.removeLine(0);

    EXPECT_EQ(table.lineCount(), 1);
    EXPECT_EQ(table.renderLine(0), "");
}

TEST_F(TableTest, RemoveLine_FirstLine)
{
    Table table;
    std::ofstream file(test_file);
    file << "Line 1\nLine 2\nLine 3";
    file.close();
    table.loadFromFile(test_file);

    table.removeLine(0);

    EXPECT_EQ(table.lineCount(), 2);
    EXPECT_EQ(table.renderLine(0), "Line 1Line 2");
    EXPECT_EQ(table.renderLine(1), "Line 3");
}

TEST_F(TableTest, RemoveLine_MiddleLine)
{
    Table table;
    std::ofstream file(test_file);
    file << "Line 1\nLine 2\nLine 3";
    file.close();
    table.loadFromFile(test_file);

    table.removeLine(1);

    EXPECT_EQ(table.lineCount(), 2);
    EXPECT_EQ(table.renderLine(0), "Line 1Line 2");
    EXPECT_EQ(table.renderLine(1), "Line 3");
}

TEST_F(TableTest, RemoveLine_LastLine)
{
    Table table;
    std::ofstream file(test_file);
    file << "Line 1\nLine 2\nLine 3";
    file.close();
    table.loadFromFile(test_file);

    table.removeLine(2);

    EXPECT_EQ(table.lineCount(), 2);
    EXPECT_EQ(table.renderLine(0), "Line 1");
    EXPECT_EQ(table.renderLine(1), "Line 2Line 3");
}

TEST_F(TableTest, SaveToFile_SimpleContent)
{
    Table table;
    std::ofstream file(test_file);
    file << "Hello World";
    file.close();
    table.loadFromFile(test_file);

    char tmpl[] = "/tmp/table_save_test_XXXXXX";
    const int fd = mkstemp(tmpl);
    ASSERT_NE(fd, -1);

    close(fd);
    std::string save_file = tmpl;
    table.saveToFile(save_file);

    std::ifstream saved(save_file);
    std::string content((std::istreambuf_iterator<char>(saved)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "Hello World");

    std::filesystem::remove(save_file);
}

TEST_F(TableTest, SaveToFile_ModifiedContent)
{
    Table table;
    std::ofstream file(test_file);
    file << "Hello";
    file.close();
    table.loadFromFile(test_file);

    table.insertChar(5, 0, ' ');
    table.insertChar(6, 0, 'W');
    table.insertChar(7, 0, 'o');
    table.insertChar(8, 0, 'r');
    table.insertChar(9, 0, 'l');
    table.insertChar(10, 0, 'd');

    char tmpl[] = "/tmp/table_save_test_XXXXXX";
    const int fd = mkstemp(tmpl);
    ASSERT_NE(fd, -1);

    close(fd);
    std::string save_file = tmpl;
    table.saveToFile(save_file);

    std::ifstream saved(save_file);
    std::string content((std::istreambuf_iterator<char>(saved)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "Hello World");

    std::filesystem::remove(save_file);
}

TEST_F(TableTest, SaveToFile_MultipleLines)
{
    Table table;
    std::ofstream file(test_file);
    file << "Line 1\nLine 2\nLine 3";
    file.close();
    table.loadFromFile(test_file);

    char tmpl[] = "/tmp/table_save_test_XXXXXX";
    const int fd = mkstemp(tmpl);
    ASSERT_NE(fd, -1);

    close(fd);
    std::string save_file = tmpl;
    table.saveToFile(save_file);

    std::ifstream saved(save_file);
    std::string content((std::istreambuf_iterator<char>(saved)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "Line 1\nLine 2\nLine 3");

    std::filesystem::remove(save_file);
}

TEST_F(TableTest, GetLines_ReturnsAllLines)
{
    Table table;
    std::ofstream file(test_file);
    file << "Line 1\nLine 2\nLine 3";
    file.close();
    table.loadFromFile(test_file);

    auto lines = table.getLines();
    EXPECT_EQ(lines.size(), 3);
    EXPECT_EQ(lines[0], "Line 1");
    EXPECT_EQ(lines[1], "Line 2");
    EXPECT_EQ(lines[2], "Line 3");
}

TEST_F(TableTest, LineLength_CalculatesCorrectly)
{
    Table table;
    std::ofstream file(test_file);
    file << "Hello\nWorld\nTest";
    file.close();
    table.loadFromFile(test_file);

    EXPECT_EQ(table.lineLength(0), 5);
    EXPECT_EQ(table.lineLength(1), 5);
    EXPECT_EQ(table.lineLength(2), 4);
}

TEST_F(TableTest, ComplexEditSequence)
{
    Table table;
    std::ofstream file(test_file);
    file << "Hello\nWorld";
    file.close();
    table.loadFromFile(test_file);

    table.insertChar(0, 0, 'X');
    EXPECT_EQ(table.renderLine(0), "XHello");

    table.deleteChar(3, 0);
    EXPECT_EQ(table.renderLine(0), "XHllo");

    table.insertNewLine(4, 0);
    EXPECT_EQ(table.lineCount(), 3);
    EXPECT_EQ(table.renderLine(0), "XHll");
    EXPECT_EQ(table.renderLine(1), "o");
    EXPECT_EQ(table.renderLine(2), "World");
}