#include <gtest/gtest.h>

#include "nexus_mock_control.h"
#include "westeros-gl.h"

#include <cstdlib>
#include <vector>

extern "C" bool _WstGLGetDisplayInfo(WstGLCtx *ctx, WstGLDisplayInfo *info);
extern "C" bool _WstGLGetDisplaySafeArea(WstGLCtx *ctx, int *x, int *y, int *w, int *h);
extern "C" bool _WstGLAddDisplaySizeListener(WstGLCtx *ctx, void *data, WstGLDisplaySizeCallback callback);
extern "C" bool _WstGLRemoveDisplaySizeListener(WstGLCtx *ctx, WstGLDisplaySizeCallback callback);

namespace
{
struct DisplayEvent
{
   int width;
   int height;
};

void recordDisplayEvent(void *userData, int width, int height)
{
   static_cast<std::vector<DisplayEvent> *>(userData)->push_back({width, height});
}

class WesterosGLBrcmTest : public ::testing::Test
{
protected:
   void SetUp() override
   {
      WstGLMockReset();
      unsetenv("WESTEROS_SECURE_GRAPHICS");
      unsetenv("WESTEROS_RENDER_PROTECTED_CONTENT");
      setenv("WESTEROS_GL_GRAPHICS_SD_USE_720", "1", 1);
      context = WstGLInit();
   }

   void TearDown() override
   {
      if (context)
      {
         WstGLTerm(context);
         context = nullptr;
      }
      unsetenv("WESTEROS_GL_GRAPHICS_SD_USE_720");
   }

