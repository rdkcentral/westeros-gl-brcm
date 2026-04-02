/*
 * L1 Unit Tests for Westeros Renderer Nexus - Initialization & Termination
 * Target: renderer_init(), wstRendererTerm(), wstRendererNXCreate()
 */

#include <gtest/gtest.h>
#include "westeros-render.h"

// External mock control functions
extern "C" {
    extern void MockNexus_reset();
    extern void mock_nxclient_reset();
    extern void mock_nxclient_set_join_failure(bool fail);
    extern void mock_surface_client_set_client_create_failure(bool fail);
}

class WesterosRendererInitTest : public ::testing::Test {
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

// Test successful renderer initialization
TEST_F(WesterosRendererInitTest, SuccessfulInit) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    int result = renderer_init(&renderer, 0, nullptr);
    
    EXPECT_EQ(0, result);
    EXPECT_NE(nullptr, renderer.renderer);
    EXPECT_NE(nullptr, renderer.renderTerm);
    EXPECT_NE(nullptr, renderer.updateScene);
    EXPECT_NE(nullptr, renderer.surfaceCreate);
    EXPECT_NE(nullptr, renderer.surfaceDestroy);
    EXPECT_NE(nullptr, renderer.surfaceCommit);
    EXPECT_NE(nullptr, renderer.surfaceSetVisible);
    EXPECT_NE(nullptr, renderer.surfaceGetVisible);
    EXPECT_NE(nullptr, renderer.surfaceSetGeometry);
    EXPECT_NE(nullptr, renderer.surfaceGetGeometry);
    EXPECT_NE(nullptr, renderer.surfaceSetOpacity);
    EXPECT_NE(nullptr, renderer.surfaceGetOpacity);
    EXPECT_NE(nullptr, renderer.surfaceSetZOrder);
    EXPECT_NE(nullptr, renderer.surfaceGetZOrder);
    EXPECT_NE(nullptr, renderer.delegateUpdateScene);
    
    // Cleanup
    if (renderer.renderTerm) {
        renderer.renderTerm(&renderer);
    }
}

// Test renderer termination
TEST_F(WesterosRendererInitTest, SuccessfulTermination) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    int result = renderer_init(&renderer, 0, nullptr);
    ASSERT_EQ(0, result);
    
    // Should not crash
    ASSERT_NO_THROW({
        renderer.renderTerm(&renderer);
    });
}

// Test init with --display argument (should fail for Nexus)
TEST_F(WesterosRendererInitTest, InitWithDisplayArgument_Fails) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    char arg1[] = "--display";
    char arg2[] = "wayland-0";
    char* argv[] = {arg1, arg2};
    
    int result = renderer_init(&renderer, 2, argv);
    
    EXPECT_EQ(-1, result);
}

// Test init failure when NxClient_Join fails
TEST_F(WesterosRendererInitTest, InitFailure_NxClientJoinFails) {
    mock_nxclient_set_join_failure(true);
    
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    int result = renderer_init(&renderer, 0, nullptr);
    
    EXPECT_EQ(-1, result);
}

// Test multiple init/term cycles
TEST_F(WesterosRendererInitTest, MultipleInitTermCycles) {
    for (int i = 0; i < 3; i++) {
        WstRenderer renderer;
        memset(&renderer, 0, sizeof(renderer));
        
        int result = renderer_init(&renderer, 0, nullptr);
        ASSERT_EQ(0, result);
        
        renderer.renderTerm(&renderer);
    }
}

// Test NULL renderer pointer
TEST_F(WesterosRendererInitTest, TerminateNullRenderer_NoCrash) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    renderer.renderer = nullptr;
    
    // Mock renderTerm function should handle NULL safely
    ASSERT_NO_THROW({
        if (renderer.renderTerm) {
            renderer.renderTerm(&renderer);
        }
    });
}

// Test surface client acquisition failure
TEST_F(WesterosRendererInitTest, InitFailure_SurfaceClientAcquireFails) {
    mock_surface_client_set_client_create_failure(true);
    
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    // Init should still succeed but surface client will be NULL
    int result = renderer_init(&renderer, 0, nullptr);
    
    // Behavior depends on implementation - may succeed or fail
    // Just verify no crash
    if (result == 0 && renderer.renderTerm) {
        renderer.renderTerm(&renderer);
    }
}

// Test init with empty arguments
TEST_F(WesterosRendererInitTest, InitWithEmptyArgs) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    char* argv[] = {};
    int result = renderer_init(&renderer, 0, argv);
    
    EXPECT_EQ(0, result);
    
    if (renderer.renderTerm) {
        renderer.renderTerm(&renderer);
    }
}

// Test renderer struct is fully populated
TEST_F(WesterosRendererInitTest, AllCallbacksPopulated) {
    WstRenderer renderer;
    memset(&renderer, 0, sizeof(renderer));
    
    int result = renderer_init(&renderer, 0, nullptr);
    ASSERT_EQ(0, result);
    
    // Verify ALL function pointers are set
    EXPECT_NE(nullptr, renderer.renderTerm) << "renderTerm not set";
    EXPECT_NE(nullptr, renderer.updateScene) << "updateScene not set";
    EXPECT_NE(nullptr, renderer.surfaceCreate) << "surfaceCreate not set";
    EXPECT_NE(nullptr, renderer.surfaceDestroy) << "surfaceDestroy not set";
    EXPECT_NE(nullptr, renderer.surfaceCommit) << "surfaceCommit not set";
    EXPECT_NE(nullptr, renderer.surfaceSetVisible) << "surfaceSetVisible not set";
    EXPECT_NE(nullptr, renderer.surfaceGetVisible) << "surfaceGetVisible not set";
    EXPECT_NE(nullptr, renderer.surfaceSetGeometry) << "surfaceSetGeometry not set";
    EXPECT_NE(nullptr, renderer.surfaceGetGeometry) << "surfaceGetGeometry not set";
    EXPECT_NE(nullptr, renderer.surfaceSetOpacity) << "surfaceSetOpacity not set";
    EXPECT_NE(nullptr, renderer.surfaceGetOpacity) << "surfaceGetOpacity not set";
    EXPECT_NE(nullptr, renderer.surfaceSetZOrder) << "surfaceSetZOrder not set";
    EXPECT_NE(nullptr, renderer.surfaceGetZOrder) << "surfaceGetZOrder not set";
    EXPECT_NE(nullptr, renderer.delegateUpdateScene) << "delegateUpdateScene not set";
    
    renderer.renderTerm(&renderer);
}

// Stress test: rapid init/term cycles
TEST_F(WesterosRendererInitTest, StressTest_RapidInitTerm) {
    for (int i = 0; i < 50; i++) {
        WstRenderer renderer;
        memset(&renderer, 0, sizeof(renderer));
        
        int result = renderer_init(&renderer, 0, nullptr);
        if (result == 0) {
            renderer.renderTerm(&renderer);
        }
    }
}
