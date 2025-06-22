#version 430 core
in vec3 pos;
in vec3 vertexColor;
out vec3 color;
void main() {
    gl_Position = vec4(pos, 1.0);
    color = vertexColor;
}