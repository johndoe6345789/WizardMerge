/**
 * @file test_git_cli.cpp
 * @brief Unit tests for Git CLI wrapper functionality
 */

#include "wizardmerge/git/git_cli.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

using namespace wizardmerge::git;
namespace fs = std::filesystem;

class GitCLITest : public ::testing::Test {
protected:
    std::string test_dir;
    
    void SetUp() override {
        // Create temporary test directory using std::filesystem
        std::filesystem::path temp_base = std::filesystem::temp_directory_path();
        test_dir = (temp_base / ("wizardmerge_git_test_" + std::to_string(time(nullptr)))).string();
        fs::create_directories(test_dir);
    }
    
    void TearDown() override {
        // Clean up test directory
        if (fs::exists(test_dir)) {
            fs::remove_all(test_dir);
        }
    }
    
    // Helper to initialize a git repo
    void init_repo(const std::string& path) {
        system(("git init \"" + path + "\" 2>&1 > /dev/null").c_str());
        system(("git -C \"" + path + "\" config user.name \"Test User\"").c_str());
        system(("git -C \"" + path + "\" config user.email \"test@example.com\"").c_str());
    }
    
    // Helper to create a file
    void create_file(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        file << content;
        file.close();
    }
};

/**
 * Test Git availability check
 */
TEST_F(GitCLITest, GitAvailability) {
    // Git should be available in CI environment
    EXPECT_TRUE(is_git_available());
}

/**
 * Test branch existence check
 */
TEST_F(GitCLITest, BranchExists) {
    std::string repo_path = test_dir + "/test_repo";
    init_repo(repo_path);
    
    // Create initial commit (required for branch operations)
    create_file(repo_path + "/test.txt", "initial content");
    system(("git -C \"" + repo_path + "\" add test.txt 2>&1 > /dev/null").c_str());
    system(("git -C \"" + repo_path + "\" commit -m \"Initial commit\" 2>&1 > /dev/null").c_str());
    
    // Default branch should exist (main or master)
    auto current_branch = get_current_branch(repo_path);
    ASSERT_TRUE(current_branch.has_value());
    EXPECT_TRUE(branch_exists(repo_path, current_branch.value()));
    
    // Non-existent branch should not exist
    EXPECT_FALSE(branch_exists(repo_path, "nonexistent-branch"));
}

/**
 * Test getting current branch
 */
TEST_F(GitCLITest, GetCurrentBranch) {
    std::string repo_path = test_dir + "/test_repo";
    init_repo(repo_path);
    
    // Create initial commit
    create_file(repo_path + "/test.txt", "initial content");
    system(("git -C \"" + repo_path + "\" add test.txt 2>&1 > /dev/null").c_str());
    system(("git -C \"" + repo_path + "\" commit -m \"Initial commit\" 2>&1 > /dev/null").c_str());
    
    auto branch = get_current_branch(repo_path);
    ASSERT_TRUE(branch.has_value());
    // Should be either "main" or "master" depending on Git version
    EXPECT_TRUE(branch.value() == "main" || branch.value() == "master");
}

/**
 * Test creating a new branch
 */
TEST_F(GitCLITest, CreateBranch) {
    std::string repo_path = test_dir + "/test_repo";
    init_repo(repo_path);
    
    // Create initial commit
    create_file(repo_path + "/test.txt", "initial content");
    system(("git -C \"" + repo_path + "\" add test.txt 2>&1 > /dev/null").c_str());
    system(("git -C \"" + repo_path + "\" commit -m \"Initial commit\" 2>&1 > /dev/null").c_str());
    
    // Create new branch
    GitResult result = create_branch(repo_path, "test-branch");
    EXPECT_TRUE(result.success) << "Error: " << result.error;
    
    // Verify we're on the new branch
    auto current_branch = get_current_branch(repo_path);
    ASSERT_TRUE(current_branch.has_value());
    EXPECT_EQ(current_branch.value(), "test-branch");
    
    // Verify branch exists
    EXPECT_TRUE(branch_exists(repo_path, "test-branch"));
}

/**
 * Test adding files
 */
TEST_F(GitCLITest, AddFiles) {
    std::string repo_path = test_dir + "/test_repo";
    init_repo(repo_path);
    
    // Create test files
    create_file(repo_path + "/file1.txt", "content1");
    create_file(repo_path + "/file2.txt", "content2");
    
    // Add files
    GitResult result = add_files(repo_path, {"file1.txt", "file2.txt"});
    EXPECT_TRUE(result.success) << "Error: " << result.error;
}

/**
 * Test committing changes
 */
TEST_F(GitCLITest, Commit) {
    std::string repo_path = test_dir + "/test_repo";
    init_repo(repo_path);
    
    // Create and add a file
    create_file(repo_path + "/test.txt", "content");
    add_files(repo_path, {"test.txt"});
    
    // Commit
    GitConfig config;
    config.user_name = "Test User";
    config.user_email = "test@example.com";
    
    GitResult result = commit(repo_path, "Test commit", config);
    EXPECT_TRUE(result.success) << "Error: " << result.error;
}

/**
 * Test repository status
 */
TEST_F(GitCLITest, Status) {
    std::string repo_path = test_dir + "/test_repo";
    init_repo(repo_path);
    
    GitResult result = status(repo_path);
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.output.empty());
}

/**
 * Test checkout branch
 */
TEST_F(GitCLITest, CheckoutBranch) {
    std::string repo_path = test_dir + "/test_repo";
    init_repo(repo_path);
    
    // Create initial commit
    create_file(repo_path + "/test.txt", "initial content");
    system(("git -C \"" + repo_path + "\" add test.txt 2>&1 > /dev/null").c_str());
    system(("git -C \"" + repo_path + "\" commit -m \"Initial commit\" 2>&1 > /dev/null").c_str());
    
    // Create and switch to new branch
    create_branch(repo_path, "test-branch");
    
    // Get original branch
    auto original_branch = get_current_branch(repo_path);
    system(("git -C \"" + repo_path + "\" checkout " + original_branch.value() + " 2>&1 > /dev/null").c_str());
    
    // Checkout the test branch
    GitResult result = checkout_branch(repo_path, "test-branch");
    EXPECT_TRUE(result.success) << "Error: " << result.error;
    
    // Verify we're on test-branch
    auto current_branch = get_current_branch(repo_path);
    ASSERT_TRUE(current_branch.has_value());
    EXPECT_EQ(current_branch.value(), "test-branch");
}

/**
 * Test empty file list
 */
TEST_F(GitCLITest, AddEmptyFileList) {
    std::string repo_path = test_dir + "/test_repo";
    init_repo(repo_path);
    
    // Add empty file list should succeed without error
    GitResult result = add_files(repo_path, {});
    EXPECT_TRUE(result.success);
}
