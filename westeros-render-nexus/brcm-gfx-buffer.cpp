#include "brcm-gfx-buffer.h"
#include "protocol/brcm-gfx-buffer-server-protocol.h"
#include "nexus_platform.h"
#include "wayland-server.h"
#include <cstdio>
#include <cassert>

struct brcm_gfx_buffer
{
   struct wl_display *display;
   struct wl_global *gfxbuf;
};

static void WstBGBDestroyBuffer(struct wl_resource *resource)
{
   auto ptr = static_cast<std::shared_ptr<NEXUS_Surface> *>(wl_resource_get_user_data(resource));
   delete ptr;
}

static void WstIBGBDestroy(struct wl_client *client,
                           struct wl_resource *resource)
{
   wl_resource_destroy(resource);
}

static const struct wl_buffer_interface brcm_gfx_buffer_interface = {
    WstIBGBDestroy};

static void WstIBGBCreate(struct wl_client *client, struct wl_resource *resource,
                          uint32_t id, uint32_t format, uint32_t width, uint32_t height,
                          uint32_t flags, uint32_t pitch, uint32_t size, int32_t fd)
{
   struct wl_resource *buffer;
   NEXUS_MemoryBlockHandle memoryHandle;
   NEXUS_SurfaceCreateSettings surfaceCreateSettings;
   NEXUS_SurfaceHandle surfaceHandle;
   std::shared_ptr<NEXUS_Surface> *ptr = nullptr;

   buffer = wl_resource_create(client, &wl_buffer_interface, 1, id);
   if (!buffer)
      goto failed;

   memoryHandle = NEXUS_Platform_CloneMemoryFromDescriptor(fd);
   if (!memoryHandle)
      goto cleanup_resource;

   NEXUS_Surface_GetDefaultCreateSettings(&surfaceCreateSettings);
   surfaceCreateSettings.pixelFormat = (NEXUS_PixelFormat)format;
   surfaceCreateSettings.width = width;
   surfaceCreateSettings.height = height;
   surfaceCreateSettings.compatibility.graphicsv3d = true;
   surfaceCreateSettings.pMemory = NULL;
   surfaceCreateSettings.pixelMemory = NULL;
   surfaceCreateSettings.pixelMemory = memoryHandle;
   surfaceHandle = NEXUS_Surface_Create(&surfaceCreateSettings);
   if (!surfaceHandle)
      goto cleanup_memory;

   try
   {
      auto deleter = [](NEXUS_SurfaceHandle surface)
      {
         if (!surface)
            return;
         NEXUS_SurfaceCreateSettings settings;
         NEXUS_Surface_GetCreateSettings(surface, &settings);
         NEXUS_Surface_Destroy(surface);
         if (settings.pixelMemory)
            NEXUS_MemoryBlock_Free(settings.pixelMemory);
      };
      ptr = new std::shared_ptr<NEXUS_Surface>(surfaceHandle, deleter);
   }
   catch (...)
   {
      goto cleanup_resource;
   };

   close(fd);
   wl_resource_set_implementation(buffer, &brcm_gfx_buffer_interface,
                                  ptr, WstBGBDestroyBuffer);
   return;

cleanup_memory:
   NEXUS_MemoryBlock_Free(memoryHandle);
cleanup_resource:
   wl_resource_destroy(buffer);
failed:
   wl_resource_post_no_memory(resource);
   if (fd >= 0)
      close(fd);
   return;
}

std::shared_ptr<NEXUS_Surface> WstBGBBufferGet(struct wl_resource *resource)
{
   if (!resource)
      return nullptr;

   if (!wl_resource_instance_of(resource, &wl_buffer_interface, &brcm_gfx_buffer_interface))
      return nullptr;

   auto ptr = static_cast<std::shared_ptr<NEXUS_Surface> *>(wl_resource_get_user_data(resource));
   assert(ptr);

   return *ptr;
}

static void WstBGBBind(struct wl_client *client, void *data, uint32_t version, uint32_t id)
{
   struct wl_resource *resource;

   printf("WstBGBBind: client %p data %p version %d id %d", client, data, version, id);

   resource = wl_resource_create(client, &brcm_gfx_interface, version, id);
   if (!resource) {
      wl_client_post_no_memory(client);
      return;
   }

   static const struct brcm_gfx_interface brcm_gfx = {
      WstIBGBCreate
   };

   wl_resource_set_implementation(resource, &brcm_gfx, data, nullptr);
}

brcm_gfx_buffer* WstBGBInit( struct wl_display *display )
{
   brcm_gfx_buffer *gfxbuf;

   printf("brcm_gfx_buffer: WstBGBInit: enter: display %p\n", display);
   gfxbuf = (struct brcm_gfx_buffer *)calloc(1, sizeof(struct brcm_gfx_buffer));
   if (!gfxbuf)
      goto exit;

   gfxbuf->display = display;
   gfxbuf->gfxbuf = wl_global_create(display, &brcm_gfx_interface, 2, nullptr, WstBGBBind);

exit:
   printf("brcm_gfx_buffer: WstBGBInit: exit: display %p gfxbuf %p\n", display, gfxbuf);
   return gfxbuf;
}

void WstBGBUninit(struct brcm_gfx_buffer *gfxbuf)
{
   if (gfxbuf)
   {
      wl_global_destroy(gfxbuf->gfxbuf);
      free(gfxbuf);
   }
}
