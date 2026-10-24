#ifndef LOUNGINECL_STRUCT_H
#define LOUNGINECL_STRUCT_H

#include <cglm/struct.h>

typedef struct {
	vec3s pos;
	vec3s norm;
	vec2s uv;
} Vertex;

typedef struct {
	mat4s model;
	mat4s view;
	mat4s proj;
} UBO; // Uniform Buffer Object

#endif //LOUNGINECL_STRUCT_H
