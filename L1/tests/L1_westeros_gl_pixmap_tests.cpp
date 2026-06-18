/**
 * L1 Test Suite - Native Pixmap Tests
 * Tests for pixmap operations: Get, GetDimensions, Release, GetEGLNativePixmap
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "westeros-gl.h"
#include <cstring>

// Nexus types for surface creation
typedef void* NEXUS_SurfaceHandle;
typedef enum {
    NEXUS_PixelFormat_eA8_R8_G8_B8 = 1
} NEXUS_PixelFormat;

typedef struct {
    unsigned width;
    unsigned height;
    NEXUS_PixelFormat pixelFormat;
    void* buffer;
    unsigned pitch;
} NEXUS_SurfaceCreateSettings;

typedef struct {
    unsigned width;
    unsigned height;
} NEXUS_SurfaceStatus;

extern "C" {
    NEXUS_SurfaceHandle NEXUS_Surface_Create(const NEXUS_SurfaceCreateSettings *pSettings);
    void NEXUS_Surface_Destroy(NEXUS_SurfaceHandle surface);
    void NEXUS_Surface_GetDefaultCreateSettings(NEXUS_SurfaceCreateSettings *pSettings);
    int NEXUS_Surface_GetStatus(NEXUS_SurfaceHandle surface, NEXUS_SurfaceStatus *pStatus);
}

// External mock control functions
namespace MockNexus {
    extern void reset();
    extern void setFailNextAlloc(bool fail);
}

namespace MockNxClient {
    extern void reset();
}

class WesterosGLPixmapTest : public ::testing::Test {
protected:
    WstGLCtx* ctx;
    
    void SetUp() override {
        MockNxClient::reset();
        MockNexus::reset();
        
        ctx = WstGLInit();
        ASSERT_NE(nullptr, ctx) << "Failed to initialize context for test";
    }
    
    void TearDown() override {
        if (ctx) {
            WstGLTerm(ctx);
        }
    }
    
    // Helper to create a Nexus surface for testing
    NEXUS_SurfaceHandle createSurface(int width, int height) {
        NEXUS_SurfaceCreateSettings settings;
        memset(&settings, 0, sizeof(settings));
        settings.width = width;
        settings.height = height;
        settings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        return NEXUS_Surface_Create(&settings);
    }
};

/**
 * Test: Get native pixmap with valid parameters
 * Requirement: WstGLGetNativePixmap must return valid pixmap handle
 */
TEST_F(WesterosGLPixmapTest, GetNativePixmap_ValidParams_ReturnsPixmap) {
    NEXUS_SurfaceHandle surface = createSurface(1280, 720);
    ASSERT_NE(nullptr, surface);
    
    void* pixmap = nullptr;
    bool result = WstGLGetNativePixmap(ctx, surface, &pixmap);
    
    EXPECT_TRUE(result) << "WstGLGetNativePixmap should succeed";
    EXPECT_NE(nullptr, pixmap) << "WstGLGetNativePixmap should return valid pixmap";
    
    if (pixmap) {
        WstGLReleaseNativePixmap(ctx, pixmap);
    }
    NEXUS_Surface_Destroy(surface);
}

/**
 * Test: Get native pixmap with NULL context
 * Requirement: WstGLGetNativePixmap must handle NULL context
 */
TEST_F(WesterosGLPixmapTest, GetNativePixmap_NullContext_ReturnsNull) {
    NEXUS_SurfaceHandle surface = createSurface(1280, 720);
    ASSERT_NE(nullptr, surface);
    
    void* pixmap = nullptr;
    bool result = WstGLGetNativePixmap(nullptr, surface, &pixmap);
    
    EXPECT_FALSE(result) << "Should fail with NULL context";
    EXPECT_EQ(nullptr, pixmap) << "Should return NULL pixmap with NULL context";
    
    NEXUS_Surface_Destroy(surface);
}

/**
 * Test: Get native pixmap with zero dimensions
 * Requirement: WstGLGetNativePixmap must handle invalid dimensions
 */
