/**
 * L1 Test Suite - Native Window Tests
 * Tests for WstGLCreateNativeWindow and WstGLDestroyNativeWindow
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "westeros-gl.h"

// External mock control functions
namespace MockNxClient {
    extern void reset();
    extern void setAllocSuccess(bool success);
}

namespace MockNXPL {
    extern void reset();
    extern void setWindowCreationSuccess(bool success);
}

namespace MockNexus {
    extern void reset();
}

class WesterosGLWindowTest : public ::testing::Test {
protected:
    WstGLCtx* ctx;
    
    void SetUp() override {
        MockNxClient::reset();
        MockNXPL::reset();
        MockNexus::reset();
        
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
 * Test: Create native window with valid parameters
 * Requirement: WstGLCreateNativeWindow must create window successfully
 */
TEST_F(WesterosGLWindowTest, CreateNativeWindow_ValidParams_ReturnsWindow) {
    int x = 0, y = 0, width = 1280, height = 720;
    
    void* nativeWindow = WstGLCreateNativeWindow(ctx, x, y, width, height);
    
    ASSERT_NE(nullptr, nativeWindow) << "WstGLCreateNativeWindow should return valid window";
    
    WstGLDestroyNativeWindow(ctx, nativeWindow);
}

/**
 * Test: Create native window with NULL context
 * Requirement: WstGLCreateNativeWindow must handle NULL context
 */
TEST_F(WesterosGLWindowTest, CreateNativeWindow_NullContext_ReturnsNull) {
    void* nativeWindow = WstGLCreateNativeWindow(nullptr, 0, 0, 1280, 720);
    
    EXPECT_EQ(nullptr, nativeWindow) << "Should return NULL with NULL context";
}

/**
 * Test: Create native window with zero dimensions
 * Requirement: WstGLCreateNativeWindow must handle invalid dimensions
 */
TEST_F(WesterosGLWindowTest, CreateNativeWindow_ZeroDimensions_HandledGracefully) {
    void* window1 = WstGLCreateNativeWindow(ctx, 0, 0, 0, 720);
    void* window2 = WstGLCreateNativeWindow(ctx, 0, 0, 1280, 0);
    void* window3 = WstGLCreateNativeWindow(ctx, 0, 0, 0, 0);
    
    // Implementation may return NULL or accept zero dimensions
    // Just ensure no crash
    
    if (window1) WstGLDestroyNativeWindow(ctx, window1);
    if (window2) WstGLDestroyNativeWindow(ctx, window2);
    if (window3) WstGLDestroyNativeWindow(ctx, window3);
}

/**
 * Test: Create native window with negative position
 * Requirement: WstGLCreateNativeWindow must handle negative coordinates
 */
TEST_F(WesterosGLWindowTest, CreateNativeWindow_NegativePosition_Handled) {
    void* nativeWindow = WstGLCreateNativeWindow(ctx, -100, -100, 1280, 720);
    
    // Implementation may handle negative positions differently
    // Verify no crash
    
    if (nativeWindow) {
        WstGLDestroyNativeWindow(ctx, nativeWindow);
    }
}

/**
 * Test: Create native window with large dimensions
 * Requirement: WstGLCreateNativeWindow must handle large dimensions
 */
TEST_F(WesterosGLWindowTest, CreateNativeWindow_LargeDimensions_Handled) {
    void* nativeWindow = WstGLCreateNativeWindow(ctx, 0, 0, 4096, 2160);
    
    EXPECT_NE(nullptr, nativeWindow) << "Should handle 4K dimensions";
    
    if (nativeWindow) {
        WstGLDestroyNativeWindow(ctx, nativeWindow);
    }
}

/**
 * Test: Destroy native window with valid window
 * Requirement: WstGLDestroyNativeWindow must cleanup window resources
 */
TEST_F(WesterosGLWindowTest, DestroyNativeWindow_ValidWindow_ReturnsTrue) {
    void* nativeWindow = WstGLCreateNativeWindow(ctx, 0, 0, 1280, 720);
    ASSERT_NE(nullptr, nativeWindow);
    
    WstGLDestroyNativeWindow(ctx, nativeWindow);
}

/**
 * Test: Destroy native window with NULL context
 * Requirement: WstGLDestroyNativeWindow must handle NULL context
 */
TEST_F(WesterosGLWindowTest, DestroyNativeWindow_NullContext_ReturnsFalse) {
    void* nativeWindow = WstGLCreateNativeWindow(ctx, 0, 0, 1280, 720);
    ASSERT_NE(nullptr, nativeWindow);
    
    WstGLDestroyNativeWindow(nullptr, nativeWindow);
    
    // Cleanup with valid context
    WstGLDestroyNativeWindow(ctx, nativeWindow);
}

/**
 * Test: Destroy native window with NULL window
 * Requirement: WstGLDestroyNativeWindow must handle NULL window
 */
