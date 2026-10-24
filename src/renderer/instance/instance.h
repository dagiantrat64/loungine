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
#define CGLM_CONFIG_CLIP_CONTROL CGLM_CLIP_CONTROL_RH_ZO

// ...
#include <volk.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <cglm/struct.h>

#include <SDL3/SDL_vulkan.h>

extern uint32_t startWidth; // Currently stuck at 1280x720
extern uint32_t startHeight;
extern SDL_Window *window;
extern VkBool32 dedicatedTransferQueue;

// Vulkan objects
extern VkInstance instance;
extern VkPhysicalDevice *physicalDevices;
extern VkPhysicalDevice physicalDevice;
extern VkSurfaceKHR surface;
extern uint32_t graphicsQfIndex;
extern uint32_t transferQfIndex;
extern VkDevice device;
extern VkQueue graphicsQueue;
extern VkSwapchainKHR swapchain;
extern VkPipelineLayout pipelineLayout;
extern VkCommandPool transferCommandPool;
extern VkCommandBuffer transferCmdBuf;
// VLAs
extern VkImage *swapchainImages;
extern uint32_t swapchainImageCount;
extern VkImageView *swapchainImageViews;
/* Image view count determined by swapchainImageCount */

// Global handles
extern VkPhysicalDeviceMemoryProperties memProps;

void createInstance();
void createWindow();
void selectPhysicalDevice();
void createLogicalDevice();
void createSwapchain();
void createSwapchainImages(); // Also creates image views
void createCommandPool();

void cleanup();

#endif //LOUNGINE_VULKAN_H
