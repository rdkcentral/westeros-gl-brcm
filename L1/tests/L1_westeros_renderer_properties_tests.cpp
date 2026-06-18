/*
 * L1 Unit Tests for Westeros Renderer Nexus - Surface Properties
 * Target: surfaceSetVisible, surfaceGetVisible, surfaceSetGeometry, surfaceGetGeometry,
 *         surfaceSetOpacity, surfaceGetOpacity, surfaceSetZOrder, surfaceGetZOrder
 */

#include <gtest/gtest.h>
#include "westeros-render.h"

// External mock functions
extern "C" {
    extern void MockNexus_reset();
    extern void mock_nxclient_reset();
}

class WesterosRendererPropertiesTest : public ::testing::Test {
protected:
    WstRenderer *renderer;
    WstRenderSurface *surface;
    
    void SetUp() override {
        MockNexus_reset();
        mock_nxclient_reset();
        
        renderer = new WstRenderer();
        memset(renderer, 0, sizeof(WstRenderer));
        
        int result = renderer_init(renderer, 0, nullptr);
        ASSERT_EQ(0, result);
        
        surface = renderer->surfaceCreate(renderer);
        ASSERT_NE(nullptr, surface);
    }
    
    void TearDown() override {
        if (renderer && surface) {
            renderer->surfaceDestroy(renderer, surface);
        }
        if (renderer) {
            if (renderer->renderTerm) {
                renderer->renderTerm(renderer);
            }
            delete renderer;
        }
    }
};

// ========== Visibility Tests ==========

TEST_F(WesterosRendererPropertiesTest, SetVisible_True) {
    ASSERT_NO_THROW({
        renderer->surfaceSetVisible(renderer, surface, true);
    });
}

TEST_F(WesterosRendererPropertiesTest, SetVisible_False) {
    ASSERT_NO_THROW({
        renderer->surfaceSetVisible(renderer, surface, false);
    });
}

TEST_F(WesterosRendererPropertiesTest, GetVisible_Default) {
    bool visible = false;
    bool result = renderer->surfaceGetVisible(renderer, surface, &visible);
    
    EXPECT_TRUE(result);
    // Check if visible is set to some value
}

TEST_F(WesterosRendererPropertiesTest, SetAndGetVisible) {
    // Set visible to true
    renderer->surfaceSetVisible(renderer, surface, true);
    
    bool visible = false;
    bool result = renderer->surfaceGetVisible(renderer, surface, &visible);
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(visible);
    
    // Set visible to false
    renderer->surfaceSetVisible(renderer, surface, false);
    
    result = renderer->surfaceGetVisible(renderer, surface, &visible);
    EXPECT_TRUE(result);
    EXPECT_FALSE(visible);
}

TEST_F(WesterosRendererPropertiesTest, GetVisible_NullOutput) {
    bool result = renderer->surfaceGetVisible(renderer, surface, nullptr);
    
    // Should handle gracefully or return false
    EXPECT_FALSE(result);
}

// ========== Geometry Tests ==========

TEST_F(WesterosRendererPropertiesTest, SetGeometry_ValidValues) {
    ASSERT_NO_THROW({
        renderer->surfaceSetGeometry(renderer, surface, 100, 200, 1920, 1080);
    });
}

TEST_F(WesterosRendererPropertiesTest, SetGeometry_ZeroValues) {
    ASSERT_NO_THROW({
        renderer->surfaceSetGeometry(renderer, surface, 0, 0, 0, 0);
    });
}

TEST_F(WesterosRendererPropertiesTest, SetGeometry_NegativePosition) {
    ASSERT_NO_THROW({
        renderer->surfaceSetGeometry(renderer, surface, -100, -50, 800, 600);
    });
}

TEST_F(WesterosRendererPropertiesTest, GetGeometry_Default) {
    int x = -1, y = -1, width = -1, height = -1;
    
    ASSERT_NO_THROW({
        renderer->surfaceGetGeometry(renderer, surface, &x, &y, &width, &height);
    });
    
    // Values should be set to something
}

