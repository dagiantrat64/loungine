#ifndef LOUNGINE_INIT_H
#define LOUNGINE_INIT_H

static void createWindow() {
	// init sdl
	if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) exit(42);
	// and actually create the window...
	window = SDL_CreateWindow(APP_NAME, startX, startY, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
	if (!window) exit(42);
}

static void createInstance() {
	// give vulkan's nosy ass some info on ur app
	VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.apiVersion = VULKAN_VERSION,
		.applicationVersion = APP_VERSION,
		.pApplicationName = APP_NAME,
		.engineVersion = ENGINE_VERSION,
		.pEngineName = ENGINE_NAME
	};

	uint32_t extensionCount = 0;
	const char *const *requiredExtensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount); // i need...
	const char *requestedExtensions[] = {
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
	}; // i want...
	size_t requiredExtensionsSize = extensionCount * sizeof *requiredExtensions;
	// combined buffer for extensions
	const char **extensionBuffer = malloc(requiredExtensionsSize + sizeof requestedExtensions);
	if (!extensionBuffer) exit(16);
	// copy the required extensions
	memcpy(extensionBuffer, requiredExtensions, requiredExtensionsSize);
	// then the requested
	memcpy(extensionBuffer + requiredExtensionsSize, requestedExtensions, sizeof requestedExtensions);

	VkInstanceCreateInfo instanceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
	};
}

#endif //LOUNGINE_INIT_H
