#ifndef RENDERER_H
#define RENDERER_H

#include <cglm/cglm.h>

typedef struct Vertex {
    vec3 position;
    vec3 color;
    vec2 texture;
} Vertex;

typedef Vertex Triangle[3];

void renderer_init();
void renderer_render();

#endif