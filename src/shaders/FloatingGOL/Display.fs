#version 430 core
// 410 for mac

layout(location=0) out vec4 out_color;

in vec2 interUV;

uniform sampler2D life;


#define RAD         0


void main() {
    float plife = float(texture(life, interUV));
    out_color = vec4(plife, plife, plife, 1.0);
}
