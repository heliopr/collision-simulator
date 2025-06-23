#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "renderer.h"

GLuint vao;
GLuint shader;
GLuint texture;

const float X_DELTA = 0.3f;
const int MAX_TRIS = 5;
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

void loadTexture() {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("textures/test.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        printf("Failed to load texture\n");
    }
    stbi_image_free(data);
}

void compileShaderProgram() {
    const char *vertexShaderSource =
        "#version 430 core\n"
        "in vec3 pos;"
        "in vec3 vertexColor;"
        "in vec2 textCoord;"
        "out vec3 color;"
        "out vec2 coord;"
        "uniform mat4 transform;"
        "void main() {"
        "   gl_Position = transform * vec4(pos, 1.0);"
        "   color = vertexColor;"
        "   coord = textCoord;"
        "}";
    const char *fragmentShaderSource =
        "#version 430 core\n"
        "out vec4 fragColor;"
        "in vec3 color;"
        "in vec2 coord;"
        "uniform sampler2D text;"
        "void main() {"
        "   fragColor = texture(text, coord) * vec4(color, 1.0);"
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
    glBufferData(GL_ARRAY_BUFFER, MAX_TRIS * sizeof(Triangle), NULL, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)(sizeof(GLfloat)*3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(GLfloat)));
}

void addTri() {
    if (triangles == MAX_TRIS)
        return;

    GLfloat x = -1 + triangles * X_DELTA;
    Triangle tri = {
        {
            {x, 1.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
            {0.0f, 1.0f}
        },

        {
            {x+X_DELTA, 1.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f}
        },

        {
            {x, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
            {0.0f, 0.0f}
        }
    };

    glBufferSubData(GL_ARRAY_BUFFER, triangles * sizeof(Triangle), sizeof(Triangle), tri);
    triangles++;
}

void renderer_init() {
    glEnable(GL_DEPTH_TEST);
    setupRenderer();
    loadTexture();
    compileShaderProgram();
}

void renderer_render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 transform;
    glm_mat4_identity(transform);
    glm_translate(transform, (vec3){0.5f, -0.5f, 0.0f});
    glm_rotate(transform, (float)glfwGetTime(), (vec3){0.0f, 0.0f, 1.0f});

    glUseProgram(shader);
    unsigned int transformLoc = glGetUniformLocation(shader, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, (const float*)transform);

    glBindVertexArray(vao);

    addTri();
    glDrawArrays(GL_TRIANGLES, 0, triangles * 3);
}