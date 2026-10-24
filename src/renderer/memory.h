#ifndef SRC_RENDERER_MEMORY_H_
#define SRC_RENDERER_MEMORY_H_

#include "instance/instance.h"
#include "../handleError.h"
#include <stdint.h>
#include <stdio.h>

inline void allocateMemory(VkMemoryRequirements memReq, VkMemoryPropertyFlags memTypeProps, VkDeviceMemory *pMemory) {
	uint32_t memTypeIndex = UINT32_MAX;
	for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
		/* Mask with (i)th bit set to check compatibility with current type and a bitwise and of 
		the current type's properties compared with the original to ensure correct type*/
		if ((memReq.memoryTypeBits & (1 << i)) && 
		((memTypeProps & memProps.memoryTypes[i].propertyFlags) == memTypeProps)) {
			memTypeIndex = i;
			break;
		}
	}
	if (memTypeIndex == UINT32_MAX) handleError(5);
	
	VkMemoryAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memReq.size,
		.memoryTypeIndex = memTypeIndex
	};
	if (vkAllocateMemory(device, &allocInfo, NULL, pMemory) != VK_SUCCESS) handleError(16);
}

#endif /* SRC_RENDERER_MEMORY_H_ */