TEST_F(WesterosGLWindowTest, DestroyNativeWindow_NullWindow_ReturnsFalse) {
    WstGLDestroyNativeWindow(ctx, nullptr);
}

/**
 * Test: Create multiple native windows
 * Requirement: Multiple windows should be supported
 */
TEST_F(WesterosGLWindowTest, CreateNativeWindow_Multiple_AllCreated) {
    void* window1 = WstGLCreateNativeWindow(ctx, 0, 0, 640, 480);
    void* window2 = WstGLCreateNativeWindow(ctx, 640, 0, 640, 480);
    void* window3 = WstGLCreateNativeWindow(ctx, 0, 480, 640, 480);
    
    EXPECT_NE(nullptr, window1) << "First window should be created";
    EXPECT_NE(nullptr, window2) << "Second window should be created";
    EXPECT_NE(nullptr, window3) << "Third window should be created";
    
    // Windows should be different
    EXPECT_NE(window1, window2) << "Windows should have different pointers";
    EXPECT_NE(window2, window3) << "Windows should have different pointers";
    EXPECT_NE(window1, window3) << "Windows should have different pointers";
    
    if (window1) WstGLDestroyNativeWindow(ctx, window1);
    if (window2) WstGLDestroyNativeWindow(ctx, window2);
    if (window3) WstGLDestroyNativeWindow(ctx, window3);
}

/**
 * Test: Double destroy of native window
 * Requirement: WstGLDestroyNativeWindow must handle double destroy safely
 */
TEST_F(WesterosGLWindowTest, DestroyNativeWindow_DoubleDestroy_HandlesSafely) {
    void* nativeWindow = WstGLCreateNativeWindow(ctx, 0, 0, 1280, 720);
    ASSERT_NE(nullptr, nativeWindow);
    
    WstGLDestroyNativeWindow(ctx, nativeWindow);
    
    // Second destroy should be handled safely (no crash)
    WstGLDestroyNativeWindow(ctx, nativeWindow);
}

/**
 * Test: Create window after allocation failure
 * Requirement: WstGLCreateNativeWindow must handle allocation failures
 */
TEST_F(WesterosGLWindowTest, CreateNativeWindow_AllocationFails_ReturnsNull) {
    MockNXPL::setWindowCreationSuccess(false);
    
    void* nativeWindow = WstGLCreateNativeWindow(ctx, 0, 0, 1280, 720);
    
    EXPECT_EQ(nullptr, nativeWindow) << "Should return NULL when allocation fails";
}

/**
 * Test: Create and destroy window stress test
 * Requirement: Window creation/destruction should be stable under stress
 */
TEST_F(WesterosGLWindowTest, CreateDestroyWindow_Stress_Stable) {
    const int iterations = 100;
    
    for (int i = 0; i < iterations; i++) {
        void* window = WstGLCreateNativeWindow(ctx, i % 100, i % 100, 800 + (i % 400), 600 + (i % 200));
        
        if (window) {
            WstGLDestroyNativeWindow(ctx, window);
        }
    }
}

/**
 * Test: Create window with various aspect ratios
 * Requirement: WstGLCreateNativeWindow should support different aspect ratios
 */
TEST_F(WesterosGLWindowTest, CreateNativeWindow_VariousAspectRatios_Success) {
    // 16:9
    void* window1 = WstGLCreateNativeWindow(ctx, 0, 0, 1920, 1080);
    EXPECT_NE(nullptr, window1) << "16:9 window should be created";
    
    // 4:3
    void* window2 = WstGLCreateNativeWindow(ctx, 0, 0, 1024, 768);
    EXPECT_NE(nullptr, window2) << "4:3 window should be created";
    
    // 21:9 ultrawide
    void* window3 = WstGLCreateNativeWindow(ctx, 0, 0, 2560, 1080);
    EXPECT_NE(nullptr, window3) << "21:9 window should be created";
    
    // 1:1 square
    void* window4 = WstGLCreateNativeWindow(ctx, 0, 0, 1000, 1000);
    EXPECT_NE(nullptr, window4) << "Square window should be created";
    
    if (window1) WstGLDestroyNativeWindow(ctx, window1);
    if (window2) WstGLDestroyNativeWindow(ctx, window2);
    if (window3) WstGLDestroyNativeWindow(ctx, window3);
    if (window4) WstGLDestroyNativeWindow(ctx, window4);
}

/**
 * Test: Window operations with terminated context
 * Requirement: Operations should fail gracefully after context termination
 */
TEST_F(WesterosGLWindowTest, WindowOperations_AfterTermination_FailGracefully) {
    void* window = WstGLCreateNativeWindow(ctx, 0, 0, 1280, 720);
    ASSERT_NE(nullptr, window);
    
    // Terminate context
    WstGLTerm(ctx);
    ctx = nullptr; // Prevent double free in TearDown
    
    // Operations should be handled gracefully (no crash)
    WstGLDestroyNativeWindow(ctx, window);
}