TEST_F(WesterosRendererPropertiesTest, SetAndGetGeometry) {
    // Set geometry
    renderer->surfaceSetGeometry(renderer, surface, 250, 300, 1280, 720);
    
    int x = 0, y = 0, width = 0, height = 0;
    renderer->surfaceGetGeometry(renderer, surface, &x, &y, &width, &height);
    
    EXPECT_EQ(250, x);
    EXPECT_EQ(300, y);
    EXPECT_EQ(1280, width);
    EXPECT_EQ(720, height);
}

TEST_F(WesterosRendererPropertiesTest, SetGeometry_LargeValues) {
    ASSERT_NO_THROW({
        renderer->surfaceSetGeometry(renderer, surface, 0, 0, 3840, 2160);
    });
    
    int x, y, width, height;
    renderer->surfaceGetGeometry(renderer, surface, &x, &y, &width, &height);
    
    EXPECT_EQ(3840, width);
    EXPECT_EQ(2160, height);
}

TEST_F(WesterosRendererPropertiesTest, GetGeometry_NullOutputs_NoCrash) {
    ASSERT_NO_THROW({
        renderer->surfaceGetGeometry(renderer, surface, nullptr, nullptr, nullptr, nullptr);
    });
}

// ========== Opacity Tests ==========

TEST_F(WesterosRendererPropertiesTest, SetOpacity_ValidValues) {
    ASSERT_NO_THROW({
        renderer->surfaceSetOpacity(renderer, surface, 0.5f);
    });
}

TEST_F(WesterosRendererPropertiesTest, SetOpacity_Zero) {
    ASSERT_NO_THROW({
        renderer->surfaceSetOpacity(renderer, surface, 0.0f);
    });
}

TEST_F(WesterosRendererPropertiesTest, SetOpacity_One) {
    ASSERT_NO_THROW({
        renderer->surfaceSetOpacity(renderer, surface, 1.0f);
    });
}

TEST_F(WesterosRendererPropertiesTest, GetOpacity_Default) {
    float opacity = -1.0f;
    
    ASSERT_NO_THROW({
        renderer->surfaceGetOpacity(renderer, surface, &opacity);
    });
    
    // Should be set to valid value
    EXPECT_GE(opacity, 0.0f);
    EXPECT_LE(opacity, 1.0f);
}

TEST_F(WesterosRendererPropertiesTest, SetAndGetOpacity) {
    renderer->surfaceSetOpacity(renderer, surface, 0.75f);
    
    float opacity = 0.0f;
    renderer->surfaceGetOpacity(renderer, surface, &opacity);
    
    EXPECT_FLOAT_EQ(0.75f, opacity);
}

TEST_F(WesterosRendererPropertiesTest, GetOpacity_NullOutput_NoCrash) {
    ASSERT_NO_THROW({
        renderer->surfaceGetOpacity(renderer, surface, nullptr);
    });
}

// ========== Z-Order Tests ==========

TEST_F(WesterosRendererPropertiesTest, SetZOrder_ValidValues) {
    ASSERT_NO_THROW({
        renderer->surfaceSetZOrder(renderer, surface, 0.5f);
    });
}

TEST_F(WesterosRendererPropertiesTest, SetZOrder_Zero) {
    ASSERT_NO_THROW({
        renderer->surfaceSetZOrder(renderer, surface, 0.0f);
    });
}

TEST_F(WesterosRendererPropertiesTest, SetZOrder_One) {
    ASSERT_NO_THROW({
        renderer->surfaceSetZOrder(renderer, surface, 1.0f);
    });
}

TEST_F(WesterosRendererPropertiesTest, SetZOrder_Negative) {
    ASSERT_NO_THROW({
        renderer->surfaceSetZOrder(renderer, surface, -1.0f);
    });
}

