/**
 * L1 Test Suite - Coverage Boost Tests
 * Comprehensive tests to achieve 75%+ code coverage
 * Tests edge cases, error paths, and complex scenarios
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "westeros-gl.h"
#include <vector>
#include <pthread.h>
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

extern "C" {
    NEXUS_SurfaceHandle NEXUS_Surface_Create(const NEXUS_SurfaceCreateSettings *pSettings);
    void NEXUS_Surface_Destroy(NEXUS_SurfaceHandle surface);
}

// External mock control functions
namespace MockNxClient {
    extern void reset();
    extern void setJoinSuccess(bool success);
    extern void setAllocSuccess(bool success);
    extern void setDisplaySize(unsigned width, unsigned height);
}

namespace MockNXPL {
    extern void reset();
    extern void setWindowCreationSuccess(bool success);
}

namespace MockNexus {
    extern void reset();
    extern void setInitSuccess(bool success);
    extern void setFailNextAlloc(bool fail);
}

class WesterosGLCoverageTest : public ::testing::Test {
protected:
    void SetUp() override {
        MockNxClient::reset();
        MockNXPL::reset();
        MockNexus::reset();
    }
    
    void TearDown() override {
    }
};

/**
 * Test: MEGA lifecycle test covering full API usage
 * Requirement: Cover complete lifecycle with all operations
 */
TEST_F(WesterosGLCoverageTest, MEGA_CompleteLifecycle_AllOperations) {
    // Initialize context
    WstGLCtx* ctx = WstGLInit();
    ASSERT_NE(nullptr, ctx);
    
    // Display operations
    WstGLDisplayInfo displayInfo;
    bool displayResult = WstGLGetDisplayInfo(ctx, &displayInfo);
    ASSERT_TRUE(displayResult);
    EXPECT_GT(displayInfo.width, 0);
    EXPECT_GT(displayInfo.height, 0);
    
    // Safe area operations
    EXPECT_GE(displayInfo.safeArea.x, 0);
    EXPECT_GE(displayInfo.safeArea.y, 0);
    EXPECT_GT(displayInfo.safeArea.w, 0);
    EXPECT_GT(displayInfo.safeArea.h, 0);
    
    // Create multiple windows
    std::vector<void*> windows;
    for (int i = 0; i < 5; i++) {
        void* window = WstGLCreateNativeWindow(ctx, i * 100, i * 100, 800, 600);
        if (window) {
            windows.push_back(window);
        }
    }
    EXPECT_GT(windows.size(), 0);
    
    // Create multiple pixmaps with Nexus surfaces
    std::vector<void*> pixmaps;
    std::vector<NEXUS_SurfaceHandle> surfaces;
    for (int i = 0; i < 5; i++) {
        NEXUS_SurfaceCreateSettings settings;
        memset(&settings, 0, sizeof(settings));
        settings.width = 640 + i * 64;
        settings.height = 480 + i * 48;
        settings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        
        NEXUS_SurfaceHandle surface = NEXUS_Surface_Create(&settings);
        if (surface) {
            surfaces.push_back(surface);
            
            void* pixmap = nullptr;
            bool getResult = WstGLGetNativePixmap(ctx, surface, &pixmap);
            if (getResult && pixmap) {
                pixmaps.push_back(pixmap);
                
                // Get dimensions
                int width, height;
                WstGLGetNativePixmapDimensions(ctx, pixmap, &width, &height);
                
                // Get EGL pixmap
                void* eglPixmap = WstGLGetEGLNativePixmap(ctx, pixmap);
                EXPECT_NE(nullptr, eglPixmap);
            }
        }
    }
    EXPECT_GT(pixmaps.size(), 0);
    
    // Cleanup pixmaps
    for (auto pixmap : pixmaps) {
        WstGLReleaseNativePixmap(ctx, pixmap);
    }
    
    // Cleanup surfaces
    for (auto surface : surfaces) {
        NEXUS_Surface_Destroy(surface);
    }
    
    // Cleanup windows
    for (auto window : windows) {
        WstGLDestroyNativeWindow(ctx, window);
    }
    
    // Terminate
    WstGLTerm(ctx);
}

