#version 430 core
// 410 for mac

layout(location=0) in vec3 position;

uniform mat4 modelMat;
uniform mat4 otherTransforms;

void main() {
    gl_Position = otherTransforms*modelMat*vec4(position, 1.0);
}

