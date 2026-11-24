#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>

#include "lib/sdl3webgpu.h"
#include "webgpu/webgpu.h"

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

    SDL_Window *window = SDL_CreateWindow("FluidWebGPU", 1280, 720, SDL_WINDOW_RESIZABLE);
    // We create a descriptor
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = NULL;

    // We create the instance using this descriptor
    WGPUInstance instance = wgpuCreateInstance(&desc);
    SDL_Surface *surface = SDL_GetWGPUSurface(instance, window);
    // renderer = SDL_CreateRenderer(window, nullptr);
    printf("HELLO FluidWebGPU\n");
    return 0;
}

void main_loop()
{
}