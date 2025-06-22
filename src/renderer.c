#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "renderer.h"

GLuint vao;
GLuint shader;

const float X_DELTA = 0.3f;
const int MAX_TRIS = 5;
const int TRIANGLE_BYTE_SIZE = sizeof(GLfloat) * 6 * 3;
int triangles = 0;

int checkStatus(GLuint objectID, PFNGLGETSHADERIVPROC ivFun, PFNGLGETSHADERINFOLOGPROC infoLogFun, GLenum statusType) {
    GLint status;
    ivFun(objectID, statusType, &status);
    if (status != GL_TRUE) {
        GLint logLen;
        ivFun(objectID, GL_INFO_LOG_LENGTH, &logLen);

        char buff[logLen];
        infoLogFun(objectID, logLen, NULL, buff);
        printf("ERROR COMPILING OBJECT %d: %s\n", objectID, buff);
        return 0;
    }
    return 1;
}

int checkShader(GLuint shaderID) {
    return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

int checkProgram(GLuint programID) {
    return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

void compileShaderProgram() {
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

    if (!checkShader(vertexShader) || !checkShader(fragmentShader))
        return;

    shader = glCreateProgram();
    glAttachShader(shader, fragmentShader);
    glAttachShader(shader, vertexShader);
    glLinkProgram(shader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!checkProgram(shader))
        return;
}

void setupRenderer() {
    GLuint bufferID;
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, MAX_TRIS * TRIANGLE_BYTE_SIZE, NULL, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*6, NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*6, (char*)(sizeof(GLfloat)*3));
}

void addTri() {
    if (triangles == MAX_TRIS)
        return;

    GLfloat x = -1 + triangles * X_DELTA;
    GLfloat tri[] = {
        x, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        x+X_DELTA, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        x, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };

    glBufferSubData(GL_ARRAY_BUFFER, triangles * TRIANGLE_BYTE_SIZE, TRIANGLE_BYTE_SIZE, tri);
    triangles++;
}

void renderer_init() {
    glEnable(GL_DEPTH_TEST);
    setupRenderer();
    compileShaderProgram();
}

void renderer_render() {
    glClear(GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    glBindVertexArray(vao);

    addTri();
    glDrawArrays(GL_TRIANGLES, 0, triangles * 3);
}