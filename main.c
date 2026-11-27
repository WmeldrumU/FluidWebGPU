#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <emscripten/emscripten.h>

#include "lib/sdl3webgpu.h"
#include "webgpu/webgpu.h"

typedef struct UserData
{
    WGPUAdapter adapter;
    bool request_ended;
} UserData;

static void adapter_request_callback(WGPURequestAdapterStatus status,
                                     WGPUAdapter adapter, WGPUStringView message,
                                     WGPU_NULLABLE void *userdata1,
                                     WGPU_NULLABLE void *userdata2)
{
    UserData *user_data = (UserData *)(userdata1);
    SDL_assert(user_data);
    if (status == WGPURequestAdapterStatus_Success)
    {
        user_data->adapter = adapter;
    }
    else
    {
        printf("Could not get WebGPU adapter: %s", message.data);
    }
    user_data->request_ended = true;
}

int main(int argc, char **argv)
{
    // Set the hint BEFORE window creation or SDL_Init
    // SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        //     throw std::runtime_error(
        //         std::string("SDL_Init failed: ") + SDL_GetError());
        printf("SDL_Init failed: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // SDL_Log("Success");

    const char *compiler = "Unknown Compiler";

#if defined(__EMSCRIPTEN__)
    compiler = "Emscripten (Clang)";
#elif defined(__clang__)
    compiler = "Clang";
#elif defined(__GNUC__)
    compiler = "GCC";
#elif defined(_MSC_VER)
    compiler = "MSVC";
#endif

    // SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Build Compiler: %s\n", compiler);
    printf("Build Compiler: %s\n", compiler);

    SDL_Window *window = SDL_CreateWindow("FluidWebGPU", 1280, 720, SDL_WINDOW_RESIZABLE);
    // We create a descriptor
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = NULL;

    // We create the instance using this descriptor
    WGPUInstance instance = wgpuCreateInstance(&desc);
    SDL_assert(instance != NULL);
    WGPUSurface surface = SDL_GetWGPUSurface(instance, window);

    UserData user_data = {0};

    WGPURequestAdapterOptions options = {0};
    WGPURequestAdapterCallbackInfo callback_info = {
        .mode = WGPUCallbackMode_WaitAnyOnly,
        .callback = adapter_request_callback,
        .userdata1 = (void *)&user_data};

    wgpuInstanceRequestAdapter(instance, &options, callback_info);

#ifdef __EMSCRIPTEN__
    while (!user_data.request_ended)
    {
        emscripten_sleep(100);
    }
#endif // __EMSCRIPTEN__

    SDL_assert(user_data.request_ended);

    // renderer = SDL_CreateRenderer(window, nullptr);
    printf("HELLO Adapter\n");

    wgpuAdapterRelease(user_data.adapter);
    wgpuSurfaceRelease(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
    wgpuInstanceRelease(instance);
    return 0;
}

void main_loop()
{
}