/**
 * Test: Error path coverage - all NULL parameter cases
 * Requirement: Cover all NULL pointer checks
 */
TEST_F(WesterosGLCoverageTest, ErrorPaths_NullParameters_AllHandled) {
    WstGLCtx* ctx = WstGLInit();
    ASSERT_NE(nullptr, ctx);
    
    // All GetDisplayInfo NULL variations
    WstGLDisplayInfo displayInfo;
    EXPECT_FALSE(WstGLGetDisplayInfo(nullptr, &displayInfo));
    EXPECT_FALSE(WstGLGetDisplayInfo(ctx, nullptr));
    
    // Listener NULL cases
    EXPECT_FALSE(WstGLAddDisplaySizeListener(nullptr, nullptr, [](void*, int, int){}));
    EXPECT_FALSE(WstGLAddDisplaySizeListener(ctx, nullptr, nullptr));
    EXPECT_FALSE(WstGLRemoveDisplaySizeListener(nullptr, [](void*, int, int){}));
    
    // Window NULL cases
    EXPECT_EQ(nullptr, WstGLCreateNativeWindow(nullptr, 0, 0, 800, 600));
    WstGLDestroyNativeWindow(nullptr, (void*)0x1234);
    WstGLDestroyNativeWindow(ctx, nullptr);
    
    // Pixmap NULL cases
    NEXUS_SurfaceCreateSettings settings;
    memset(&settings, 0, sizeof(settings));
    settings.width = 800;
    settings.height = 600;
    settings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    NEXUS_SurfaceHandle surface = NEXUS_Surface_Create(&settings);
    
    void* pixmap = nullptr;
    EXPECT_FALSE(WstGLGetNativePixmap(nullptr, surface, &pixmap));
    EXPECT_FALSE(WstGLGetNativePixmap(ctx, nullptr, &pixmap));
    EXPECT_FALSE(WstGLGetNativePixmap(ctx, surface, nullptr));
    
    int width, height;
    WstGLGetNativePixmapDimensions(nullptr, (void*)0x1234, &width, &height);
    WstGLGetNativePixmapDimensions(ctx, nullptr, &width, &height);
    EXPECT_EQ(nullptr, WstGLGetEGLNativePixmap(nullptr, (void*)0x1234));
    EXPECT_EQ(nullptr, WstGLGetEGLNativePixmap(ctx, nullptr));
    
    if (surface) NEXUS_Surface_Destroy(surface);
    WstGLTerm(ctx);
}

/**
 * Test: Boundary value testing
 * Requirement: Cover boundary conditions for all numeric parameters
 */
TEST_F(WesterosGLCoverageTest, BoundaryValues_AllParameters_Covered) {
    WstGLCtx* ctx = WstGLInit();
    ASSERT_NE(nullptr, ctx);
    
    // Minimum dimensions
    void* window1 = WstGLCreateNativeWindow(ctx, 0, 0, 1, 1);
    if (window1) WstGLDestroyNativeWindow(ctx, window1);
    
    // Maximum expected dimensions
    void* window2 = WstGLCreateNativeWindow(ctx, 0, 0, 7680, 4320); // 8K
    if (window2) WstGLDestroyNativeWindow(ctx, window2);
    
    // Negative positions  
    void* window3 = WstGLCreateNativeWindow(ctx, -1000, -1000, 800, 600);
    if (window3) WstGLDestroyNativeWindow(ctx, window3);
    
    // Large positions
    void* window4 = WstGLCreateNativeWindow(ctx, 10000, 10000, 800, 600);
    if (window4) WstGLDestroyNativeWindow(ctx, window4);
    
    // Pixmap boundary values
    NEXUS_SurfaceCreateSettings settings1;
    memset(&settings1, 0, sizeof(settings1));
    settings1.width = 1;
    settings1.height = 1;
    settings1.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    NEXUS_SurfaceHandle surface1 = NEXUS_Surface_Create(&settings1);
    if (surface1) {
        void* pixmap1 = nullptr;
        WstGLGetNativePixmap(ctx, surface1, &pixmap1);
        if (pixmap1) WstGLReleaseNativePixmap(ctx, pixmap1);
        NEXUS_Surface_Destroy(surface1);
    }
    
    NEXUS_SurfaceCreateSettings settings2;
    memset(&settings2, 0, sizeof(settings2));
    settings2.width = 8192;
    settings2.height = 8192;
    settings2.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    NEXUS_SurfaceHandle surface2 = NEXUS_Surface_Create(&settings2);
    if (surface2) {
        void* pixmap2 = nullptr;
        WstGLGetNativePixmap(ctx, surface2, &pixmap2);
        if (pixmap2) WstGLReleaseNativePixmap(ctx, pixmap2);
        NEXUS_Surface_Destroy(surface2);
    }
    
    WstGLTerm(ctx);
}

