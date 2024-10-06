#version 430 core
// 410 for mac

layout(location=0) out uint lifeBoolOut;

in vec2 interUV;

uniform usampler2D lifeBool;
uniform float inc_x;
uniform float inc_y;

uniform vec2 randPoint;

#define RAD         1

// get pixel relative to pixel of interest
uint getP(int i, int j) {
    return uint(texture(lifeBool, interUV + vec2(i*inc_x, j*inc_y)));
}

// Gets a pixel and its symmetric pixel (negative of relative position)
uint getPiP(int i, int j) {
    return  uint(texture(lifeBool, interUV + vec2(i*inc_x, j*inc_y)))
          + uint(texture(lifeBool, interUV + vec2(-i*inc_x, -j*inc_y)));;
}


void main() {
    vec2 dist = abs(fract(randPoint) - interUV);
    dist = min(dist, 1.0-dist);

    if (length(dist) < 0.04) {
        lifeBoolOut = 1;
        return;
    }
    else if (length(dist) > 0.5) {
        lifeBoolOut = 0;
        return;
    }

        /// GET PIXEL OF INTEREST ///
    bool is_alive = uint(texture(lifeBool, interUV)) > 0;


        /// GET SUM OF NEARBY VALUES
    uint near_life = 0;
    // For perimeter around pixel, grab an L-shape of pixels, and the negative positon of the L shape, forming a square
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
