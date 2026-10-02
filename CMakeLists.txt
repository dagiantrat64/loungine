cmake_minimum_required(VERSION 4.3)
project(loungineCL C)

set(CMAKE_C_STANDARD 17)

add_executable(renderer
        src/renderer/main.c
        src/renderer/instance/instance.c
        src/renderer/pipeline/pipeline.c
        src/renderer/assetLoad/assetLoad.c
)

add_executable(assetCompile
        src/assetCompile/assetCompile.c
        src/assetCompile/tinyobj/tiny_obj_c.c
        src/assetCompile/tinyobj/tobj_tess.c
)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)
find_package(volk REQUIRED)

target_link_libraries(renderer PRIVATE SDL3::SDL3 volk m)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/assets")