#version 430 core
// 410 mac

layout(location=0) out float lifeOut;

in vec3 outColor;

void main() {
    lifeOut = (outColor.r + outColor.g + outColor.b) / 3.0;
}