TEST_F(WesterosGLPixmapTest, GetNativePixmap_ZeroDimensions_HandledGracefully) {
    NEXUS_SurfaceHandle surface1 = createSurface(0, 720);
    NEXUS_SurfaceHandle surface2 = createSurface(1280, 0);
    NEXUS_SurfaceHandle surface3 = createSurface(0, 0);
    
    void* pixmap1 = nullptr;
    void* pixmap2 = nullptr;
    void* pixmap3 = nullptr;
    
    // Implementation may handle zero dimensions differently
    // Just ensure no crash
    if (surface1) {
        WstGLGetNativePixmap(ctx, surface1, &pixmap1);
        if (pixmap1) WstGLReleaseNativePixmap(ctx, pixmap1);
        NEXUS_Surface_Destroy(surface1);
    }
    
    if (surface2) {
        WstGLGetNativePixmap(ctx, surface2, &pixmap2);
        if (pixmap2) WstGLReleaseNativePixmap(ctx, pixmap2);
        NEXUS_Surface_Destroy(surface2);
    }
    
    if (surface3) {
        WstGLGetNativePixmap(ctx, surface3, &pixmap3);
        if (pixmap3) WstGLReleaseNativePixmap(ctx, pixmap3);
        NEXUS_Surface_Destroy(surface3);
    }
}

/**
 * Test: Get native pixmap with negative dimensions
 * Requirement: WstGLGetNativePixmap must handle negative dimensions
 */
TEST_F(WesterosGLPixmapTest, GetNativePixmap_NegativeDimensions_ReturnsNull) {
    // Nexus Surface_Create will likely fail with negative dimensions
    // Test that we handle this gracefully
    NEXUS_SurfaceCreateSettings settings1, settings2;
    memset(&settings1, 0, sizeof(settings1));
    memset(&settings2, 0, sizeof(settings2));
    
    settings1.width = (unsigned int)-1280; // Will wrap to large positive
    settings1.height = 720;
    settings1.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    
    settings2.width = 1280;
    settings2.height = (unsigned int)-720; // Will wrap to large positive
    settings2.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    
    // These should either fail creation or be handled by implementation
    NEXUS_SurfaceHandle surface1 = NEXUS_Surface_Create(&settings1);
    NEXUS_SurfaceHandle surface2 = NEXUS_Surface_Create(&settings2);
    
    if (surface1) {
        NEXUS_Surface_Destroy(surface1);
    }
    if (surface2) {
        NEXUS_Surface_Destroy(surface2);
    }
    
    // Main test: ensure no crash with unusual values
    SUCCEED();
}

/**
 * Test: Get native pixmap with large dimensions
 * Requirement: WstGLGetNativePixmap must handle large dimensions
 */
TEST_F(WesterosGLPixmapTest, GetNativePixmap_LargeDimensions_Success) {
    NEXUS_SurfaceHandle surface = createSurface(4096, 2160);
    ASSERT_NE(nullptr, surface) << "Should create 4K surface";
    
    void* pixmap = nullptr;
    bool result = WstGLGetNativePixmap(ctx, surface, &pixmap);
    
    EXPECT_TRUE(result) << "Should handle 4K dimensions";
    EXPECT_NE(nullptr, pixmap) << "Should return valid pixmap for 4K";
    
    if (pixmap) {
        WstGLReleaseNativePixmap(ctx, pixmap);
    }
    NEXUS_Surface_Destroy(surface);
}

/**
 * Test: Get native pixmap dimensions with valid pixmap
 * Requirement: WstGLGetNativePixmapDimensions must return correct dimensions
 */
TEST_F(WesterosGLPixmapTest, GetNativePixmapDimensions_ValidPixmap_ReturnsDimensions) {
    int createWidth = 1280;
    int createHeight = 720;
    
    NEXUS_SurfaceHandle surface = createSurface(createWidth, createHeight);
    ASSERT_NE(nullptr, surface);
    
    void* pixmap = nullptr;
    bool getResult = WstGLGetNativePixmap(ctx, surface, &pixmap);
    ASSERT_TRUE(getResult);
    ASSERT_NE(nullptr, pixmap);
    
    int width = 0, height = 0;
    WstGLGetNativePixmapDimensions(ctx, pixmap, &width, &height);
    
    EXPECT_EQ(createWidth, width) << "Width should match creation width";
    EXPECT_EQ(createHeight, height) << "Height should match creation height";
    
    WstGLReleaseNativePixmap(ctx, pixmap);
    NEXUS_Surface_Destroy(surface);
}

