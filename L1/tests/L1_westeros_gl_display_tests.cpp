/**
 * L1 Test Suite - Display Information and Configuration Tests
 * Tests for display get/set operations and listeners
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "westeros-gl.h"
#include <cstring>

// External mock control functions
namespace MockNxClient {
    extern void reset();
    extern void setDisplaySize(unsigned width, unsigned height);
}

namespace MockNexus {
    extern void reset();
}

// Test display size callback
static bool g_callbackInvoked = false;
static void* g_callbackUserData = nullptr;

void testDisplaySizeCallback(void* userData, int width, int height) {
    g_callbackInvoked = true;
    g_callbackUserData = userData;
}

class WesterosGLDisplayTest : public ::testing::Test {
protected:
    WstGLCtx* ctx;
    
    void SetUp() override {
        MockNxClient::reset();
        MockNexus::reset();
        g_callbackInvoked = false;
        g_callbackUserData = nullptr;
        
        ctx = WstGLInit();
        ASSERT_NE(nullptr, ctx) << "Failed to initialize context for test";
    }
    
    void TearDown() override {
        if (ctx) {
            WstGLTerm(ctx);
        }
    }
};

/**
 * Test: Get display information with valid context
 * Requirement: WstGLGetDisplayInfo must return display information
 */
TEST_F(WesterosGLDisplayTest, GetDisplayInfo_ValidContext_ReturnsInfo) {
    WstGLDisplayInfo displayInfo;
    
    bool result = WstGLGetDisplayInfo(ctx, &displayInfo);
    
    EXPECT_TRUE(result) << "WstGLGetDisplayInfo should succeed";
    EXPECT_GT(displayInfo.width, 0) << "Display width should be positive";
    EXPECT_GT(displayInfo.height, 0) << "Display height should be positive";
}

/**
 * Test: Get display info with NULL context
 * Requirement: WstGLGetDisplayInfo must handle NULL context
 */
TEST_F(WesterosGLDisplayTest, GetDisplayInfo_NullContext_ReturnsFalse) {
    WstGLDisplayInfo displayInfo;
    
    bool result = WstGLGetDisplayInfo(nullptr, &displayInfo);
    
    EXPECT_FALSE(result) << "WstGLGetDisplayInfo should fail with NULL context";
}

/**
 * Test: Get display info with NULL output parameters
 * Requirement: WstGLGetDisplayInfo must handle NULL output pointers
 */
TEST_F(WesterosGLDisplayTest, GetDisplayInfo_NullOutputs_ReturnsFalse) {
    bool result = WstGLGetDisplayInfo(ctx, nullptr);
    EXPECT_FALSE(result) << "Should fail with NULL displayInfo pointer";
}

/**
 * Test: Get display safe area with valid context
 * Requirement: WstGLGetDisplaySafeArea must return safe area coordinates
 */
TEST_F(WesterosGLDisplayTest, GetDisplaySafeArea_ValidContext_ReturnsArea) {
    int x = -1, y = -1, width = 0, height = 0;
    
    bool result = WstGLGetDisplaySafeArea(ctx, &x, &y, &width, &height);
    
    EXPECT_TRUE(result) << "WstGLGetDisplaySafeArea should succeed";
    EXPECT_GE(x, 0) << "Safe area X should be non-negative";
    EXPECT_GE(y, 0) << "Safe area Y should be non-negative";
    EXPECT_GT(width, 0) << "Safe area width should be positive";
    EXPECT_GT(height, 0) << "Safe area height should be positive";
}

/**
 * Test: Get display safe area with NULL context
 * Requirement: WstGLGetDisplaySafeArea must handle NULL context
 */
TEST_F(WesterosGLDisplayTest, GetDisplaySafeArea_NullContext_ReturnsFalse) {
    int x, y, width, height;
    
    bool result = WstGLGetDisplaySafeArea(nullptr, &x, &y, &width, &height);
    
    EXPECT_FALSE(result) << "Should fail with NULL context";
}

/**
 * Test: Get display safe area with NULL output parameters
 * Requirement: WstGLGetDisplaySafeArea must handle NULL outputs
 */
TEST_F(WesterosGLDisplayTest, GetDisplaySafeArea_NullOutputs_ReturnsFalse) {
    bool result = WstGLGetDisplaySafeArea(ctx, nullptr, nullptr, nullptr, nullptr);
    
    EXPECT_FALSE(result) << "Should fail with all NULL output pointers";
}

/**
 * Test: Add display size listener with valid callback
 * Requirement: WstGLAddDisplaySizeListener must register callback
 */
TEST_F(WesterosGLDisplayTest, AddDisplaySizeListener_ValidCallback_ReturnsTrue) {
    void* userData = (void*)0x12345;
    
    bool result = WstGLAddDisplaySizeListener(ctx, userData, testDisplaySizeCallback);
    
    EXPECT_TRUE(result) << "WstGLAddDisplaySizeListener should succeed";
}

/**
 * Test: Add display size listener with NULL context
 * Requirement: WstGLAddDisplaySizeListener must handle NULL context
 */
TEST_F(WesterosGLDisplayTest, AddDisplaySizeListener_NullContext_ReturnsFalse) {
    bool result = WstGLAddDisplaySizeListener(nullptr, nullptr, testDisplaySizeCallback);
    
    EXPECT_FALSE(result) << "Should fail with NULL context";
}