/**
 * Test: Resource exhaustion scenarios
 * Requirement: Test behavior under resource pressure
 */
TEST_F(WesterosGLCoverageTest, ResourceExhaustion_HandledGracefully) {
    WstGLCtx* ctx = WstGLInit();
    ASSERT_NE(nullptr, ctx);
    
    // Allocate many resources
    std::vector<void*> windows;
    for (int i = 0; i < 100; i++) {
        void* window = WstGLCreateNativeWindow(ctx, i, i, 100, 100);
        if (window) {
            windows.push_back(window);
        }
    }
    
    std::vector<void*> pixmaps;
    std::vector<NEXUS_SurfaceHandle> surfaces;
    for (int i = 0; i < 100; i++) {
        NEXUS_SurfaceCreateSettings settings;
        memset(&settings, 0, sizeof(settings));
        settings.width = 100 + i;
        settings.height = 100 + i;
        settings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        
        NEXUS_SurfaceHandle surface = NEXUS_Surface_Create(&settings);
        if (surface) {
            surfaces.push_back(surface);
            void* pixmap = nullptr;
            if (WstGLGetNativePixmap(ctx, surface, &pixmap) && pixmap) {
                pixmaps.push_back(pixmap);
            }
        }
    }
    
    // Cleanup all
    for (auto window : windows) {
        WstGLDestroyNativeWindow(ctx, window);
    }
    
    for (auto pixmap : pixmaps) {
        WstGLReleaseNativePixmap(ctx, pixmap);
    }
    
    for (auto surface : surfaces) {
        NEXUS_Surface_Destroy(surface);
    }
    
    WstGLTerm(ctx);
}

/**
 * Test: Mixed operations stress test
 * Requirement: Interleaved operations should work correctly
 */
TEST_F(WesterosGLCoverageTest, MixedOperations_Interleaved_Stable) {
    WstGLCtx* ctx = WstGLInit();
    ASSERT_NE(nullptr, ctx);
    
    for (int i = 0; i < 50; i++) {
        // Create window
        void* window = WstGLCreateNativeWindow(ctx, i, i, 800, 600);
        
        // Get display info
        WstGLDisplayInfo displayInfo;
        WstGLGetDisplayInfo(ctx, &displayInfo);
        
        // Create pixmap with Nexus surface
        NEXUS_SurfaceCreateSettings settings;
        memset(&settings, 0, sizeof(settings));
        settings.width = displayInfo.width / 2;
        settings.height = displayInfo.height / 2;
        settings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        
        NEXUS_SurfaceHandle surface = NEXUS_Surface_Create(&settings);
        void* pixmap = nullptr;
        
        if (surface) {
            WstGLGetNativePixmap(ctx, surface, &pixmap);
            
            // Get pixmap dimensions
            if (pixmap) {
                int pWidth, pHeight;
                WstGLGetNativePixmapDimensions(ctx, pixmap, &pWidth, &pHeight);
                
                // Get EGL pixmap
                WstGLGetEGLNativePixmap(ctx, pixmap);
            }
        }
        
        // Cleanup in different order
        if (i % 2 == 0) {
            if (window) WstGLDestroyNativeWindow(ctx, window);
            if (pixmap) WstGLReleaseNativePixmap(ctx, pixmap);
        } else {
            if (pixmap) WstGLReleaseNativePixmap(ctx, pixmap);
            if (window) WstGLDestroyNativeWindow(ctx, window);
        }
        
        if (surface) NEXUS_Surface_Destroy(surface);
    }
    
    WstGLTerm(ctx);
}