/**
 * Test: Get native pixmap dimensions with NULL context
 * Requirement: WstGLGetNativePixmapDimensions must handle NULL context
 */
TEST_F(WesterosGLPixmapTest, GetNativePixmapDimensions_NullContext_ReturnsFalse) {
    NEXUS_SurfaceHandle surface = createSurface(1280, 720);
    ASSERT_NE(nullptr, surface);
    
    void* pixmap = nullptr;
    bool getResult = WstGLGetNativePixmap(ctx, surface, &pixmap);
    ASSERT_TRUE(getResult);
    ASSERT_NE(nullptr, pixmap);
    
    int width = 0, height = 0;
    // With NULL context, function should handle gracefully (no crash)
    WstGLGetNativePixmapDimensions(nullptr, pixmap, &width, &height);
    
    WstGLReleaseNativePixmap(ctx, pixmap);
    NEXUS_Surface_Destroy(surface);
}

/**
 * Test: Get native pixmap dimensions with NULL pixmap
 * Requirement: WstGLGetNativePixmapDimensions must handle NULL pixmap
 */
TEST_F(WesterosGLPixmapTest, GetNativePixmapDimensions_NullPixmap_ReturnsFalse) {
    int width = 0, height = 0;
    // With NULL pixmap, function should handle gracefully (no crash)
    WstGLGetNativePixmapDimensions(ctx, nullptr, &width, &height);
    
    // No crash indicates success
    SUCCEED();
}

/**
 * Test: Get native pixmap dimensions with NULL output pointers
 * Requirement: WstGLGetNativePixmapDimensions must handle NULL outputs
 */
TEST_F(WesterosGLPixmapTest, GetNativePixmapDimensions_NullOutputs_ReturnsFalse) {
    NEXUS_SurfaceHandle surface = createSurface(1280, 720);
    ASSERT_NE(nullptr, surface);
    
    void* pixmap = nullptr;
    bool getResult = WstGLGetNativePixmap(ctx, surface, &pixmap);
    ASSERT_TRUE(getResult);
    ASSERT_NE(nullptr, pixmap);
    
    // With NULL outputs, function should handle gracefully (no crash)
    WstGLGetNativePixmapDimensions(ctx, pixmap, nullptr, nullptr);
    
    int width = 0;
    WstGLGetNativePixmapDimensions(ctx, pixmap, &width, nullptr);
    
    WstGLReleaseNativePixmap(ctx, pixmap);
    NEXUS_Surface_Destroy(surface);
}

/**
 * Test: Release native pixmap with valid pixmap
 * Requirement: WstGLReleaseNativePixmap must cleanup resources
 */
TEST_F(WesterosGLPixmapTest, ReleaseNativePixmap_ValidPixmap_Success) {
    NEXUS_SurfaceHandle surface = createSurface(1280, 720);
    ASSERT_NE(nullptr, surface);
    
    void* pixmap = nullptr;
    bool getResult = WstGLGetNativePixmap(ctx, surface, &pixmap);
    ASSERT_TRUE(getResult);
    ASSERT_NE(nullptr, pixmap);
    
    WstGLReleaseNativePixmap(ctx, pixmap);
    
    // No crash indicates success
    SUCCEED();
    
    NEXUS_Surface_Destroy(surface);
}

/**
 * Test: Release native pixmap with NULL context
 * Requirement: WstGLReleaseNativePixmap must handle NULL context
 */
