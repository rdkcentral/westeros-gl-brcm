/*
 * L1 Unit Tests for Westeros Renderer Nexus - Coverage Boost
 * Comprehensive edge cases, error paths, and boundary testing
 */

#include <gtest/gtest.h>
#include "westeros-render.h"
#include "wayland-server.h"
#include <vector>

// External mock functions
extern "C" {
    extern void MockNexus_reset();
    extern void mock_nxclient_reset();
    extern struct wl_resource* mock_create_resource(uint32_t id);
    extern void mock_destroy_resource(struct wl_resource *resource);
    extern void mock_surface_client_set_push_failure(bool fail);
    extern void mock_nxclient_set_join_failure(bool fail);
}

class WesterosRendererCoverageTest : public ::testing::Test {
protected:
    void SetUp() override {
        MockNexus_reset();
        mock_nxclient_reset();
    }
    
    void TearDown() override {
        MockNexus_reset();
        mock_nxclient_reset();
    }
};

// ========== MEGA Lifecycle Test ==========

TEST_F(WesterosRendererCoverageTest, MEGA_FullLifecycleWithAllOperations) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    // 1. Initialize
    int result = renderer_init(&renderer, 0, nullptr);
    ASSERT_EQ(0, result);
    
    // 2. Create multiple surfaces
    std::vector<WstRenderSurface*> surfaces;
    for (int i = 0; i < 5; i++) {
        WstRenderSurface *surface = renderer.surfaceCreate(&renderer);
        ASSERT_NE(nullptr, surface);
        surfaces.push_back(surface);
    }
    
    // 3. Set properties on all surfaces
    for (size_t i = 0; i < surfaces.size(); i++) {
        renderer.surfaceSetVisible(&renderer, surfaces[i], (i % 2) == 0);
        renderer.surfaceSetGeometry(&renderer, surfaces[i], i * 100, i * 50, 800, 600);
        renderer.surfaceSetOpacity(&renderer, surfaces[i], (float)i / 10.0f);
        renderer.surfaceSetZOrder(&renderer, surfaces[i], (float)i / 10.0f);
    }
    
    // 4. Commit surfaces with resources
    std::vector<struct wl_resource*> resources;
    for (size_t i = 0; i < surfaces.size(); i++) {
        struct wl_resource *resource = mock_create_resource(2000 + i);
        resources.push_back(resource);
        renderer.surfaceCommit(&renderer, surfaces[i], resource);
    }
    
    // 5. Update scene
    renderer.updateScene(&renderer);
    
    // 6. Delegate update with rectangles
    std::vector<WstRect> rects;
    for (int i = 0; i < 3; i++) {
        WstRect rect = {i * 200, i * 100, 640, 480};
        rects.push_back(rect);
    }
    renderer.delegateUpdateScene(&renderer, rects);
    
    // 7. Verify all properties
    for (size_t i = 0; i < surfaces.size(); i++) {
        bool visible;
        renderer.surfaceGetVisible(&renderer, surfaces[i], &visible);
        
        int x, y, width, height;
        renderer.surfaceGetGeometry(&renderer, surfaces[i], &x, &y, &width, &height);
        EXPECT_EQ((int)(i * 100), x);
        EXPECT_EQ((int)(i * 50), y);
        
        float opacity, z;
        renderer.surfaceGetOpacity(&renderer, surfaces[i], &opacity);
        renderer.surfaceGetZOrder(&renderer, surfaces[i], &z);
    }
    
    // 8. Cleanup resources
    for (auto resource : resources) {
        mock_destroy_resource(resource);
    }
    
    // 9. Destroy surfaces
    for (auto surface : surfaces) {
        renderer.surfaceDestroy(&renderer, surface);
    }
    
    // 10. Terminate
    renderer.renderTerm(&renderer);
}

// ========== Error Path Tests ==========

TEST_F(WesterosRendererCoverageTest, ErrorPaths_NullParameters) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    int result = renderer_init(&renderer, 0, nullptr);
    ASSERT_EQ(0, result);
    
    WstRenderSurface *surface = renderer.surfaceCreate(&renderer);
    ASSERT_NE(nullptr, surface);
    
    // NULL parameter tests - should not crash
    ASSERT_NO_THROW({
        renderer.surfaceSetVisible(&renderer, nullptr, true);
        renderer.surfaceSetGeometry(&renderer, nullptr, 0, 0, 100, 100);
        renderer.surfaceSetOpacity(&renderer, nullptr, 0.5f);
        renderer.surfaceSetZOrder(&renderer, nullptr, 0.5f);
        renderer.surfaceCommit(&renderer, nullptr, nullptr);
        renderer.surfaceDestroy(&renderer, nullptr);
    });
    
    // NULL output parameters
    ASSERT_NO_THROW({
        renderer.surfaceGetVisible(&renderer, surface, nullptr);
        renderer.surfaceGetGeometry(&renderer, surface, nullptr, nullptr, nullptr, nullptr);
        renderer.surfaceGetOpacity(&renderer, surface, nullptr);
        renderer.surfaceGetZOrder(&renderer, surface, nullptr);
    });
    
    renderer.surfaceDestroy(&renderer, surface);
    renderer.renderTerm(&renderer);
}

