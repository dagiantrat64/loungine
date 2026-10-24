#ifndef LOUNGINECL_ASSETLOAD_H
#define LOUNGINECL_ASSETLOAD_H

#include "../instance/instance.h"
#include <stdint.h>

extern VkBuffer vertexBuffer;
extern uint32_t assetCount;

uint32_t *loadScene(char *file);
uint32_t *loadAsset(char *file, int index);

#endif //LOUNGINECL_ASSETLOAD_H
