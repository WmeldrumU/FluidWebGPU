#include "sdl3webgpu.h"

#include <SDL3/SDL.h>
#include <webgpu/webgpu.h>

WGPUSurface SDL_GetWGPUSurface(WGPUInstance instance, SDL_Window *window)
{
    WGPUSurfaceDescriptor desc = {0};   /* label left empty */

#if defined(SDL_PLATFORM_EMSCRIPTEN)
    (void)window;
    const char *selector = SDL_GetHint(SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR);
    if (!selector) selector = "#canvas";

    WGPUEmscriptenSurfaceSourceCanvasHTMLSelector src = {
        .chain    = { .sType = WGPUSType_EmscriptenSurfaceSourceCanvasHTMLSelector },
        .selector = { selector, WGPU_STRLEN },
    };
    desc.nextInChain = &src.chain;
    return wgpuInstanceCreateSurface(instance, &desc);

#elif defined(SDL_PLATFORM_WIN32)
    SDL_PropertiesID props = SDL_GetWindowProperties(window);
    WGPUSurfaceSourceWindowsHWND src = {
        .chain     = { .sType = WGPUSType_SurfaceSourceWindowsHWND },
        .hinstance = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, NULL),
        .hwnd      = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL),
    };
    desc.nextInChain = &src.chain;
    return wgpuInstanceCreateSurface(instance, &desc);

#elif defined(SDL_PLATFORM_MACOS)
    /* Window should be created with SDL_WINDOW_METAL. The view lives as long as the window. */
    SDL_MetalView view = SDL_Metal_CreateView(window);
    WGPUSurfaceSourceMetalLayer src = {
        .chain = { .sType = WGPUSType_SurfaceSourceMetalLayer },
        .layer = SDL_Metal_GetLayer(view),
    };
    desc.nextInChain = &src.chain;
    return wgpuInstanceCreateSurface(instance, &desc);

#elif defined(SDL_PLATFORM_LINUX)
    SDL_PropertiesID props  = SDL_GetWindowProperties(window);
    const char      *driver = SDL_GetCurrentVideoDriver();

    if (driver && SDL_strcmp(driver, "wayland") == 0) {
        WGPUSurfaceSourceWaylandSurface src = {
            .chain   = { .sType = WGPUSType_SurfaceSourceWaylandSurface },
            .display = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL),
            .surface = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL),
        };
        desc.nextInChain = &src.chain;
        return wgpuInstanceCreateSurface(instance, &desc);
    }
    if (driver && SDL_strcmp(driver, "x11") == 0) {
        WGPUSurfaceSourceXlibWindow src = {
            .chain   = { .sType = WGPUSType_SurfaceSourceXlibWindow },
            .display = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL),
            .window  = (uint64_t)SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0),
        };
        desc.nextInChain = &src.chain;
        return wgpuInstanceCreateSurface(instance, &desc);
    }
    SDL_SetError("SDL_GetWGPUSurface: unsupported video driver '%s'", driver ? driver : "(null)");
    return NULL;

#else
#error "SDL_GetWGPUSurface: unsupported platform"
#endif
}