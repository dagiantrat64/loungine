#ifndef SRC_ASSET_H_
#define SRC_ASSET_H_

#include <stdint.h>

typedef struct {
	uint32_t vDataSize;
	uint32_t iDataSize;
	uint64_t offset;
} OffsetTableEnt;

typedef struct {
	char magic[6];
	uint16_t assetCount;
} Header;

#endif /* SRC_ASSET_H_ */