/**
 * Test: Add display size listener with NULL callback
 * Requirement: WstGLAddDisplaySizeListener must handle NULL callback
 */
TEST_F(WesterosGLDisplayTest, AddDisplaySizeListener_NullCallback_ReturnsFalse) {
    bool result = WstGLAddDisplaySizeListener(ctx, nullptr, nullptr);
    
    EXPECT_FALSE(result) << "Should fail with NULL callback";
}

/**
 * Test: Remove display size listener
 * Requirement: WstGLRemoveDisplaySizeListener must unregister callback
 */
TEST_F(WesterosGLDisplayTest, RemoveDisplaySizeListener_ValidCallback_ReturnsTrue) {
    void* userData = (void*)0x12345;
    
    bool addResult = WstGLAddDisplaySizeListener(ctx, userData, testDisplaySizeCallback);
    ASSERT_TRUE(addResult) << "Failed to add listener";
    
    bool removeResult = WstGLRemoveDisplaySizeListener(ctx, testDisplaySizeCallback);
    
    EXPECT_TRUE(removeResult) << "WstGLRemoveDisplaySizeListener should succeed";
}

/**
 * Test: Remove display size listener with NULL context
 * Requirement: WstGLRemoveDisplaySizeListener must handle NULL context
 */
TEST_F(WesterosGLDisplayTest, RemoveDisplaySizeListener_NullContext_ReturnsFalse) {
    bool result = WstGLRemoveDisplaySizeListener(nullptr, testDisplaySizeCallback);
    
    EXPECT_FALSE(result) << "Should fail with NULL context";
}

/**
 * Test: Remove display size listener that was not added
 * Requirement: WstGLRemoveDisplaySizeListener must handle non-existent callback
 */
TEST_F(WesterosGLDisplayTest, RemoveDisplaySizeListener_NotAdded_ReturnsFalse) {
    bool result = WstGLRemoveDisplaySizeListener(ctx, testDisplaySizeCallback);
    
    EXPECT_FALSE(result) << "Should fail when removing non-existent listener";
}

/**
 * Test: Multiple display size listeners
 * Requirement: Multiple listeners should be supported
 */
TEST_F(WesterosGLDisplayTest, AddDisplaySizeListener_Multiple_AllRegistered) {
    void* userData1 = (void*)0x1111;
    void* userData2 = (void*)0x2222;
    
    bool result1 = WstGLAddDisplaySizeListener(ctx, userData1, testDisplaySizeCallback);
    bool result2 = WstGLAddDisplaySizeListener(ctx, userData2, testDisplaySizeCallback);
    
    EXPECT_TRUE(result1) << "First listener should be added";
    EXPECT_TRUE(result2) << "Second listener should be added";
    
    WstGLRemoveDisplaySizeListener(ctx, testDisplaySizeCallback);
    WstGLRemoveDisplaySizeListener(ctx, testDisplaySizeCallback);
}

/**
 * Test: Display size changes
 * Requirement: Verify display size can be queried correctly
 */
TEST_F(WesterosGLDisplayTest, GetDisplayInfo_DifferentSizes_ReturnsCorrectly) {
    // Set different display sizes and verify
    MockNxClient::setDisplaySize(1920, 1080);
    
    WstGLDisplayInfo displayInfo1;
    bool result1 = WstGLGetDisplayInfo(ctx, &displayInfo1);
    
    EXPECT_TRUE(result1);
    EXPECT_EQ(1920, displayInfo1.width) << "Width should match mock setting";
    EXPECT_EQ(1080, displayInfo1.height) << "Height should match mock setting";
    
    // Change size
    MockNxClient::setDisplaySize(1280, 720);
    
    // Create new context to pick up the change
    WstGLCtx* ctx2 = WstGLInit();
    ASSERT_NE(nullptr, ctx2);
    
    WstGLDisplayInfo displayInfo2;
    bool result2 = WstGLGetDisplayInfo(ctx2, &displayInfo2);
    
    EXPECT_TRUE(result2);
    EXPECT_EQ(1280, displayInfo2.width) << "Width should match new mock setting";
    EXPECT_EQ(720, displayInfo2.height) << "Height should match new mock setting";
    
    WstGLTerm(ctx2);
}

/**
 * Test: Safe area within display bounds
 * Requirement: Safe area should be within display dimensions
 */
TEST_F(WesterosGLDisplayTest, GetDisplaySafeArea_WithinDisplayBounds) {
    WstGLDisplayInfo displayInfo;
    bool displayResult = WstGLGetDisplayInfo(ctx, &displayInfo);
    ASSERT_TRUE(displayResult);
    
    int safeX, safeY, safeWidth, safeHeight;
    bool safeResult = WstGLGetDisplaySafeArea(ctx, &safeX, &safeY, &safeWidth, &safeHeight);
    ASSERT_TRUE(safeResult);
    
    EXPECT_LE(safeX + safeWidth, displayInfo.width) << "Safe area should not exceed display width";
    EXPECT_LE(safeY + safeHeight, displayInfo.height) << "Safe area should not exceed display height";
}
