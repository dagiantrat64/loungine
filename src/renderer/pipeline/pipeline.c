#include "pipeline.h"

VkDescriptorSetLayout descriptorSet = VK_NULL_HANDLE;

void createDescriptorSetLayout() {
	// Uniform buffer at set 0 bind 0
	// Remember, set layout bindings are where the resources are bound, set layouts are the holders of them
	VkDescriptorSetLayoutBinding setLayoutBinding = {
		.binding = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
	};
	VkDescriptorSetLayoutCreateInfo setLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = 1,
		.pBindings = &setLayoutBinding
	};
	if (vkCreateDescriptorSetLayout(device, &setLayoutCreateInfo, NULL, &descriptorSet) != VK_SUCCESS) handleError(15);
}

void createGraphicsPipeline() {
	// For pipeline layout, only need to specify descriptor set layouts & push constant ranges
	VkPipelineLayoutCreateInfo pipelineLayoutCI = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		// Only one set layout needed, will need multiple later
		.setLayoutCount = 1,
		.pSetLayouts = &descriptorSet
	};
	if (vkCreatePipelineLayout(device, &pipelineLayoutCI, NULL, &pipelineLayout) != VK_SUCCESS) handleError(15);

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = 2
	};
}