TEST_F(WesterosRendererCoverageTest, ErrorPaths_InitFailureScenarios) {
    // Test with --display argument
    WstRenderer renderer1;
    memset(&renderer1, 0, sizeof(renderer1));
    char arg1[] = "--display";
    char arg2[] = "wayland-1";
    char* argv1[] = {arg1, arg2};
    
    int result = renderer_init(&renderer1, 2, argv1);
    EXPECT_EQ(-1, result);
    
    // Test with NxClient join failure
    mock_nxclient_set_join_failure(true);
    WstRenderer renderer2;
    memset(&renderer2, 0, sizeof(renderer2));
    
    result = renderer_init(&renderer2, 0, nullptr);
    EXPECT_EQ(-1, result);
    
    mock_nxclient_set_join_failure(false);
}

TEST_F(WesterosRendererCoverageTest, ErrorPaths_SurfacePushFailure) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    int result = renderer_init(&renderer, 0, nullptr);
    ASSERT_EQ(0, result);
    
    WstRenderSurface *surface = renderer.surfaceCreate(&renderer);
    ASSERT_NE(nullptr, surface);
    
    // Enable push failure
    mock_surface_client_set_push_failure(true);
    
    struct wl_resource *resource = mock_create_resource(5000);
    
    // Should handle failure gracefully
    ASSERT_NO_THROW({
        renderer.surfaceCommit(&renderer, surface, resource);
    });
    
    mock_destroy_resource(resource);
    renderer.surfaceDestroy(&renderer, surface);
    renderer.renderTerm(&renderer);
    
    mock_surface_client_set_push_failure(false);
}

// ========== Boundary Value Tests ==========

TEST_F(WesterosRendererCoverageTest, BoundaryValues_GeometryExtremes) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    int result = renderer_init(&renderer, 0, nullptr);
    ASSERT_EQ(0, result);
    
    WstRenderSurface *surface = renderer.surfaceCreate(&renderer);
    ASSERT_NE(nullptr, surface);
    
    // Test extreme values
    struct {
        int x, y, width, height;
    } testCases[] = {
        {0, 0, 0, 0},
        {-1000, -1000, 100, 100},
        {10000, 10000, 7680, 4320},
        {-32768, -32768, 32767, 32767},
        {1, 1, 1, 1}
    };
    
    for (auto& tc : testCases) {
        ASSERT_NO_THROW({
            renderer.surfaceSetGeometry(&renderer, surface, tc.x, tc.y, tc.width, tc.height);
        });
        
        int x, y, width, height;
        renderer.surfaceGetGeometry(&renderer, surface, &x, &y, &width, &height);
        EXPECT_EQ(tc.x, x);
        EXPECT_EQ(tc.y, y);
        EXPECT_EQ(tc.width, width);
        EXPECT_EQ(tc.height, height);
    }
    
    renderer.surfaceDestroy(&renderer, surface);
    renderer.renderTerm(&renderer);
}

TEST_F(WesterosRendererCoverageTest, BoundaryValues_OpacityExtremes) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    int result = renderer_init(&renderer, 0, nullptr);
    ASSERT_EQ(0, result);
    
    WstRenderSurface *surface = renderer.surfaceCreate(&renderer);
    ASSERT_NE(nullptr, surface);
    
    float testValues[] = {0.0f, 0.1f, 0.5f, 0.9f, 1.0f, 1.5f, -0.5f};
    
    for (float value : testValues) {
        ASSERT_NO_THROW({
            renderer.surfaceSetOpacity(&renderer, surface, value);
        });
        
        float opacity;
        renderer.surfaceGetOpacity(&renderer, surface, &opacity);
        EXPECT_FLOAT_EQ(value, opacity);
    }
    
    renderer.surfaceDestroy(&renderer, surface);
    renderer.renderTerm(&renderer);
}

TEST_F(WesterosRendererCoverageTest, BoundaryValues_ZOrderExtremes) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    int result = renderer_init(&renderer, 0, nullptr);
    ASSERT_EQ(0, result);
    
    WstRenderSurface *surface = renderer.surfaceCreate(&renderer);
    ASSERT_NE(nullptr, surface);
    
    float testValues[] = {-100.0f, -1.0f, 0.0f, 0.5f, 1.0f, 100.0f, 1000.0f};
    
    for (float value : testValues) {
        ASSERT_NO_THROW({
            renderer.surfaceSetZOrder(&renderer, surface, value);
        });
        
        float z;
        renderer.surfaceGetZOrder(&renderer, surface, &z);
        EXPECT_FLOAT_EQ(value, z);
    }
    
    renderer.surfaceDestroy(&renderer, surface);
    renderer.renderTerm(&renderer);
}

// ========== Stress Tests ==========

