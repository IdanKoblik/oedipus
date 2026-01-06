#include <gtest/gtest.h>
#include "terminal.h"

TEST(TerminalTest, WriteStr_DoesNotCrash) {
    EXPECT_NO_THROW(writeStr(""));
    EXPECT_NO_THROW(writeStr("test"));
    EXPECT_NO_THROW(writeStr("Hello World\n"));
    EXPECT_NO_THROW(writeStr(std::string(1000, 'a')));
}

TEST(TerminalTest, Clear_DoesNotCrash) {
    EXPECT_NO_THROW(clear());
}

TEST(TerminalTest, WindowSize_ReturnsValidWindow) {
    try {
        Window w = windowSize();
        EXPECT_GT(w.rows, 0);
        EXPECT_GT(w.cols, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Not running in a terminal environment";
    }
}