//TODO: account for vertex and index buffers and deduplication

#include "assetLoad.h"

#include "../../handleError.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan_core.h> // Remove pls
#include "../../asset.h"
#include "../instance/instance.h"
#include "../pipeline/pipeline.h"

#include "../memory.h"

VkBuffer vertexBuffer = VK_NULL_HANDLE;
VkBuffer stagingBuffer = VK_NULL_HANDLE;
VkDeviceMemory stagingBufMemory = VK_NULL_HANDLE;
VkDeviceMemory vertBufMemory = VK_NULL_HANDLE;
void *stagingBufMapped = NULL;
OffsetTableEnt *offsetTable;
uint32_t assetCount = 0;

void createStagingBuffer(uint32_t size) {
	VkBufferCreateInfo stagingBufferCI = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};
	if (vkCreateBuffer(device, &stagingBufferCI, NULL, &stagingBuffer) != VK_SUCCESS) 
		handleError(15);
	
	VkMemoryRequirements stagingBufMemReq = {};
	vkGetBufferMemoryRequirements(device, stagingBuffer, &stagingBufMemReq);
	
	// Allocate on host visible to be able to map and coherent so no flush needed, should be changed to allow for
	// non-coherent memory in the future
	VkMemoryPropertyFlags propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	allocateMemory(stagingBufMemReq, propertyFlags, &stagingBufMemory);
	
	if (vkBindBufferMemory(device, stagingBuffer, stagingBufMemory, 0)) handleError(16);
	
	// Set stagingBufMapped to the mapped pointer. If does not work, except
	if (vkMapMemory(device, stagingBufMemory, 0, VK_WHOLE_SIZE, 0, &stagingBufMapped) != VK_SUCCESS) 
		handleError(16);
}

void copyToVertexBuffer(uint32_t size) {
	VkBufferCreateInfo vertexBufferCI = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};
	if (vkCreateBuffer(device, &vertexBufferCI, NULL, &vertexBuffer) != VK_SUCCESS) handleError(15);
	
	// Data given fragmented for fast read, should change in the future though, for now regional 
	// copy is required
	
	// Nothing needed for memReq, allocateMemory handles everything
	VkMemoryRequirements vertexBufMemReq = {};
	vkGetBufferMemoryRequirements(device, vertexBuffer, &vertexBufMemReq);
	
	// Device local bc it's faster
	allocateMemory(vertexBufMemReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertBufMemory);
	
	VkCommandBufferBeginInfo transferCmdBufBeginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
		// No one time submit flag
	};
	vkBeginCommandBuffer(transferCmdBuf, &transferCmdBufBeginInfo);
	
	
	for (uint32_t i = 0; i < assetCount; ++i) {
		
	}
	
	vkCmdCopyBuffer(transferCmdBuf, stagingBuffer, vertexBuffer, )
	
	VkBufferMemoryBarrier2 memoryBarrier = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
	};
	
	handleError(15);
}

uint32_t *loadAssetGroup(char *file) {
	FILE *fPtr = fopen(file, "rb");
	
	Header header;
	if (!fread(&header, sizeof(Header), 1, fPtr)) handleError(22);
	if (strcmp("asset", header.magic)) handleError(99);
	
	if (!fread(&offsetTable, sizeof(OffsetTableEnt), header.assetCount, fPtr)) handleError(22);
	
	// Get total size of all the data stored in the entire file (excluding the header  & table ofc)
	uint64_t sceneTotalSize = 0;
	for (uint32_t i = 0; i < header.assetCount; ++i) {
		sceneTotalSize += offsetTable[i].vDataSize + offsetTable[i].iDataSize;
	}
	if (!sceneTotalSize) handleError(99);
	
	createStagingBuffer(sceneTotalSize);
	if (!stagingBufMapped) handleError(17);
	
	fread(stagingBufMapped, sceneTotalSize, 1, fPtr);
	
	
}

/*
 * Should not be used to grab assets between scenes unless absolutely necessary,
 * instead should be used with an individual asset file that contains assets used
 * between scenes anyway. 
 */
uint32_t *loadAsset(char *file, int indexV) {}