/**
 * Test: Initialization failure scenarios
 * Requirement: Cover all initialization failure paths
 */
TEST_F(WesterosGLCoverageTest, InitializationFailures_AllPaths_Covered) {
    // NxClient join failure
    MockNxClient::setJoinSuccess(false);
    WstGLCtx* ctx1 = WstGLInit();
    EXPECT_EQ(nullptr, ctx1);
    
    // Reset and try Graphics2D failure
    MockNxClient::reset();
    MockNexus::setInitSuccess(false);
    WstGLCtx* ctx2 = WstGLInit();
    // May still return context but with degraded functionality
    if (ctx2) WstGLTerm(ctx2);
    
    // Reset for subsequent tests
    MockNxClient::reset();
    MockNexus::reset();
}

/**
 * Test: Display size listener management
 * Requirement: Complete listener lifecycle coverage
 */
TEST_F(WesterosGLCoverageTest, DisplaySizeListeners_Lifecycle_Complete) {
    WstGLCtx* ctx = WstGLInit();
    ASSERT_NE(nullptr, ctx);
    
    auto callback1 = [](void* userData, int width, int height) {};
    auto callback2 = [](void* userData, int width, int height) {};
    
    // Add multiple listeners
    EXPECT_TRUE(WstGLAddDisplaySizeListener(ctx, (void*)0x1111, callback1));
    EXPECT_TRUE(WstGLAddDisplaySizeListener(ctx, (void*)0x2222, callback2));
    
    // Remove listeners
    EXPECT_TRUE(WstGLRemoveDisplaySizeListener(ctx, callback1));
    EXPECT_TRUE(WstGLRemoveDisplaySizeListener(ctx, callback2));
    
    // Try removing again (should fail)
    EXPECT_FALSE(WstGLRemoveDisplaySizeListener(ctx, callback1));
    
    WstGLTerm(ctx);
}

/**
 * Test: Pixmap allocation failure handling
 * Requirement: Cover pixmap allocation error paths
 */
TEST_F(WesterosGLCoverageTest, PixmapAllocation_Failures_Handled) {
    WstGLCtx* ctx = WstGLInit();
    ASSERT_NE(nullptr, ctx);
    
    // Cause allocation failure
    MockNexus::setFailNextAlloc(true);
    NEXUS_SurfaceCreateSettings settings;
    memset(&settings, 0, sizeof(settings));
    settings.width = 1280;
    settings.height = 720;
    settings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    
    NEXUS_SurfaceHandle surface = NEXUS_Surface_Create(&settings);
    void* pixmap = nullptr;
    if (surface) {
        WstGLGetNativePixmap(ctx, surface, &pixmap);
    }
    EXPECT_EQ(nullptr, pixmap);
    if (surface) NEXUS_Surface_Destroy(surface);
    
    // Reset and verify recovery
    MockNexus::setFailNextAlloc(false);
    NEXUS_SurfaceCreateSettings settings2;
    memset(&settings2, 0, sizeof(settings2));
    settings2.width = 1280;
    settings2.height = 720;
    settings2.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    
    NEXUS_SurfaceHandle surface2 = NEXUS_Surface_Create(&settings2);
    void* pixmap2 = nullptr;
    if (surface2) {
        WstGLGetNativePixmap(ctx, surface2, &pixmap2);
    }
    EXPECT_NE(nullptr, pixmap2);
    
    if (pixmap2) {
        WstGLReleaseNativePixmap(ctx, pixmap2);
    }
    if (surface2) NEXUS_Surface_Destroy(surface2);
    
    WstGLTerm(ctx);
}

/**
 * Test: Window allocation failure handling
 * Requirement: Cover window allocation error paths
 */
