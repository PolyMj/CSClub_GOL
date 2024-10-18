#version 430 core
// 410 for mac

layout(location=0) out vec4 out_color;

in vec2 interUV;

uniform sampler2D life;


void main() {
    out_color = vec4(vec3(texture(life, interUV)), 1.0);
}
