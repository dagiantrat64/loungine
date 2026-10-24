#ifndef LOUNGINECL_PIPELINE_H
#define LOUNGINECL_PIPELINE_H

#include "../instance/instance.h"

extern VkBuffer VertexBuffer;
extern VkDescriptorSetLayout descriptorSet;


void createGraphicsPipeline();
void createDescriptorSetLayout();

#endif //LOUNGINECL_PIPELINE_H
