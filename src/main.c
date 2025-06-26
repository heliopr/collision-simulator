#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <cglm/cglm.h>

#include "renderer.h"

#define CAMERA_SPEED 2.5
#define CAMERA_SENSITIVITY 0.1f

vec3 cameraPos = (vec3){0.0f, 0.0f, 3.0f};
vec3 cameraFront = (vec3){0.0f, 0.0f, -1.0f};
vec3 cameraUp = (vec3){0.0f, 1.0f, 0.0f};

int firstMouse = 1;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 0.0f;
float lastY = 0.0f;
float fov = 45.0f;

void sizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, double deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }

    double delta = CAMERA_SPEED * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm_vec3_muladds(cameraFront, delta, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm_vec3_mulsubs(cameraFront, delta, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        vec3 right;
        glm_vec3_cross(cameraFront, cameraUp, right);
        glm_vec3_normalize(right);
        glm_vec3_mulsubs(right, delta, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        vec3 right;
        glm_vec3_cross(cameraFront, cameraUp, right);
        glm_vec3_normalize(right);
        glm_vec3_muladds(right, delta, cameraPos);
    }
}

void mouseCallback(GLFWwindow* window, double x, double y) {
    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = 0;
    }
  
    float dx = x - lastX;
    float dy = lastY - y; 
    lastX = x;
    lastY = y;

    dx *= CAMERA_SENSITIVITY;
    dy *= CAMERA_SENSITIVITY;

    yaw += dx;
    pitch += dy;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }
    
    vec3 direction;
    direction[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
    direction[1] = sin(glm_rad(pitch));
    direction[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
    glm_vec3_normalize_to(direction, cameraFront);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.6);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4.6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Collision Simulation", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, sizeCallback);

    // RENDERER INIT
    renderer_init(window);

    double deltaTime = 0;
    double lastFrame = glfwGetTime();
    while(!glfwWindowShouldClose(window))
    {
        double current = glfwGetTime();
        deltaTime = current - lastFrame;
        lastFrame = current;

        // PROCESS INPUT
        glfwPollEvents();
        processInput(window, deltaTime);

        // RENDERER RENDER
        renderer_render(deltaTime, cameraPos, cameraFront, cameraUp);

        glfwSwapBuffers(window);
    }
  
    glfwTerminate();
    return 0;
}