#ifndef LOUNGINE_VULKAN_H
#define LOUNGINE_VULKAN_H

// Preprocessor Constants
#define APP_NAME "Untitled"
#define APP_VERSION 0
#define ENGINE_NAME "loungine"
#define ENGINE_VERSION 0
#define VULKAN_VERSION VK_API_VERSION_1_2

#include <stdint.h>

// Pre-Include Defines
#define VOLK_IMPLEMENTATION
#define CGLM_CONFIG_CLIP_CONTROL CGLM_CLIP_CONTROL_RH_ZO

// Includes
#include <volk.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <cglm/struct.h>

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "vulkan.h"

uint32_t startX = 1280;
uint32_t startY = 720;
SDL_Window *window;

#include "init.h"



#endif //LOUNGINE_VULKAN_H