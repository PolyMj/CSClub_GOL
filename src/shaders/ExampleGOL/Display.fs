#version 430 core
// 410 for mac

layout(location=0) out vec4 out_color;

in vec2 interUV;

uniform usampler2D lifeBool;


void main() {
    uint life = uint(texture(lifeBool, interUV));
    if (life > 0) {
        out_color = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }
    out_color = vec4(0.0, 0.0, 0.0, 1.0);
}
