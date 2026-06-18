/*
 * L1 Unit Tests for Westeros Renderer Nexus - Surface Management
 * Target: surfaceCreate, surfaceDestroy, surfaceCommit, updateScene
 */

#include <gtest/gtest.h>
#include "westeros-render.h"
#include "wayland-server.h"

// External mock functions
extern "C" {
    extern void MockNexus_reset();
    extern void mock_nxclient_reset();
    extern struct wl_resource* mock_create_resource(uint32_t id);
    extern void mock_destroy_resource(struct wl_resource *resource);
    extern void mock_surface_client_set_push_failure(bool fail);
}

class WesterosRendererSurfaceTest : public ::testing::Test {
protected:
    WstRenderer *renderer;
    
    void SetUp() override {
        MockNexus_reset();
        mock_nxclient_reset();
        
        renderer = new WstRenderer();
        memset(renderer, 0, sizeof(WstRenderer));
        
        int result = renderer_init(renderer, 0, nullptr);
        ASSERT_EQ(0, result);
    }
    
    void TearDown() override {
        if (renderer) {
            if (renderer->renderTerm) {
                renderer->renderTerm(renderer);
            }
            delete renderer;
        }
    }
};

// Test surface creation
TEST_F(WesterosRendererSurfaceTest, CreateSurface_Success) {
    ASSERT_NE(nullptr, renderer->surfaceCreate);
    
    WstRenderSurface *surface = renderer->surfaceCreate(renderer);
    
    EXPECT_NE(nullptr, surface);
    
    if (surface) {
        renderer->surfaceDestroy(renderer, surface);
    }
}

// Test surface destruction
TEST_F(WesterosRendererSurfaceTest, DestroySurface_Success) {
    WstRenderSurface *surface = renderer->surfaceCreate(renderer);
    ASSERT_NE(nullptr, surface);
    
    ASSERT_NO_THROW({
        renderer->surfaceDestroy(renderer, surface);
    });
}

// Test multiple surface creation
TEST_F(WesterosRendererSurfaceTest, CreateMultipleSurfaces) {
    std::vector<WstRenderSurface*> surfaces;
    
    // Create 10 surfaces
    for (int i = 0; i < 10; i++) {
        WstRenderSurface *surface = renderer->surfaceCreate(renderer);
        ASSERT_NE(nullptr, surface);
        surfaces.push_back(surface);
    }
    
    // Destroy all surfaces
    for (auto surface : surfaces) {
        renderer->surfaceDestroy(renderer, surface);
    }
}

// Test surface commit with valid resource
TEST_F(WesterosRendererSurfaceTest, CommitSurface_ValidResource) {
    WstRenderSurface *surface = renderer->surfaceCreate(renderer);
    ASSERT_NE(nullptr, surface);
    
    struct wl_resource *resource = mock_create_resource(100);
    ASSERT_NE(nullptr, resource);
    
    ASSERT_NO_THROW({
        renderer->surfaceCommit(renderer, surface, resource);
    });
    
    mock_destroy_resource(resource);
    renderer->surfaceDestroy(renderer, surface);
}

// Test surface commit with NULL resource
TEST_F(WesterosRendererSurfaceTest, CommitSurface_NullResource) {
    WstRenderSurface *surface = renderer->surfaceCreate(renderer);
    ASSERT_NE(nullptr, surface);
    
    ASSERT_NO_THROW({
        renderer->surfaceCommit(renderer, surface, nullptr);
    });
    
    renderer->surfaceDestroy(renderer, surface);
}

// Test update scene
TEST_F(WesterosRendererSurfaceTest, UpdateScene_Success) {
    ASSERT_NE(nullptr, renderer->updateScene);
    
    ASSERT_NO_THROW({
        renderer->updateScene(renderer);
    });
}

// Test delegate update scene
TEST_F(WesterosRendererSurfaceTest, DelegateUpdateScene_Success) {
    ASSERT_NE(nullptr, renderer->delegateUpdateScene);
    
    std::vector<WstRect> rects;
    WstRect rect1 = {0, 0, 1920, 1080};
    WstRect rect2 = {100, 100, 800, 600};
    rects.push_back(rect1);
    rects.push_back(rect2);
    
    ASSERT_NO_THROW({
        renderer->delegateUpdateScene(renderer, rects);
    });
}

// Test delegate update scene with empty rects
TEST_F(WesterosRendererSurfaceTest, DelegateUpdateScene_EmptyRects) {
    std::vector<WstRect> rects;
    
    ASSERT_NO_THROW({
        renderer->delegateUpdateScene(renderer, rects);
    });
}

// Test surface commit failure scenario
TEST_F(WesterosRendererSurfaceTest, CommitSurface_PushFailure) {
    mock_surface_client_set_push_failure(true);
    
    WstRenderSurface *surface = renderer->surfaceCreate(renderer);
    ASSERT_NE(nullptr, surface);
    
    struct wl_resource *resource = mock_create_resource(200);
    
    // Should not crash even if push fails
    ASSERT_NO_THROW({
        renderer->surfaceCommit(renderer, surface, resource);
    });
    
    mock_destroy_resource(resource);
    renderer->surfaceDestroy(renderer, surface);
    
    mock_surface_client_set_push_failure(false);
}

// Test create and destroy same surface multiple times
TEST_F(WesterosRendererSurfaceTest, RepeatedCreateDestroy) {
    for (int i = 0; i < 20; i++) {
        WstRenderSurface *surface = renderer->surfaceCreate(renderer);
        ASSERT_NE(nullptr, surface);
        renderer->surfaceDestroy(renderer, surface);
    }
}

// Test surface lifecycle with commits
TEST_F(WesterosRendererSurfaceTest, SurfaceLifecycleWithCommits) {
    WstRenderSurface *surface = renderer->surfaceCreate(renderer);
    ASSERT_NE(nullptr, surface);
    
    struct wl_resource *resource = mock_create_resource(300);
    
    // Multiple commits
    for (int i = 0; i < 5; i++) {
        renderer->surfaceCommit(renderer, surface, resource);
    }
    
    // Update scene
    renderer->updateScene(renderer);
    
    mock_destroy_resource(resource);
    renderer->surfaceDestroy(renderer, surface);
}

// Test NULL surface destruction (error path)
TEST_F(WesterosRendererSurfaceTest, DestroyNullSurface_NoCrash) {
    ASSERT_NO_THROW({
        renderer->surfaceDestroy(renderer, nullptr);
    });
}

// Stress test: many surfaces with commits
TEST_F(WesterosRendererSurfaceTest, StressTest_ManySurfacesWithCommits) {
    std::vector<WstRenderSurface*> surfaces;
    std::vector<struct wl_resource*> resources;
    
    // Create 20 surfaces
    for (int i = 0; i < 20; i++) {
        WstRenderSurface *surface = renderer->surfaceCreate(renderer);
        ASSERT_NE(nullptr, surface);
        surfaces.push_back(surface);
        
        struct wl_resource *resource = mock_create_resource(1000 + i);
        resources.push_back(resource);
        
        renderer->surfaceCommit(renderer, surface, resource);
    }
    
    // Update scene
    renderer->updateScene(renderer);
    
    // Cleanup
    for (size_t i = 0; i < surfaces.size(); i++) {
        renderer->surfaceDestroy(renderer, surfaces[i]);
        mock_destroy_resource(resources[i]);
    }
}
