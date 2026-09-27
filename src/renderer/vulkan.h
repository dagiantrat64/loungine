#ifndef LOUNGINE_VULKAN_H
#define LOUNGINE_VULKAN_H

// Preprocessor Constants
#include <SDL2/SDL_video.h>
#include <cstddef>
#include <cstdlib>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <vulkan/vulkan_core.h>
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

#ifndef SDL3
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#else
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#endif

#include <cglm/struct.h>

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

uint32_t startX = 1280;
uint32_t startY = 720;
SDL_Window *window;

// Vulkan objects
VkInstance instance = VK_NULL_HANDLE;
VkPhysicalDevice *physicalDevices = NULL;
VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
VkSurfaceKHR surface = VK_NULL_HANDLE;
uint32_t qfIndex = UINT32_MAX;
VkDevice device = VK_NULL_HANDLE;
VkQueue queue = VK_NULL_HANDLE;
VkSwapchainKHR swapchain = VK_NULL_HANDLE;

// VLAs
VkImage *swapchainImages = NULL;

void handleError(int id) {

}

void cleanup();

#include "init.h"

#endif //LOUNGINE_VULKAN_H
