#version 430 core
// 410 mac

layout(location=0) out vec3 lifeOut;

in vec3 interColor;

void main() {
    lifeOut = interColor;
}
