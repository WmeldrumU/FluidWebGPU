cmake_minimum_required(VERSION 4.0)
project(FluidWebGPU_project LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(SDL3 REQUIRED)
set(DAWN_FETCH_DEPENDENCIES ON)
add_subdirectory("dawn" EXCLUDE_FROM_ALL)

add_executable(FluidWebGPU main.c)



if(EMSCRIPTEN)
  message("Building with emscripten")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  target_compile_options(FluidWebGPU PRIVATE "-sUSE_SDL=0") # do not use emscriptens sdl
  target_include_directories(FluidWebGPU PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dawn)
endif()

target_link_libraries(FluidWebGPU SDL3::SDL3 webgpu_dawn)


