cmake_minimum_required(VERSION 4.3)
project(loungine C)

set(CMAKE_C_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
        volk
        GIT_REPOSITORY https://github.com/zeux/volk.git
        GIT_TAG master
)

FetchContent_MakeAvailable(volk)
add_executable(loungine
        src/renderer/main.c
        src/renderer/vulkan.h
)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)

target_link_libraries(loungine PRIVATE SDL3::SDL3 volk m)