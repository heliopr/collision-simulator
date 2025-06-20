#include "renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLuint vao = 0;
GLuint bufferID;
GLuint shader_program;

GLfloat verts[] = {
    0.0f, 1.0f,
    -1.0f, -1.0f,
    1.0f, -1.0f
};

const char* vertex_shader =
    "#version 410 core\n"
    "in vec3 vp;"
    "void main() {"
    "  gl_Position = vec4(vp, 1.0);"
    "}";

const char* fragment_shader =
    "#version 410 core\n"
    "out vec4 frag_colour;"
    "void main() {"
    "  frag_colour = vec4(1.0, 0.0, 0.0, 1.0);"
    "}";

void renderer_init() {
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);
}

void renderer_render() {
    glUseProgram(shader_program);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}