TEST_F(WesterosRendererPropertiesTest, GetZOrder_Default) {
    float z = -999.0f;
    
    ASSERT_NO_THROW({
        renderer->surfaceGetZOrder(renderer, surface, &z);
    });
    
    // Should be set to some value
}

TEST_F(WesterosRendererPropertiesTest, SetAndGetZOrder) {
    renderer->surfaceSetZOrder(renderer, surface, 0.25f);
    
    float z = 0.0f;
    renderer->surfaceGetZOrder(renderer, surface, &z);
    
    EXPECT_FLOAT_EQ(0.25f, z);
}

TEST_F(WesterosRendererPropertiesTest, GetZOrder_NullOutput_NoCrash) {
    ASSERT_NO_THROW({
        renderer->surfaceGetZOrder(renderer, surface, nullptr);
    });
}

// ========== Combined Properties Tests ==========

TEST_F(WesterosRendererPropertiesTest, SetAllProperties) {
    // Set all properties
    renderer->surfaceSetVisible(renderer, surface, true);
    renderer->surfaceSetGeometry(renderer, surface, 100, 200, 1920, 1080);
    renderer->surfaceSetOpacity(renderer, surface, 0.8f);
    renderer->surfaceSetZOrder(renderer, surface, 0.5f);
    
    // Verify all properties
    bool visible = false;
    renderer->surfaceGetVisible(renderer, surface, &visible);
    EXPECT_TRUE(visible);
    
    int x, y, width, height;
    renderer->surfaceGetGeometry(renderer, surface, &x, &y, &width, &height);
    EXPECT_EQ(100, x);
    EXPECT_EQ(200, y);
    EXPECT_EQ(1920, width);
    EXPECT_EQ(1080, height);
    
    float opacity;
    renderer->surfaceGetOpacity(renderer, surface, &opacity);
    EXPECT_FLOAT_EQ(0.8f, opacity);
    
    float z;
    renderer->surfaceGetZOrder(renderer, surface, &z);
    EXPECT_FLOAT_EQ(0.5f, z);
}

TEST_F(WesterosRendererPropertiesTest, ModifyPropertiesMultipleTimes) {
    // Modify each property multiple times
    for (int i = 0; i < 5; i++) {
        renderer->surfaceSetGeometry(renderer, surface, i * 10, i * 20, 800 + i * 100, 600 + i * 100);
        renderer->surfaceSetOpacity(renderer, surface, (float)i / 10.0f);
        renderer->surfaceSetZOrder(renderer, surface, (float)i / 10.0f);
    }
    
    // Final verification
    int x, y, width, height;
    renderer->surfaceGetGeometry(renderer, surface, &x, &y, &width, &height);
    EXPECT_EQ(40, x);
    EXPECT_EQ(80, y);
}

// ========== NULL Surface Tests ==========

TEST_F(WesterosRendererPropertiesTest, SetProperties_NullSurface_NoCrash) {
    ASSERT_NO_THROW({
        renderer->surfaceSetVisible(renderer, nullptr, true);
        renderer->surfaceSetGeometry(renderer, nullptr, 0, 0, 100, 100);
        renderer->surfaceSetOpacity(renderer, nullptr, 0.5f);
        renderer->surfaceSetZOrder(renderer, nullptr, 0.5f);
    });
}

TEST_F(WesterosRendererPropertiesTest, GetProperties_NullSurface_NoCrash) {
    bool visible;
    int x, y, width, height;
    float opacity, z;
    
    ASSERT_NO_THROW({
        renderer->surfaceGetVisible(renderer, nullptr, &visible);
        renderer->surfaceGetGeometry(renderer, nullptr, &x, &y, &width, &height);
        renderer->surfaceGetOpacity(renderer, nullptr, &opacity);
        renderer->surfaceGetZOrder(renderer, nullptr, &z);
    });
}
