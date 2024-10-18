#version 430 core
// 410 for mac

layout(location=0) out uint lifeBoolOut;

in vec2 interUV;

uniform usampler2D lifeBool;
uniform float inc_x;
uniform float inc_y;

uniform vec2 epicenter;

#define RAD         1
#define EPIC_MIN    0.02
#define EPIC_MAX    0.55


// Gets a pixel and its symmetric pixel (negative of relative position)
uint getPiP(int i, int j) {
    return  uint(texture(lifeBool, interUV + vec2(i*inc_x, j*inc_y)))
          + uint(texture(lifeBool, interUV + vec2(-i*inc_x, -j*inc_y)));;
}


void main() {

        /// CHECK RELATION TO EPICENTER ///
    vec2 travel = abs(fract(epicenter) - interUV);
    float dist = length(min(travel, 1.0-travel));

    if (dist < EPIC_MIN) {
        lifeBoolOut = 1;
        return;
    }
    else if (dist > EPIC_MAX) {
        lifeBoolOut = 0;
        return;
    }
        /// END EPICENTER ///


        /// GET SUM OF NEIGHBORHOOD ///
    uint near_life = 0;
    // For perimeter around pixel with ~radius "r", grab an L-shape of pixels, and the negative positon of the L shape, forming a square
    for (int r = 1; r <= RAD; ++r) {
        // Get pixels along x/y axis
        near_life += getPiP(r,0);
        near_life += getPiP(0,r);
        
        // For all x values > 0 at the top of the perimeter
        for (int i = 1; i <= RAD; ++i) {
            // Get pixels at at (x,top)
            near_life += getPiP(i,r);
            near_life += getPiP(-i,r);
        }

        // For all RAD > y values > 1 at the side of the perimeter
        for (int j = 1; j < RAD; ++j) {
            // Get pixels at (side, y)
            near_life += getPiP(r,j);
            near_life += getPiP(r,-j);
        }
    }
        /// END SUM ///


        /// CELL LOGIC ///

    // Get pixel of interest
    bool is_alive = uint(texture(lifeBool, interUV)) > 0;

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
