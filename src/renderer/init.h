#ifndef LOUNGINE_INIT_H
#define LOUNGINE_INIT_H

inline uint32_t getDeviceScore(VkPhysicalDeviceType deviceType);

void createInstance() {
	// load standard pfns
	if (!volkInitialize()) handleError(32);
	// All set to constants
	VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.apiVersion = VULKAN_VERSION,
		.applicationVersion = APP_VERSION,
		.pApplicationName = APP_NAME,
		.engineVersion = ENGINE_VERSION,
		.pEngineName = ENGINE_NAME
	};

	uint32_t extensionCount = 0;
	// Account for SDL2 and SDL3
	#ifndef SDL3
	const char **requiredExtensions;
	if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, requiredExtensions)) handleError(42);
	#else
	const char *const *requiredExtensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount); // i need...
	#endif
	// Since there's only 1 requested extension and it could be nothing everything was wrapped in an ifndef
	const char **extBuffer; // just in case this will exist

	// set the initial, constant fields
	VkInstanceCreateInfo instanceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo
	};
	// If doing a debug build, enable validation
#ifndef NDEBUG
	{
		const char *requestedExtensions[] = {
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
		}; // I want...
		const size_t requiredExtensionsSize = extensionCount * sizeof *requiredExtensions;
		// combined buffer for extensions
		extBuffer = malloc(requiredExtensionsSize + sizeof requestedExtensions);
		if (!extBuffer) handleError(16);
		// copy the required extensions
		memcpy(extBuffer, requiredExtensions, requiredExtensionsSize);
		// then the requested ones
		memcpy(extBuffer + requiredExtensionsSize, requestedExtensions, sizeof requestedExtensions);

		// Now for layers, since no required layers only 1 buffer necessary
		const char *requestedLayers[] = {
			"VK_LAYER_KHRONOS_validation"
		};

		// Set validation related fields
		instanceCreateInfo.ppEnabledExtensionNames = extBuffer;
		instanceCreateInfo.ppEnabledLayerNames = requestedLayers;
		instanceCreateInfo.enabledLayerCount = 1;
		instanceCreateInfo.enabledExtensionCount = extensionCount + 1;
	}
	// And for release skip
#else
	instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions;
	instanceCreateInfo.enabledExtensionCount = extensionCount;
#endif
	// If an Apple device, we need the portability enumeration bit for MoltenVK
#ifdef APPLE
	instanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

	if (vkCreateInstance(&instanceCreateInfo, NULL, &instance) != VK_SUCCESS) exit(32);

	// Load instance PFNs
	volkLoadInstance(instance);
#ifndef NDEBUG
	free(extBuffer);
#endif
}

