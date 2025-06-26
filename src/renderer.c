#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "renderer.h"
#include <cglm/cglm.h>

GLFWwindow *window;
GLuint vao;
GLuint shader;
GLuint texture;

vec3 positions[] = {
    {0.0f, 0.0f, 0.0f},
    {2.0f, 5.0f, -15.0f},
    {-1.5f, -2.2f, -2.5f},
    {-3.8f, -2.0f, -12.3f},
    {2.4f, -0.4f, -3.5f},
    {-1.7f, 3.0f, -7.5f},
    {1.3f, -2.0f, -2.5f},
    {1.5f, 2.0f, -2.5},
    {1.5f, 0.2f, -1.5f},
    {-1.3f, 1.0f, -1.5f}
};

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
        "in vec2 textCoord;"
        "out vec2 coord;"
        "uniform mat4 model;"
        "uniform mat4 view;"
        "uniform mat4 projection;"
        "void main() {"
        "   gl_Position = projection * view * model * vec4(pos, 1.0);"
        "   coord = textCoord;"
        "}";
    const char *fragmentShaderSource =
        "#version 430 core\n"
        "out vec4 fragColor;"
        "in vec2 coord;"
        "uniform sampler2D text;"
        "void main() {"
        "   fragColor = texture(text, coord);"
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
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    GLuint bufferID;
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (char*)(sizeof(GLfloat)*3));
}

void renderer_init(GLFWwindow *w) {
    window = w;

    glEnable(GL_DEPTH_TEST);
    setupRenderer();
    loadTexture();
    compileShaderProgram();
}

void renderer_render(double deltaTime, vec3 cameraPos, vec3 cameraFront, vec3 cameraUp) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    mat4 projection;
    glm_mat4_identity(projection);
    glm_perspective(glm_rad(45.0f), (float)width/(float)height, 0.1f, 100.0f, projection);
    unsigned int projectionLoc  = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
    
    mat4 view;
    glm_mat4_identity(view);
    vec3 at;
    glm_vec3_add(cameraPos, cameraFront, at);
    glm_lookat(cameraPos, at, cameraUp, view);
    unsigned int viewLoc  = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

    glUseProgram(shader);
    unsigned int modelLoc = glGetUniformLocation(shader, "model");
    glBindVertexArray(vao);
    for (int i = 0; i < 10; i++) {
        mat4 model;
        glm_mat4_identity(model);
        glm_translate(model, positions[i]);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}