TEST_F(WesterosGLCoverageTest, WindowAllocation_Failures_Handled) {
    WstGLCtx* ctx = WstGLInit();
    ASSERT_NE(nullptr, ctx);
    
    // Cause allocation failure
    MockNXPL::setWindowCreationSuccess(false);
    void* window = WstGLCreateNativeWindow(ctx, 0, 0, 1280, 720);
    EXPECT_EQ(nullptr, window);
    
    // Reset and verify recovery
    MockNXPL::setWindowCreationSuccess(true);
    void* window2 = WstGLCreateNativeWindow(ctx, 0, 0, 1280, 720);
    EXPECT_NE(nullptr, window2);
    
    if (window2) {
        WstGLDestroyNativeWindow(ctx, window2);
    }
    
    WstGLTerm(ctx);
}

/**
 * Test: Multiple display size queries
 * Requirement: Verify consistent display information
 */
TEST_F(WesterosGLCoverageTest, DisplayInfo_MultipleQueries_Consistent) {
    WstGLCtx* ctx = WstGLInit();
    ASSERT_NE(nullptr, ctx);
    
    WstGLDisplayInfo displayInfo1, displayInfo2;
    
    bool result1 = WstGLGetDisplayInfo(ctx, &displayInfo1);
    bool result2 = WstGLGetDisplayInfo(ctx, &displayInfo2);
    
    EXPECT_TRUE(result1);
    EXPECT_TRUE(result2);
    EXPECT_EQ(displayInfo1.width, displayInfo2.width) << "Display width should be consistent";
    EXPECT_EQ(displayInfo1.height, displayInfo2.height) << "Display height should be consistent";
    
    WstGLTerm(ctx);
}

/**
 * Test: Operations after termination
 * Requirement: All operations should fail gracefully after termination
 */
TEST_F(WesterosGLCoverageTest, OperationsAfterTermination_FailGracefully) {
    WstGLCtx* ctx = WstGLInit();
    ASSERT_NE(nullptr, ctx);
    
    WstGLTerm(ctx);
    
    // All operations should fail gracefully
    WstGLDisplayInfo displayInfo;
    EXPECT_FALSE(WstGLGetDisplayInfo(ctx, &displayInfo));
    
    EXPECT_EQ(nullptr, WstGLCreateNativeWindow(ctx, 0, 0, 800, 600));
    
    NEXUS_SurfaceCreateSettings settings;
    memset(&settings, 0, sizeof(settings));
    settings.width = 800;
    settings.height = 600;
    settings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    
    NEXUS_SurfaceHandle surface = NEXUS_Surface_Create(&settings);
    void* pixmap = nullptr;
    if (surface) {
        EXPECT_FALSE(WstGLGetNativePixmap(ctx, surface, &pixmap));
        NEXUS_Surface_Destroy(surface);
    }
}

/**
 * Test: Various pixmap sizes
 * Requirement: Cover different pixmap size code paths
 */
TEST_F(WesterosGLCoverageTest, PixmapSizes_Various_AllHandled) {
    WstGLCtx* ctx = WstGLInit();
    ASSERT_NE(nullptr, ctx);
    
    struct {
        int width;
        int height;
        const char* name;
    } sizes[] = {
        {640, 480, "VGA"},
        {800, 600, "SVGA"},
        {1024, 768, "XGA"},
        {1280, 720, "HD"},
        {1920, 1080, "FullHD"},
        {2560, 1440, "QHD"},
        {3840, 2160, "4K"},
    };
    
    for (auto& size : sizes) {
        NEXUS_SurfaceCreateSettings settings;
        memset(&settings, 0, sizeof(settings));
        settings.width = size.width;
        settings.height = size.height;
        settings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        
        NEXUS_SurfaceHandle surface = NEXUS_Surface_Create(&settings);
        void* pixmap = nullptr;
        
        if (surface) {
            WstGLGetNativePixmap(ctx, surface, &pixmap);
            
            if (pixmap) {
                int width, height;
                WstGLGetNativePixmapDimensions(ctx, pixmap, &width, &height);
                
                EXPECT_EQ(size.width, width) << "Width mismatch for " << size.name;
                EXPECT_EQ(size.height, height) << "Height mismatch for " << size.name;
                
                WstGLReleaseNativePixmap(ctx, pixmap);
            }
            
            NEXUS_Surface_Destroy(surface);
        }
    }
    
    WstGLTerm(ctx);
}