void createWindow() {
	// init sdl
	if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) handleError(42);
	// and actually create the window...
	#ifndef SDL3
	SDL_CreateWindow(APP_NAME, 0, 0, startX, startY, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
	#else
	window = SDL_CreateWindow(APP_NAME, startX, startY, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
	#endif
	if (!window) handleError(42);
	#ifndef SDL3
	if (!SDL_Vulkan_CreateSurface(window, instance, &surface))
	#else
	if (!SDL_Vulkan_CreateSurface(window, instance, NULL, &surface)) handleError(42);
	#endif
}

void selectPhysicalDevice() {
	// Standard 2 call for enumeration, first a count and then a VLA of the objects
	/* Here physical devices are enumerated to be selected. This is the initial automatic selection that should be
	 * changeable by the end user. */
	uint32_t deviceCount = 0;
	if (vkEnumeratePhysicalDevices(instance, &deviceCount, NULL) != VK_SUCCESS) handleError(17);
	if (!deviceCount) handleError(17);
	physicalDevices = malloc(deviceCount * sizeof *physicalDevices);
	if (vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices) != VK_SUCCESS) handleError(17);

	uint32_t selectedDeviceScore = 0;
	uint32_t selectedDeviceMaxImage = 0;
	VkDeviceSize selectedVRAM = 0;
	uint32_t selectedDeviceIndex;

	for (uint32_t i = 0; i < deviceCount; ++i) {
	    // Check required feature compatibility first, then find the fittest device
		const char *requiredExts[] = {
			VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
			// Extension instead of feature if targeting Vulkan 1.2 as a baseline
			VK_KHR_SWAPCHAIN_EXTENSION_NAME, // Needed in general
			VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME, // Also needed for 1.2
			VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME, // " "
		};
		uint32_t reqExtCount = sizeof requiredExts / sizeof *requiredExts;

		// Note: VkExtensionProperties is per extension
		uint32_t extCount = 0;
		if (vkEnumerateDeviceExtensionProperties(physicalDevices[i], NULL, &extCount, NULL) != VK_SUCCESS)
			handleError(17);
		if (!extCount) handleError(17);
		VkExtensionProperties *extProperties = malloc(sizeof *extProperties * extCount);
		if (!extProperties) handleError(16);
		if (vkEnumerateDeviceExtensionProperties(physicalDevices[i], NULL, &extCount, extProperties) != VK_SUCCESS)
			handleError(17);

		// Check if all extensions are listed
		uint32_t supportedExts = 0;
		for (size_t j = 0; j < extCount; ++j) {
			for (size_t k = 0; k < sizeof requiredExts / sizeof *requiredExts; ++k) {
				if (!strcmp(extProperties[j].extensionName, requiredExts[k])) {
					supportedExts++;
				}
			}
		}
		free(extProperties);
		if (supportedExts < reqExtCount) {
			continue;
		}

		// Now check if said extensions are ACTUALLY supported
		// NOTE: When adding required extensions, if they have features, make sure to check for them here
		VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR
		};
		VkPhysicalDeviceSynchronization2FeaturesKHR synchronization2Features = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR,
			.pNext = &dynamicRenderingFeatures
		};
		VkPhysicalDeviceExtendedDynamicState2FeaturesEXT extendedDynamicStateFeatures = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT,
			.pNext = &synchronization2Features
		};
		VkPhysicalDeviceFeatures2 features = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
			.pNext = &extendedDynamicStateFeatures
		};
		vkGetPhysicalDeviceFeatures2(physicalDevices[i], &features);

		if (!dynamicRenderingFeatures.dynamicRendering ||
		    !synchronization2Features.synchronization2 ||
		    !extendedDynamicStateFeatures.extendedDynamicState2 ||
		    !features.features.geometryShader)
			continue;

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);

		// Since targeting (late) 1.2 as baseline, need to check for support
		if (deviceProperties.apiVersion < VK_API_VERSION_1_2) continue;

		// Check format compatibility
        uint32_t surfaceFormatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, NULL);
        if (!surfaceFormatCount) handleError(17);
        VkSurfaceFormatKHR *surfaceFormats = malloc(surfaceFormatCount * sizeof *surfaceFormats));
        if (!surfaceFormats) handleError(16);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats);

        // TODO: Add array of required formats and alternates for each format unsupported, maybe outside of this,
        // and add respective color space array if using color space other than nonlinear SRGB

        bool formatSupported = false;
        for (uint32_t i = 0; i < surfaceFormatCount; ++i) {
            if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                formatSupported = true;
            }
        }
        free(surfaceFormats);
        if (!formatSupported) continue;

		// Check against previous devices and see if selected is more fit
		uint32_t currentDeviceScore = getDeviceScore(deviceProperties.deviceType);
		if (selectedDeviceScore == currentDeviceScore) {
			if (deviceProperties.limits.maxImageDimension2D < selectedDeviceMaxImage) {
				continue;
			}

			VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
			vkGetPhysicalDeviceMemoryProperties(physicalDevices[i], &deviceMemoryProperties);

			for (uint32_t j = 0; j < deviceMemoryProperties.memoryHeapCount; ++j) {
				if (deviceMemoryProperties.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
					if (deviceMemoryProperties.memoryHeaps[j].size > selectedVRAM) {
						selectedVRAM = deviceMemoryProperties.memoryHeaps[j].size;
						selectedDeviceIndex = i;
						selectedDeviceScore = currentDeviceScore;
						selectedDeviceMaxImage = deviceProperties.limits.maxImageDimension2D;
					}
				}
			}
			// Maybe add more checks?
		} else if (selectedDeviceScore < currentDeviceScore) {
			VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
			vkGetPhysicalDeviceMemoryProperties(physicalDevices[i], &deviceMemoryProperties);

			uint32_t currentVRAM = 0;
			for (uint32_t j = 0; j < deviceMemoryProperties.memoryHeapCount; ++j) {
				if (deviceMemoryProperties.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
					if (deviceMemoryProperties.memoryHeaps[j].size > currentVRAM) {
						currentVRAM = deviceMemoryProperties.memoryHeaps[j].size;
					}
				}
			}
			currentVRAM = selectedVRAM;
			selectedDeviceScore = currentDeviceScore;
			selectedDeviceIndex = i;
		}
	}
	physicalDevice = physicalDevices[selectedDeviceIndex];
}

