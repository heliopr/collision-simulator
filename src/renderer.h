#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

void renderer_init(GLFWwindow *w);
void renderer_render(double deltaTime, vec3 cameraPos, vec3 cameraFront, vec3 cameraUp);

#endif