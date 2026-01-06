#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <cstdio>
#include "config/config.h"
#include "editor.h"

using namespace config;

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        char tmpl[] = "/tmp/cake_test_XXXXXX";
        const int fd = mkstemp(tmpl);
        ASSERT_NE(fd, -1);

        close(fd);
        test_file = tmpl;
    }

    void TearDown() override {
        if (std::filesystem::exists(test_file)) {
            std::filesystem::remove(test_file);
        }
    }

    std::string test_file;
};

TEST_F(ConfigTest, DefaultConstructor) {
    Config cfg;
    EXPECT_EQ(cfg.keybindings[UNDO].key, UNDO);
    EXPECT_EQ(cfg.keybindings[UNDO].shortcut, 'u');
    EXPECT_EQ(cfg.keybindings[REDO].key, REDO);
    EXPECT_EQ(cfg.keybindings[REDO].shortcut, 'r');
    EXPECT_EQ(cfg.keybindings[SEARCH_MOVE].key, SEARCH_MOVE);
    EXPECT_EQ(cfg.keybindings[SEARCH_MOVE].shortcut, '/');
    EXPECT_EQ(cfg.settings[TAB].setting, TAB);
    EXPECT_EQ(cfg.settings[TAB].value, 4);
}

TEST_F(ConfigTest, LoadConfig_FileNotFound) {
    Config cfg;
    EXPECT_THROW(load_config("/nonexistent/path/config.ini", cfg), std::runtime_error);
}

TEST_F(ConfigTest, LoadConfig_EmptyFile) {
    std::ofstream file(test_file);
    file.close();

    Config cfg;
    EXPECT_NO_THROW(load_config(test_file, cfg));
    EXPECT_EQ(cfg.keybindings[UNDO].shortcut, 'u');
}

TEST_F(ConfigTest, LoadConfig_Keybindings) {
    std::ofstream file(test_file);
    file << "[KEYBINDING]\n";
    file << "UNDO = x\n";
    file << "REDO = y\n";
    file << "SEARCH_MOVE = z\n";
    file.close();

    Config cfg;
    load_config(test_file, cfg);

    EXPECT_EQ(cfg.keybindings[UNDO].shortcut, 'x');
    EXPECT_EQ(cfg.keybindings[REDO].shortcut, 'y');
    EXPECT_EQ(cfg.keybindings[SEARCH_MOVE].shortcut, 'z');
}

TEST_F(ConfigTest, LoadConfig_Options) {
    std::ofstream file(test_file);
    file << "[OPTIONS]\n";
    file << "TAB = 8\n";
    file.close();

    Config cfg;
    load_config(test_file, cfg);

    EXPECT_EQ(cfg.settings[TAB].value, 8);
}

TEST_F(ConfigTest, LoadConfig_WithComments) {
    std::ofstream file(test_file);
    file << "# This is a comment\n";
    file << "[KEYBINDING]\n";
    file << "# Another comment\n";
    file << "UNDO = a\n";
    file.close();

    Config cfg;
    load_config(test_file, cfg);

    EXPECT_EQ(cfg.keybindings[UNDO].shortcut, 'a');
}

TEST_F(ConfigTest, LoadConfig_WithWhitespace) {
    std::ofstream file(test_file);
    file << "  [KEYBINDING]  \n";
    file << "  UNDO  =  b  \n";
    file.close();

    Config cfg;
    load_config(test_file, cfg);

    EXPECT_EQ(cfg.keybindings[UNDO].shortcut, 'b');
}

TEST_F(ConfigTest, LoadConfig_LeaderPrefix) {
    std::ofstream file(test_file);
    file << "[KEYBINDING]\n";
    file << "UNDO = $leader,u\n";
    file.close();

    Config cfg;
    load_config(test_file, cfg);

    // CTRL_KEY('u') should be 0x15 (21)
    EXPECT_EQ(cfg.keybindings[UNDO].shortcut, CTRL_KEY('u'));
}

TEST_F(ConfigTest, LoadConfig_MultipleSections) {
    std::ofstream file(test_file);
    file << "[KEYBINDING]\n";
    file << "UNDO = c\n";
    file << "[OPTIONS]\n";
    file << "TAB = 2\n";
    file.close();

    Config cfg;
    load_config(test_file, cfg);

    EXPECT_EQ(cfg.keybindings[UNDO].shortcut, 'c');
    EXPECT_EQ(cfg.settings[TAB].value, 2);
}

TEST_F(ConfigTest, LoadConfig_InvalidKeyIgnored) {
    std::ofstream file(test_file);
    file << "[KEYBINDING]\n";
    file << "INVALID_KEY = x\n";
    file << "UNDO = d\n";
    file.close();

    Config cfg;
    load_config(test_file, cfg);

    EXPECT_EQ(cfg.keybindings[UNDO].shortcut, 'd');
}

TEST_F(ConfigTest, LoadConfig_EmptyLinesIgnored) {
    std::ofstream file(test_file);
    file << "\n";
    file << "[KEYBINDING]\n";
    file << "\n";
    file << "UNDO = e\n";
    file << "\n";
    file.close();

    Config cfg;
    load_config(test_file, cfg);

    EXPECT_EQ(cfg.keybindings[UNDO].shortcut, 'e');
}

TEST_F(ConfigTest, LoadConfig_NoEqualsSignIgnored) {
    std::ofstream file(test_file);
    file << "[KEYBINDING]\n";
    file << "UNDO e\n";
    file << "REDO = f\n";
    file.close();

    Config cfg;
    load_config(test_file, cfg);

    EXPECT_EQ(cfg.keybindings[REDO].shortcut, 'f');
}

TEST_F(ConfigTest, LoadConfig_EmptyValue) {
    std::ofstream file(test_file);
    file << "[KEYBINDING]\n";
    file << "UNDO =\n";
    file.close();

    Config cfg;
    load_config(test_file, cfg);

    // Should handle empty value gracefully
    EXPECT_EQ(cfg.keybindings[UNDO].shortcut, 0);
}

TEST_F(ConfigTest, LoadConfig_OnlyFirstCharUsed) {
    std::ofstream file(test_file);
    file << "[KEYBINDING]\n";
    file << "UNDO = hello\n";
    file.close();

    Config cfg;
    load_config(test_file, cfg);

    EXPECT_EQ(cfg.keybindings[UNDO].shortcut, 'h');
}

