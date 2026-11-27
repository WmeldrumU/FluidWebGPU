cmake_minimum_required(VERSION 4.0)
project(FluidWebGPU_project LANGUAGES C CXX)   # CXX: Dawn (native) and emdawnwebgpu (web) are C++

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(DAWN_RELEASE "v20261120.121236" CACHE STRING "Dawn release used for both native and web")

# Conan dependencies
find_package(SDL3 REQUIRED)
find_package(nuklear REQUIRED)

add_executable(FluidWebGPU
    main.c
    lib/sdl3webgpu.c
)
target_include_directories(FluidWebGPU PRIVATE lib)
target_link_libraries(FluidWebGPU PRIVATE SDL3::SDL3 nuklear::nuklear)

if(EMSCRIPTEN)
    set(DAWN_PORT "${CMAKE_CURRENT_SOURCE_DIR}/emdawnwebgpu-${DAWN_RELEASE}.remoteport.py"
        CACHE FILEPATH "emdawnwebgpu port file (remote port, or a locally built emdawnwebgpu_pkg)")
    if(NOT EXISTS "${DAWN_PORT}")
        message(FATAL_ERROR
            "Missing ${DAWN_PORT}\n"
            "Download emdawnwebgpu-${DAWN_RELEASE}.remoteport.py from "
            "https://github.com/google/dawn/releases/tag/${DAWN_RELEASE}")
    endif()

    set(WGPU_PORT "--use-port=${DAWN_PORT}:cpp_bindings=false")
    target_compile_options(FluidWebGPU PRIVATE ${WGPU_PORT})
    target_link_options(FluidWebGPU PRIVATE
        ${WGPU_PORT}
        -sALLOW_MEMORY_GROWTH=1
        # assertions for extra runtime check (i.e. invalid memory access, misused apis) 
        # gsource map for stepping through C code instead of .js when debugging
        "$<$<CONFIG:Debug>:-sASSERTIONS=2 -gsource-map>"
        #  --closure=1 runs Google's Closure Compiler over the generated .js file to minify it and remove unused code.
        "$<$<NOT:$<CONFIG:Debug>>:--closure=1>"
        -sASYNCIFY=1   # only if you block on futures with wgpuInstanceWaitAny
    )
    set_target_properties(FluidWebGPU PROPERTIES
        SUFFIX ".html"
        LINKER_LANGUAGE CXX
    )

else()
    # Native - Use Dawn from the submodule
    set(DAWN_FETCH_DEPENDENCIES ON  CACHE BOOL "" FORCE)
    set(DAWN_BUILD_SAMPLES      OFF CACHE BOOL "" FORCE)
    set(DAWN_BUILD_TESTS        OFF CACHE BOOL "" FORCE)
    set(DAWN_BUILD_BENCHMARKS   OFF CACHE BOOL "" FORCE)
    set(TINT_BUILD_TESTS        OFF CACHE BOOL "" FORCE)
    set(TINT_BUILD_CMD_TOOLS    OFF CACHE BOOL "" FORCE)
    set(DAWN_USE_GLFW           OFF CACHE BOOL "" FORCE)

    add_subdirectory(dawn EXCLUDE_FROM_ALL)
    target_link_libraries(FluidWebGPU PRIVATE webgpu_dawn)
endif()