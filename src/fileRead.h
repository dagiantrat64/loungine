#ifndef VULKANTUTORIAL_FILEREAD_H
#define VULKANTUTORIAL_FILEREAD_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

static long readFile(const char *restrict path, char **restrict buf) {
	if (!buf) return -1;
	if (!path) return -1;

	*buf = NULL;
	FILE *fPtr = fopen(path, "rb");
	if (!fPtr) return -1;

	if (fseek(fPtr, 0, SEEK_END) != 0) {
		goto cleanup;
	}
	long size = ftell(fPtr);
	if (size <= 0) {
		goto cleanup;
	}

	// fseek() used instead because rewind() has no error handling
	if (fseek(fPtr, 0, SEEK_SET) != 0) {
		goto cleanup;
	}

	*buf = malloc(size);
	if (!*buf) {
		goto cleanup;
	}
	if (fread(*buf, 1, size, fPtr) != size) {
		free(*buf);
		*buf = NULL;
		goto cleanup;
	}

	fclose(fPtr);

	return size;

	cleanup:
		fclose(fPtr);
		return -1;
}

static bool writeFile(const char *restrict path, char *restrict buf, const size_t size) {
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
}


#endif //VULKANTUTORIAL_FILEREAD_H
