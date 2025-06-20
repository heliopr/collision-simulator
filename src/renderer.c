#include "renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLuint vao;
GLuint shader_program;

void renderer_init() {
    GLfloat verts[] = {
        0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        -1.0f, -1.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, -1.0f,
        0.0f, 1.0f, 0.0f
    };

    GLuint bufferID;
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, (char*)(sizeof(GLfloat)*2));

    const char *vertexShaderSource =
        "#version 430 core\n"
        "in vec3 pos;"
        "in vec3 vertexColor;"
        "out vec3 color;"
        "void main() {"
        "   gl_Position = vec4(pos, 1.0);"
        "   color = vertexColor;"
        "}";
    const char *fragmentShaderSource =
        "#version 430 core\n"
        "out vec4 fragColor;"
        "in vec3 color;"
        "void main() {"
        "   fragColor = vec4(color, 1.0);"
        "}";
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shader_program = glCreateProgram();
    glAttachShader(shader_program, fragmentShader);
    glAttachShader(shader_program, vertexShader);
    glLinkProgram(shader_program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLushort indices[] = {0,1,2, 0,3,4};
    GLuint indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void renderer_render() {
    glUseProgram(shader_program);
    glBindVertexArray(vao);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);
}