   WstGLCtx *context = nullptr;
};

TEST_F(WesterosGLBrcmTest, InitializationProvidesDisplayInfoAndSmpteSafeArea)
{
   ASSERT_NE(nullptr, context);
   WstGLDisplayInfo info = {};
   ASSERT_TRUE(WstGLGetDisplayInfo(context, &info));
   EXPECT_EQ(1920, info.width);
   EXPECT_EQ(1080, info.height);
   EXPECT_EQ(96, info.safeArea.x);
   EXPECT_EQ(54, info.safeArea.y);
   EXPECT_EQ(1728, info.safeArea.w);
   EXPECT_EQ(972, info.safeArea.h);
   EXPECT_FALSE(info.secureGraphics);

   int x = 0, y = 0, width = 0, height = 0;
   ASSERT_TRUE(WstGLGetDisplaySafeArea(context, &x, &y, &width, &height));
   EXPECT_EQ(info.safeArea.x, x);
   EXPECT_EQ(info.safeArea.y, y);
   EXPECT_EQ(info.safeArea.w, width);
   EXPECT_EQ(info.safeArea.h, height);
}

TEST_F(WesterosGLBrcmTest, DisplayQueriesRejectInvalidArguments)
{
   WstGLDisplayInfo info = {};
   EXPECT_FALSE(WstGLGetDisplayInfo(nullptr, &info));
   EXPECT_FALSE(WstGLGetDisplayInfo(context, nullptr));
   EXPECT_FALSE(WstGLGetDisplaySafeArea(nullptr, nullptr, nullptr, nullptr, nullptr));
   int x = 0, y = 0, width = 0, height = 0;
   EXPECT_FALSE(WstGLGetDisplaySafeArea(context, &x, nullptr, &width, &height));
}

TEST_F(WesterosGLBrcmTest, SdDisplayUsesConfigured720WidthPolicy)
{
   ASSERT_NE(nullptr, context);
   unsetenv("WESTEROS_GL_GRAPHICS_SD_USE_720");
   WstGLMockTriggerDisplayChange(720, 480);

   WstGLDisplayInfo info = {};
   ASSERT_TRUE(WstGLGetDisplayInfo(context, &info));
   EXPECT_EQ(640, info.width);
   EXPECT_EQ(480, info.height);
}

TEST_F(WesterosGLBrcmTest, ListenerReceivesInitialAndChangedSizeOnly)
{
   ASSERT_NE(nullptr, context);
   std::vector<DisplayEvent> events;
   ASSERT_TRUE(WstGLAddDisplaySizeListener(context, &events, recordDisplayEvent));
   ASSERT_EQ(1u, events.size());
   EXPECT_EQ(1920, events[0].width);
   EXPECT_EQ(1080, events[0].height);

   EXPECT_FALSE(WstGLAddDisplaySizeListener(context, &events, recordDisplayEvent));
   WstGLMockTriggerDisplayChange(1920, 1080);
   EXPECT_EQ(1u, events.size());
   WstGLMockTriggerDisplayChange(1280, 720);
   ASSERT_EQ(2u, events.size());
   EXPECT_EQ(1280, events[1].width);
   EXPECT_EQ(720, events[1].height);

   EXPECT_TRUE(WstGLRemoveDisplaySizeListener(context, recordDisplayEvent));
   EXPECT_FALSE(WstGLRemoveDisplaySizeListener(context, recordDisplayEvent));
   WstGLMockTriggerDisplayChange(3840, 2160);
   EXPECT_EQ(2u, events.size());
}

TEST_F(WesterosGLBrcmTest, ListenerAndTermHandleNullInputs)
{
   EXPECT_FALSE(WstGLAddDisplaySizeListener(nullptr, nullptr, recordDisplayEvent));
   EXPECT_FALSE(WstGLRemoveDisplaySizeListener(nullptr, recordDisplayEvent));
   EXPECT_FALSE(WstGLRemoveDisplaySizeListener(context, nullptr));
   WstGLTerm(nullptr);
}

TEST_F(WesterosGLBrcmTest, CompatibilityEntryPointsForwardToPublicBehavior)
{
   WstGLDisplayInfo info = {};
   int x = 0, y = 0, width = 0, height = 0;
   std::vector<DisplayEvent> events;

   EXPECT_TRUE(_WstGLGetDisplayInfo(context, &info));
   EXPECT_TRUE(_WstGLGetDisplaySafeArea(context, &x, &y, &width, &height));
   EXPECT_EQ(info.safeArea.x, x);
   EXPECT_TRUE(_WstGLAddDisplaySizeListener(context, &events, recordDisplayEvent));
   EXPECT_TRUE(_WstGLRemoveDisplaySizeListener(context, recordDisplayEvent));
}

TEST_F(WesterosGLBrcmTest, TermRemovesListenersBeforeContextIsReinitialized)
{
   std::vector<DisplayEvent> events;
   ASSERT_TRUE(WstGLAddDisplaySizeListener(context, &events, recordDisplayEvent));
   WstGLTerm(context);
   context = nullptr;

   context = WstGLInit();
   ASSERT_NE(nullptr, context);
   WstGLMockTriggerDisplayChange(1600, 900);
   EXPECT_EQ(1u, events.size());
}

TEST_F(WesterosGLBrcmTest, NativeWindowLifecycleDelegatesToPlatform)
{
   ASSERT_NE(nullptr, context);
   void *window = WstGLCreateNativeWindow(context, 10, 20, 640, 360);
   ASSERT_NE(nullptr, window);
   EXPECT_EQ(1, WstGLMockGetWindowCreateCount());
   WstGLDestroyNativeWindow(context, window);
   EXPECT_EQ(1, WstGLMockGetWindowDestroyCount());

   EXPECT_EQ(nullptr, WstGLCreateNativeWindow(nullptr, 0, 0, 10, 10));
   WstGLDestroyNativeWindow(nullptr, window);
   EXPECT_EQ(1, WstGLMockGetWindowDestroyCount());
}

TEST_F(WesterosGLBrcmTest, PixmapCopiesInputAndReusesAllocationForMatchingDimensions)
{
   ASSERT_NE(nullptr, context);
   WstGLMockSurface source = {};
   WstGLMockSetInputSurfaceStatus(&source, 640, 360);
   void *pixmap = nullptr;

   ASSERT_TRUE(WstGLGetNativePixmap(context, &source, &pixmap));
   ASSERT_NE(nullptr, pixmap);
   EXPECT_EQ(1, WstGLMockGetPixmapCreateCount());
   EXPECT_EQ(1, WstGLMockGetBlitCount());
   EXPECT_NE(nullptr, WstGLGetEGLNativePixmap(context, pixmap));
   int width = 0, height = 0;
   WstGLGetNativePixmapDimensions(context, pixmap, &width, &height);
   EXPECT_EQ(640, width);
   EXPECT_EQ(360, height);

   WstGLMockSetInputSurfaceStatus(&source, 640, 360);
   ASSERT_TRUE(WstGLGetNativePixmap(context, &source, &pixmap));
   EXPECT_EQ(1, WstGLMockGetPixmapCreateCount());
   EXPECT_EQ(2, WstGLMockGetBlitCount());
   WstGLReleaseNativePixmap(context, pixmap);
   EXPECT_EQ(1, WstGLMockGetPixmapDestroyCount());
}

TEST_F(WesterosGLBrcmTest, PixmapRecreatesAllocationWhenDimensionsChange)
{
   ASSERT_NE(nullptr, context);
   WstGLMockSurface source = {};
   WstGLMockSetInputSurfaceStatus(&source, 640, 360);
   void *pixmap = nullptr;
   ASSERT_TRUE(WstGLGetNativePixmap(context, &source, &pixmap));

   WstGLMockSetInputSurfaceStatus(&source, 1280, 720);
   ASSERT_TRUE(WstGLGetNativePixmap(context, &source, &pixmap));
   EXPECT_EQ(2, WstGLMockGetPixmapCreateCount());
   EXPECT_EQ(1, WstGLMockGetPixmapDestroyCount());
   EXPECT_EQ(2, WstGLMockGetBlitCount());
   int width = 0, height = 0;
   WstGLGetNativePixmapDimensions(context, pixmap, &width, &height);
   EXPECT_EQ(1280, width);
   EXPECT_EQ(720, height);
   WstGLReleaseNativePixmap(context, pixmap);
   EXPECT_EQ(2, WstGLMockGetPixmapDestroyCount());
}

TEST_F(WesterosGLBrcmTest, PixmapAndEglAccessorsHandleNullContextAndPixmap)
{
   WstGLMockSurface source = {};
   WstGLMockSetInputSurfaceStatus(&source, 32, 16);
   void *pixmap = nullptr;
   EXPECT_FALSE(WstGLGetNativePixmap(nullptr, &source, &pixmap));
   EXPECT_EQ(nullptr, pixmap);
   EXPECT_EQ(nullptr, WstGLGetEGLNativePixmap(context, nullptr));
   WstGLReleaseNativePixmap(nullptr, nullptr);
}

TEST(WesterosGLBrcmInitFailureTest, AllocationFailureReturnsNullAndCanRecover)
{
   WstGLMockReset();
   WstGLMockFailNextAlloc();
   EXPECT_EQ(nullptr, WstGLInit());
   WstGLMockReset();
   WstGLCtx *context = WstGLInit();
   ASSERT_NE(nullptr, context);
   WstGLTerm(context);
}

TEST(WesterosGLBrcmInitFailureTest, GraphicsOpenFailureCleansUpAndReturnsNull)
{
   WstGLMockReset();
   WstGLMockFailNextGraphicsOpen();
   EXPECT_EQ(nullptr, WstGLInit());
}

TEST(WesterosGLBrcmSecurityTest, SecureGraphicsCapabilityIsPropagatedToDisplayInfo)
{
   WstGLMockReset();
   setenv("WESTEROS_SECURE_GRAPHICS", "1", 1);
   WstGLCtx *context = WstGLInit();
   ASSERT_NE(nullptr, context);

   WstGLDisplayInfo info = {};
   EXPECT_TRUE(WstGLGetDisplayInfo(context, &info));
   EXPECT_TRUE(info.secureGraphics);
   EXPECT_STREQ("1", std::getenv("WESTEROS_RENDER_PROTECTED_CONTENT"));

   WstGLTerm(context);
   unsetenv("WESTEROS_SECURE_GRAPHICS");
   unsetenv("WESTEROS_RENDER_PROTECTED_CONTENT");
}
}
