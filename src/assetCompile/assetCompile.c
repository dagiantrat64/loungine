/* Just know that I didn't do the best job at optimizing this since it doesn't matter anyway, assets will just be stored
 * as a big binary blob */

#include <stdio.h>
#define TOBJ_ENABLE_FILE_IO
#include "tinyobj/tiny_obj_c.h"
#include "../vertex.h"
#include "../renderer/instance/instance.h"

typedef struct {
	uint16_t index;
	uint16_t size;
	uint32_t offset;
} OffsetTableEnt;

typedef struct {
	char magic[2];
	uint16_t assetCount;
} Header;

// Only supports OBJs currently
void loadModel(const char *path) {
	tobj_scene_f scene;
	tobj_load_config config = tobj_default_config();

	// Triangulated vertices used
	config.triangulate = true;

	tobj_diag diag = {0};

	if (tobj_load_obj_from_file_f(&scene, path, &config, &diag) != TOBJ_OK) handleError(44);
	// Kind of a file read error since it means load from file failed

	uint32_t totalVertexCount = 0;

	FILE *fPtr = fopen(path, "wb");
	if (!fPtr) handleError(44);

	/*( static bool writeFile(const char *restrict path, char *restrict buf, const size_t size) {
		if (!buf) return false;
		if (!path) return false;

		FILE *fPtr = fopen(path, "wb");
		if (!fPtr) return false;

		if (!buf || size == 0) {
			goto cleanup;
		}
		if (fwrite(buf, 1, size, fPtr) != size) {
			goto cleanup;
		}

		fclose(fPtr);

		return true;

		cleanup:
			fclose(fPtr);
		return false;
	} */

	Header header = {
		.magic = "la",
		.assetCount = scene.num_shapes
	};
	fwrite(&header, sizeof(Header), 1, fPtr);

	// Fill out table at beginning of file
	for (uint32_t i = 0; i < scene.num_shapes; ++i) {
		OffsetTableEnt
		fwrite()
	}

	if (!totalVertexCount) handleError(44); // Also kind of a file read error

	writeFile();
}