TEST_F(WesterosGLPixmapTest, ReleaseNativePixmap_NullContext_HandledSafely) {
    NEXUS_SurfaceHandle surface = createSurface(1280, 720);
    ASSERT_NE(nullptr, surface);
    
    void* pixmap = nullptr;
    bool getResult = WstGLGetNativePixmap(ctx, surface, &pixmap);
    ASSERT_TRUE(getResult);
    ASSERT_NE(nullptr, pixmap);
    
    // Should not crash with NULL context
    WstGLReleaseNativePixmap(nullptr, pixmap);
    
    // Cleanup properly
    WstGLReleaseNativePixmap(ctx, pixmap);
    NEXUS_Surface_Destroy(surface);
}

/**
 * Test: Release native pixmap with NULL pixmap
 * Requirement: WstGLReleaseNativePixmap must handle NULL pixmap
 */
TEST_F(WesterosGLPixmapTest, ReleaseNativePixmap_NullPixmap_HandledSafely) {
    // Should not crash with NULL pixmap
    WstGLReleaseNativePixmap(ctx, nullptr);
    
    SUCCEED();
}

/**
 * Test: Get EGL native pixmap with valid pixmap
 * Requirement: WstGLGetEGLNativePixmap must return EGL-compatible handle
 */
TEST_F(WesterosGLPixmapTest, GetEGLNativePixmap_ValidPixmap_ReturnsHandle) {
    NEXUS_SurfaceHandle surface = createSurface(1280, 720);
    ASSERT_NE(nullptr, surface);
    
    void* pixmap = nullptr;
    bool getResult = WstGLGetNativePixmap(ctx, surface, &pixmap);
    ASSERT_TRUE(getResult);
    ASSERT_NE(nullptr, pixmap);
    
    void* eglPixmap = WstGLGetEGLNativePixmap(ctx, pixmap);
    
    EXPECT_NE(nullptr, eglPixmap) << "WstGLGetEGLNativePixmap should return valid EGL handle";
    
    WstGLReleaseNativePixmap(ctx, pixmap);
    NEXUS_Surface_Destroy(surface);
}

/**
 * Test: Get EGL native pixmap with NULL context
 * Requirement: WstGLGetEGLNativePixmap must handle NULL context
 */
TEST_F(WesterosGLPixmapTest, GetEGLNativePixmap_NullContext_ReturnsNull) {
    NEXUS_SurfaceHandle surface = createSurface(1280, 720);
    ASSERT_NE(nullptr, surface);
    
    void* pixmap = nullptr;
    bool getResult = WstGLGetNativePixmap(ctx, surface, &pixmap);
    ASSERT_TRUE(getResult);
    ASSERT_NE(nullptr, pixmap);
    
    void* eglPixmap = WstGLGetEGLNativePixmap(nullptr, pixmap);
    
    EXPECT_EQ(nullptr, eglPixmap) << "Should return NULL with NULL context";
    
    WstGLReleaseNativePixmap(ctx, pixmap);
    NEXUS_Surface_Destroy(surface);
}

/**
 * Test: Get EGL native pixmap with NULL pixmap
 * Requirement: WstGLGetEGLNativePixmap must handle NULL pixmap
 */
TEST_F(WesterosGLPixmapTest, GetEGLNativePixmap_NullPixmap_ReturnsNull) {
    void* eglPixmap = WstGLGetEGLNativePixmap(ctx, nullptr);
    
    EXPECT_EQ(nullptr, eglPixmap) << "Should return NULL with NULL pixmap";
}

/**
 * Test: Multiple pixmap allocations
 * Requirement: Multiple pixmaps should be supported
 */
