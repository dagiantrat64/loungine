#include "../fileRead.h"
#define TOBJ_ENABLE_FILE_IO
#include "tinyobj/tiny_obj_c.h"

// Only supports OBJs currently
void loadModel(const char *path) {
	char *buf = NULL; // Unallocated because set upon read
	readFile(path, &buf);
	tobj_scene scene;
	tobj_load_config config = tobj_default_config();



	tobj_load_obj_from_file_f();

	writeFile();
}