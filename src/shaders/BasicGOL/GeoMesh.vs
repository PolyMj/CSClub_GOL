#version 430 core
// 410 for mac

layout(location=0) in vec3 position;

uniform mat4 modelMat;
uniform mat4 viewProjMat;

// out type name;

void main() {
    vec4 pos = vec4(position, 1.0);
    vec4 worldPos = modelMat*pos;

    gl_Position = viewProjMat*worldPos;
}

