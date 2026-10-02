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

extern uint32_t startWidth; // Currently stuck at 1280x720
extern uint32_t startHeight;
extern SDL_Window *window;

 // Vulkan objects
extern VkInstance instance;
extern VkPhysicalDevice *physicalDevices;
extern VkPhysicalDevice physicalDevice;
extern VkSurfaceKHR surface;
extern uint32_t qfIndex; // Should use different qfs like dedicated transfer
extern VkDevice device;
extern VkQueue queue;
extern VkSwapchainKHR swapchain;
extern VkPipelineLayout pipelineLayout;
// VLAs
extern VkImage *swapchainImages;
extern uint32_t swapchainImageCount;
extern VkImageView *swapchainImageViews;
// Image view count determined by swapchainImageCount

void handleError(int id);
void createInstance();
void createWindow();
void selectPhysicalDevice();
void createLogicalDevice();
void createSwapchain();
void createSwapchainImages(); // Also creates image views

void cleanup();

#endif //LOUNGINE_VULKAN_H
