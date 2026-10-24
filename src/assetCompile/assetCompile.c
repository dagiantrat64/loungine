/* Just know that I didn't do the best job at optimizing this since it doesn't matter anyway, assets will just be stored
 * as a big binary blob, also unsafe because does not free memory upon error. It terminates anyway so who cares. */

#include <SDL3/SDL_iostream.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

#include "tinyobj/tiny_obj_c.h"

#include "../asset.h"
#include "../vertex.h"
#include "../renderer/instance/instance.h"
#include "../handleError.h"

uint32_t getNextIndex(Vertex *vertices, uint32_t verticesSize, Vertex vertex) {
	for (uint32_t i = 0; i < (verticesSize / sizeof *vertices); ++i) {
		if (!memcmp(&vertices[i], &vertex, sizeof(Vertex))) return i;  // Fine b/c it's 32b
	}
	return UINT32_MAX; // Sentinel, out of uint16_t bounds so careful to not go over 65k vertices...
}

// Only supports OBJs currently
void loadModel(const char *path, const char *obj) {
	tobj_scene_f scene;
	tobj_load_config config = tobj_default_config();

	// Triangulated vertices used
	config.triangulate = true;

	tobj_diag diag = {0};

	if (tobj_load_obj_from_file_f(&scene, obj, &config, &diag) != TOBJ_OK) handleError(90);
	// Kind of a file read error since it means load from file failed

	FILE *fPtr = fopen(path, "wb");
	if (!fPtr) handleError(44);
	
	size_t offsetTableSize = scene.num_shapes * sizeof(OffsetTableEnt);
	OffsetTableEnt *offsetTable = malloc(offsetTableSize);
	if (!offsetTable) handleError(16);
	
	uint32_t startingOffset = offsetTableSize + sizeof (Header);
	uint32_t currentOffset = startingOffset; // Offset required by table
	fseek(fPtr, startingOffset, SEEK_CUR);
	for (uint32_t i = 0; i < scene.num_shapes; ++i) { // Multiple loops to reduce writes
		tobj_mesh_f mesh = scene.shapes[i].mesh;
		
		Vertex *vertices = malloc(mesh.num_indices * sizeof(Vertex));
		uint16_t *indices = malloc(mesh.num_indices * sizeof *indices);
		
		uint32_t uniqueVertexCount = 0;
		for (uint32_t j = 0; j < mesh.num_indices; ++j) {
			// Pointers supplied by tinyobj
			float *posPtr = scene.attrib.vertices.ptr;
			float *normPtr = scene.attrib.normals.ptr;
			float *uvPtr = scene.attrib.texcoords.ptr;
			
			tobj_index index = mesh.indices[j]; 
			
			uint32_t posIndex = index.vertex_index;
			uint32_t normIndex = index.normal_index;
			uint32_t uvIndex = index.texcoord_index;
			
			Vertex vertex = {
				// Pointer arithmetic used, since each member consists of multiple floats
				.pos = {
					posPtr[3 * posIndex], posPtr[3 * posIndex + 1], posPtr[3 * posIndex + 2]
				},
				.norm = {
					normPtr[3 * normIndex], normPtr[3 * normIndex + 1], normPtr[3 * normIndex + 2]
				},
				.uv = {
					uvPtr[2 * uvIndex], 1 - uvPtr[2 * uvIndex + 1] // Vertical UV inverted
				}
			};
			uint32_t nextIndex = getNextIndex(vertices, uniqueVertexCount, vertex);
			if (nextIndex == UINT32_MAX) { // UINT16_MAX is when the vertex is unique, so then add 
			// a new entry and index to that
				vertices[uniqueVertexCount] = vertex;
				indices[j] = j;
				uniqueVertexCount++;
			} else {
				indices[j] = nextIndex;
			}
		}
		uint32_t vBufSize = uniqueVertexCount * sizeof *vertices;
		uint32_t iBufSize = mesh.num_indices * sizeof *indices;
		// Write data, this might be slow...
		if (!fwrite(vertices, vBufSize, 1, fPtr)) handleError(44);
		if (!fwrite(indices, mesh.num_indices * sizeof *indices, 1, fPtr)) handleError(44);

		// Fill out table
		offsetTable[i] = (OffsetTableEnt){
			.offset = currentOffset,
			.vDataSize = vBufSize,
			.iDataSize = iBufSize
		};
		// Increment the offset by both the vertex and index buffer sizes since it accounts for both
		currentOffset += vBufSize + iBufSize;
		
		free(vertices);
		free(indices);
	}
	// Go back to the beginning and write header and table
	fseek(fPtr, 0, SEEK_SET);
	Header header = {
		.magic = "asset",
		.assetCount = scene.num_shapes
	};
	fwrite(&header, sizeof(Header), 1, fPtr);
	fwrite(offsetTable, offsetTableSize, 1, fPtr);
	
	tobj_diag_free(&diag, NULL);
	tobj_scene_free(&scene);
	
	fclose(fPtr);
	
	free(offsetTable);
}