TEST_F(WesterosRendererCoverageTest, StressTest_ManySurfaces) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    int result = renderer_init(&renderer, 0, nullptr);
    ASSERT_EQ(0, result);
    
    std::vector<WstRenderSurface*> surfaces;
    
    // Create 100 surfaces
    for (int i = 0; i < 100; i++) {
        WstRenderSurface *surface = renderer.surfaceCreate(&renderer);
        ASSERT_NE(nullptr, surface);
        surfaces.push_back(surface);
        
        renderer.surfaceSetGeometry(&renderer, surface, i, i, 100, 100);
    }
    
    // Update scene
    renderer.updateScene(&renderer);
    
    // Destroy all
    for (auto surface : surfaces) {
        renderer.surfaceDestroy(&renderer, surface);
    }
    
    renderer.renderTerm(&renderer);
}

TEST_F(WesterosRendererCoverageTest, StressTest_RapidCommits) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    int result = renderer_init(&renderer, 0, nullptr);
    ASSERT_EQ(0, result);
    
    WstRenderSurface *surface = renderer.surfaceCreate(&renderer);
    ASSERT_NE(nullptr, surface);
    
    struct wl_resource *resource = mock_create_resource(9999);
    
    // 1000 rapid commits
    for (int i = 0; i < 1000; i++) {
        renderer.surfaceCommit(&renderer, surface, resource);
    }
    
    mock_destroy_resource(resource);
    renderer.surfaceDestroy(&renderer, surface);
    renderer.renderTerm(&renderer);
}

TEST_F(WesterosRendererCoverageTest, StressTest_RapidPropertyChanges) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    int result = renderer_init(&renderer, 0, nullptr);
    ASSERT_EQ(0, result);
    
    WstRenderSurface *surface = renderer.surfaceCreate(&renderer);
    ASSERT_NE(nullptr, surface);
    
    // Rapid property changes
    for (int i = 0; i < 500; i++) {
        renderer.surfaceSetVisible(&renderer, surface, i % 2 == 0);
        renderer.surfaceSetGeometry(&renderer, surface, i, i, 800, 600);
        renderer.surfaceSetOpacity(&renderer, surface, (float)(i % 100) / 100.0f);
        renderer.surfaceSetZOrder(&renderer, surface, (float)(i % 50) / 50.0f);
    }
    
    renderer.surfaceDestroy(&renderer, surface);
    renderer.renderTerm(&renderer);
}

// ========== Update Scene Coverage ==========

TEST_F(WesterosRendererCoverageTest, UpdateScene_NoSurfaces) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    int result = renderer_init(&renderer, 0, nullptr);
    ASSERT_EQ(0, result);
    
    // Update with no surfaces
    ASSERT_NO_THROW({
        renderer.updateScene(&renderer);
    });
    
    renderer.renderTerm(&renderer);
}

TEST_F(WesterosRendererCoverageTest, DelegateUpdateScene_ManyRectangles) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    int result = renderer_init(&renderer, 0, nullptr);
    ASSERT_EQ(0, result);
    
    std::vector<WstRect> rects;
    
    // Create 100 rectangles
    for (int i = 0; i < 100; i++) {
        WstRect rect = {i * 10, i * 10, 100 + i, 100 + i};
        rects.push_back(rect);
    }
    
    ASSERT_NO_THROW({
        renderer.delegateUpdateScene(&renderer, rects);
    });
    
    renderer.renderTerm(&renderer);
}

// ========== Combined Stress Test ==========

TEST_F(WesterosRendererCoverageTest, CombinedStress_AllOperations) {
    for (int cycle = 0; cycle < 3; cycle++) {
        WstRenderer renderer;
        memset(&renderer, 0, sizeof(renderer));
        
        int result = renderer_init(&renderer, 0, nullptr);
        ASSERT_EQ(0, result);
        
        std::vector<WstRenderSurface*> surfaces;
        std::vector<struct wl_resource*> resources;
        
        // Create 20 surfaces with properties and commits
        for (int i = 0; i < 20; i++) {
            WstRenderSurface *surface = renderer.surfaceCreate(&renderer);
            surfaces.push_back(surface);
            
            renderer.surfaceSetVisible(&renderer, surface, true);
            renderer.surfaceSetGeometry(&renderer, surface, i * 50, i * 30, 640, 480);
            renderer.surfaceSetOpacity(&renderer, surface, 0.8f);
            renderer.surfaceSetZOrder(&renderer, surface, (float)i / 20.0f);
            
            struct wl_resource *resource = mock_create_resource(10000 + cycle * 100 + i);
            resources.push_back(resource);
            renderer.surfaceCommit(&renderer, surface, resource);
        }
        
        // Multiple update scenes
        for (int i = 0; i < 10; i++) {
            renderer.updateScene(&renderer);
        }
        
        // Cleanup
        for (auto resource : resources) {
            mock_destroy_resource(resource);
        }
        for (auto surface : surfaces) {
            renderer.surfaceDestroy(&renderer, surface);
        }
        
        renderer.renderTerm(&renderer);
    }
}
