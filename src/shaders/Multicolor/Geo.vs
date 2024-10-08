#version 430 core
// 410 for mac

layout(location=0) in vec3 position;
layout(location=1) in vec4 color;

uniform mat4 modelMat;
uniform mat4 otherTransform;

// out type name;
out vec3 interColor;

void main() {
    interColor = vec3(color);

    gl_Position = otherTransform*modelMat*vec4(position, 1.0);
}

