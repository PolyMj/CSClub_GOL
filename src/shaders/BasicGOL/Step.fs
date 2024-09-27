#version 430 core
// 410 for mac

layout(location=0) out uint lifeBoolOut;

in vec2 interUV;

uniform usampler2D lifeBool;
uniform float inc_x;
uniform float inc_y;

#define RAD         1


void main() {
    bool is_alive = uint(texture(lifeBool, interUV)) > 0;

    uint near_life = 0;
    for (int i = 0; i <= RAD; ++i) {
        for (int j = (i == 0 ? 1 : 0); j <= RAD; ++j) {
            float iinc = i*inc_x, jinc = j*inc_y;
            near_life += uint(texture(lifeBool, interUV + vec2(iinc, jinc)));
            near_life += uint(texture(lifeBool, interUV + vec2(iinc, -jinc)));
            near_life += uint(texture(lifeBool, interUV + vec2(-iinc, jinc)));
            near_life += uint(texture(lifeBool, interUV + vec2(-iinc, -jinc)));
        }
    }

    if (is_alive) {
        if (near_life < 2) {
            lifeBoolOut = 0; return;
        }
        else if (near_life < 4) {
            lifeBoolOut = 1; return;
        }
        else {
            lifeBoolOut = 0; return;
        }
    }
    else {
        if (near_life == 3) {
            lifeBoolOut = 1; return;
        }
        else {
            lifeBoolOut = 0; return;
        }
    }
}