// Ranks devices for selectPhysicalDevices to give priority if the device type doesn't match, just for convenience
inline uint32_t getDeviceScore(VkPhysicalDeviceType deviceType) {
	// Higher is better for some reason
	switch (deviceType) {
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			return 3;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			return 0;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			return 4;
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			return 1;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			return 2;
		default:
			return 0;
	}
}

void createLogicalDevice() {
	// Same extensions as in the physical device selection function
	const char *requiredExts[] = {
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
		// Extension instead of feature if targeting Vulkan 1.2 as a baseline
		VK_KHR_SWAPCHAIN_EXTENSION_NAME, // Needed in general
		VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME, // Also needed for 1.2
		VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME, // " "
	};
	VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
		.dynamicRendering = VK_TRUE
	};
	VkPhysicalDeviceSynchronization2FeaturesKHR synchronization2Features = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR,
		.pNext = &dynamicRenderingFeatures,
		.synchronization2 = VK_TRUE
	};
	VkPhysicalDeviceExtendedDynamicState2FeaturesEXT extendedDynamicStateFeatures = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT,
		.pNext = &synchronization2Features,
		.extendedDynamicState2 = VK_TRUE
	};
	// But not here because they're required by the spec, and 1.2 support was checked
	VkPhysicalDeviceVulkan12Features vulkan12Features = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
		.pNext = &extendedDynamicStateFeatures,
		.timelineSemaphore = VK_TRUE
	};
	VkPhysicalDeviceVulkan11Features vulkan11Features = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
		.pNext = &vulkan12Features,
		.shaderDrawParameters = VK_TRUE
	};
	VkPhysicalDeviceFeatures2 features = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
		.pNext = &vulkan11Features
	};

	// Enumerate the properties of the queue families...
	uint32_t queueFamilyPropertyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, NULL);
	if (!queueFamilyPropertyCount) handleError(17);
	VkQueueFamilyProperties *queueFamilyProperties = malloc(queueFamilyPropertyCount * sizeof *queueFamilyProperties);
	if (!queueFamilyProperties) handleError(16);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties);

	// ...and find one that supports graphics

	for (uint32_t i = 0; i < queueFamilyPropertyCount; ++i) {
		VkBool32 presentSupported;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupported);
		if (!presentSupported) continue;
		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			qfIndex = i;
			break;
		}
	}

	float qfPriorities[] = {1.0f};
	VkDeviceQueueCreateInfo queueCI = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = qfIndex,
		.pQueuePriorities = qfPriorities,
		.queueCount = 1
	};

	VkDeviceCreateInfo deviceCI = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = &features,
		.enabledExtensionCount = sizeof requiredExts / sizeof *requiredExts,
		.ppEnabledExtensionNames = requiredExts,
		.pQueueCreateInfos = &queueCI,
		.queueCreateInfoCount = 1
	};
	if (vkCreateDevice(physicalDevice, &deviceCI, NULL, &device) != VK_SUCCESS) handleError(15);
	// Queue is acquired, not created, and therefore no function return code, just check if non-null.
	vkGetDeviceQueue(device, qfIndex, 0, &queue);
	if (!queue) handleError(15);

	// Load PFNs from Volk
	volkLoadDevice(device);

	free(queueFamilyProperties);
}

void createSwapchain() {
    VkSurfaceCapabilitiesKHR surfaceCaps;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCaps) != VK_SUCCESS) handleError(17);

    // Find image count
    uint32_t minImageCount = (surfaceCaps.minImageCount > 2) ? surfaceCaps.minImageCount : 2;
    uint32_t finalImageCount = minImageCount + 1;

    // Mostly predefined, should add checks for each in future
    VkSwapchainCreateInfoKHR swapchainCI = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = finalImageCount,
        .imageFormat = VK_FORMAT_B8G8R8A8_SRGB, // NOTE: Set this to format variable when alternate formats found
        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent = (VkExtent2D){startX, startY},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = surfaceCaps.currentTransform,
        .presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR, // NOTE: Change for VSync on/off
        .clipped = VK_TRUE, // Turn off if presenting all pixels or reading all pixels before present
    };
    if (vkCreateSwapchainKHR(device, &swapchainCI, NULL, &swapchain) != VK_SUCCESS) handleError(15);

    // Now we need swapchain images
    uint32_t swapchainImageCount = 0;
    if (vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, NULL) != VK_SUCCESS) handleError(17);
    if (!swapchainImageCount) handleError(17);
    if (!(swapchainImages = malloc(swapchainImageCount * sizeof *swapchainImages))) handleError(16);
    if (vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, NULL) != VK_SUCCESS) handleError(17);
}

#endif //LOUNGINE_INIT_H