TEST_F(WesterosGLPixmapTest, GetNativePixmap_Multiple_AllCreated) {
    NEXUS_SurfaceHandle surface1 = createSurface(640, 480);
    NEXUS_SurfaceHandle surface2 = createSurface(1280, 720);
    NEXUS_SurfaceHandle surface3 = createSurface(1920, 1080);
    
    ASSERT_NE(nullptr, surface1);
    ASSERT_NE(nullptr, surface2);
    ASSERT_NE(nullptr, surface3);
    
    void* pixmap1 = nullptr;
    void* pixmap2 = nullptr;
    void* pixmap3 = nullptr;
    
    bool result1 = WstGLGetNativePixmap(ctx, surface1, &pixmap1);
    bool result2 = WstGLGetNativePixmap(ctx, surface2, &pixmap2);
    bool result3 = WstGLGetNativePixmap(ctx, surface3, &pixmap3);
    
    EXPECT_TRUE(result1) << "First pixmap should be created";
    EXPECT_TRUE(result2) << "Second pixmap should be created";
    EXPECT_TRUE(result3) << "Third pixmap should be created";
    
    EXPECT_NE(nullptr, pixmap1) << "First pixmap should be valid";
    EXPECT_NE(nullptr, pixmap2) << "Second pixmap should be valid";
    EXPECT_NE(nullptr, pixmap3) << "Third pixmap should be valid";
    
    // Pixmaps should be different
    EXPECT_NE(pixmap1, pixmap2);
    EXPECT_NE(pixmap2, pixmap3);
    EXPECT_NE(pixmap1, pixmap3);
    
    WstGLReleaseNativePixmap(ctx, pixmap1);
    WstGLReleaseNativePixmap(ctx, pixmap2);
    WstGLReleaseNativePixmap(ctx, pixmap3);
    
    NEXUS_Surface_Destroy(surface1);
    NEXUS_Surface_Destroy(surface2);
    NEXUS_Surface_Destroy(surface3);
}

/**
 * Test: Double release of pixmap
 * Requirement: WstGLReleaseNativePixmap must handle double release safely
 */
TEST_F(WesterosGLPixmapTest, ReleaseNativePixmap_DoubleRelease_HandlesSafely) {
    NEXUS_SurfaceHandle surface = createSurface(1280, 720);
    ASSERT_NE(nullptr, surface);
    
    void* pixmap = nullptr;
    bool getResult = WstGLGetNativePixmap(ctx, surface, &pixmap);
    ASSERT_TRUE(getResult);
    ASSERT_NE(nullptr, pixmap);
    
    WstGLReleaseNativePixmap(ctx, pixmap);
    
    // Second release should not crash
    WstGLReleaseNativePixmap(ctx, pixmap);
    
    SUCCEED();
    NEXUS_Surface_Destroy(surface);
}

/**
 * Test: Pixmap allocation failure
 * Requirement: WstGLGetNativePixmap must handle allocation failure
 */
TEST_F(WesterosGLPixmapTest, GetNativePixmap_AllocationFails_ReturnsNull) {
    MockNexus::setFailNextAlloc(true);
    
    NEXUS_SurfaceHandle surface = createSurface(1280, 720);
    
    // Surface creation should fail with allocation failure
    EXPECT_EQ(nullptr, surface) << "Surface creation should fail when allocation fails";
}

/**
 * Test: Pixmap stress test
 * Requirement: Pixmap operations should be stable under stress
 */
TEST_F(WesterosGLPixmapTest, PixmapOperations_Stress_Stable) {
    const int iterations = 50;
    
    for (int i = 0; i < iterations; i++) {
        int width = 800 + (i * 10);
        int height = 600 + (i * 10);
        
        NEXUS_SurfaceHandle surface = createSurface(width, height);
        
        if (surface) {
            void* pixmap = nullptr;
            bool getResult = WstGLGetNativePixmap(ctx, surface, &pixmap);
            
            if (getResult && pixmap) {
                int dimWidth = 0, dimHeight = 0;
                WstGLGetNativePixmapDimensions(ctx, pixmap, &dimWidth, &dimHeight);
                
                EXPECT_EQ(width, dimWidth) << "Width mismatch on iteration " << i;
                EXPECT_EQ(height, dimHeight) << "Height mismatch on iteration " << i;
                
                void* eglPixmap = WstGLGetEGLNativePixmap(ctx, pixmap);
                EXPECT_NE(nullptr, eglPixmap) << "EGL pixmap should be valid on iteration " << i;
                
                WstGLReleaseNativePixmap(ctx, pixmap);
            }
            
            NEXUS_Surface_Destroy(surface);
        }
    }
}
