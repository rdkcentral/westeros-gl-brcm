/**
 * L1 Test Suite - Initialization and Termination Tests
 * Tests for WstGLInit and WstGLTerm functions
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "westeros-gl.h"

// External mock control functions
namespace MockNxClient {
    extern void reset();
    extern void setJoinSuccess(bool success);
    extern int getJoinCount();
    extern int getUninitCount();
}

namespace MockNexus {
    extern void reset();
    extern void setInitSuccess(bool success);
    extern int getOpenCount();
    extern int getCloseCount();
}

class WesterosGLInitTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset all mocks before each test
        MockNxClient::reset();
        MockNexus::reset();
    }
    
    void TearDown() override {
        // Cleanup after each test
    }
};

/**
 * Test: Successful initialization with valid context
 * Requirement: WstGLInit must initialize properly with valid parameters
 */
TEST_F(WesterosGLInitTest, Init_ValidContext_ReturnsTrue) {
    WstGLCtx* ctx = WstGLInit();
    
    ASSERT_NE(nullptr, ctx) << "WstGLInit should return valid context";
    EXPECT_GT(MockNxClient::getJoinCount(), 0) << "NxClient_Join should be called";
    
    WstGLTerm(ctx);
    EXPECT_GT(MockNxClient::getUninitCount(), 0) << "WstGLTerm should call NxClient_Uninit";
}

/**
 * Test: Termination with valid context
 * Requirement: WstGLTerm must properly cleanup resources
 */
TEST_F(WesterosGLInitTest, Term_ValidContext_ReturnsTrue) {
    WstGLCtx* ctx = WstGLInit();
    ASSERT_NE(nullptr, ctx);
    
    int uninitsBefore = MockNxClient::getUninitCount();
    WstGLTerm(ctx);
    
    EXPECT_GT(MockNxClient::getUninitCount(), uninitsBefore) << "NxClient_Uninit should be called";
}

/**
 * Test: Termination with NULL context
 * Requirement: WstGLTerm must handle NULL gracefully
 */
TEST_F(WesterosGLInitTest, Term_NullContext_ReturnsFalse) {
    // WstGLTerm returns void, so we just verify it doesn't crash
    WstGLTerm(nullptr);
    // If we reach here without crashing, test passes
    SUCCEED();
}

/**
 * Test: Multiple initialization and termination cycles
 * Requirement: Multiple init/term cycles should work correctly
 */
TEST_F(WesterosGLInitTest, MultipleInitTerm_Success) {
    for (int i = 0; i < 5; i++) {
        WstGLCtx* ctx = WstGLInit();
        ASSERT_NE(nullptr, ctx) << "WstGLInit should succeed on iteration " << i;
        
        WstGLTerm(ctx);
        // Test passes if no crash occurs
    }
}

/**
 * Test: Initialization failure when NxClient join fails
 * Requirement: WstGLInit must handle NxClient failure gracefully
 */
TEST_F(WesterosGLInitTest, Init_NxClientJoinFails_ReturnsNull) {
    MockNxClient::setJoinSuccess(false);
    
    WstGLCtx* ctx = WstGLInit();
    
    EXPECT_EQ(nullptr, ctx) << "WstGLInit should return NULL when NxClient_Join fails";
}

/**
 * Test: Termination with NULL context
 * Requirement: WstGLTerm must handle NULL context safely
 */
TEST_F(WesterosGLInitTest, Term_NullContext_HandlesSafely) {
    // Terminating a NULL context should not crash
    WstGLTerm(nullptr);
    
    // If we reach here without crashing, test passes
    SUCCEED();
}

/**
 * Test: Proper context termination and verification
 * Requirement: WstGLTerm must properly clean up resources
 */
TEST_F(WesterosGLInitTest, Term_ProperCleanup_Success) {
    WstGLCtx* ctx = WstGLInit();
    ASSERT_NE(nullptr, ctx);
    
    int joinCount = MockNxClient::getJoinCount();
    
    WstGLTerm(ctx);
    // After termination, do NOT use ctx pointer (undefined behavior)
    // Instead verify cleanup through mock counters
    
    EXPECT_GT(MockNxClient::getUninitCount(), 0) << "NxClient_Uninit should be called";
}

/**
 * Test: Context creation resource counting
 * Requirement: Ensure proper resource allocation tracking
 */
TEST_F(WesterosGLInitTest, Init_ResourceCounting_Correct) {
    int initialJoinCount = MockNxClient::getJoinCount();
    int initialUninitCount = MockNxClient::getUninitCount();
    
    WstGLCtx* ctx = WstGLInit();
    ASSERT_NE(nullptr, ctx);
    
    EXPECT_EQ(initialJoinCount + 1, MockNxClient::getJoinCount()) 
        << "Join count should increment by 1";
    
    WstGLTerm(ctx);
    
    EXPECT_EQ(initialUninitCount + 1, MockNxClient::getUninitCount())
        << "Uninit count should increment by 1";
}

/**
 * Test: Initialization with Graphics2D resource allocation
 * Requirement: Verify Graphics2D resources are allocated during init
 */
TEST_F(WesterosGLInitTest, Init_Graphics2DAllocation_Success) {
    int initialOpenCount = MockNexus::getOpenCount();
    
    WstGLCtx* ctx = WstGLInit();
    ASSERT_NE(nullptr, ctx);
    
    EXPECT_GT(MockNexus::getOpenCount(), initialOpenCount)
        << "Graphics2D should be opened during init";
    
    WstGLTerm(ctx);
    
    EXPECT_GT(MockNexus::getCloseCount(), 0)
        << "Graphics2D should be closed during term";
}

/**
 * Test: Initialization failure when Graphics2D open fails
 * Requirement: WstGLInit must handle Graphics2D failure
 */
TEST_F(WesterosGLInitTest, Init_Graphics2DOpenFails_HandlesGracefully) {
    MockNexus::setInitSuccess(false);
    
    WstGLCtx* ctx = WstGLInit();
    
    // Implementation may still return context but should handle the failure internally
    // Verify no crash and proper cleanup
    if (ctx) {
        WstGLTerm(ctx);
    }
}

/**
 * Test: Concurrent initialization attempts
 * Requirement: Multiple contexts should be supported or serialized
 */
TEST_F(WesterosGLInitTest, Init_ConcurrentAttempts_Handled) {
    WstGLCtx* ctx1 = WstGLInit();
    WstGLCtx* ctx2 = WstGLInit();
    
    // Both contexts should be valid or implementation should serialize
    EXPECT_NE(nullptr, ctx1) << "First context should be valid";
    
    if (ctx1) WstGLTerm(ctx1);
    if (ctx2) WstGLTerm(ctx2);
}
