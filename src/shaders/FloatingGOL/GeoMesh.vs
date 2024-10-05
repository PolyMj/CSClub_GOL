#version 430 core
// 410 for mac

layout(location=0) in vec3 position;
layout(location=1) in vec4 color;

uniform mat4 modelMat;
uniform mat4 otherTransform;

// out type name;
out vec3 outColor;

void main() {
    vec4 pos = vec4(position, 1.0);
    vec4 worldPos = modelMat*pos;

    outColor = vec3(color);

    gl_Position = otherTransform*worldPos;
}

