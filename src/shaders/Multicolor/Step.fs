#version 430 core
// 410 for mac

layout(location=0) out vec3 lifeOut;

in vec2 interUV;

uniform sampler2D life;
uniform float inc_x;
uniform float inc_y;

#define RAD         7


#define SAFE_INC    0.1
#define AGE_RATE    0.0002
#define REV_RATE    0.99 // Gets flickery past 0.99

#define SPREAD      0.001

#define SAFE_LOWER  0.3
#define SAFE_HIGHER 0.55
#define LIFE_MAX    1.0

#define LIFE_THRESH 0.0

#define REVIVE_LOW  0.5
#define REVIVE_HIGH 1.0
#define REVIVE_MID  0.6



// Gets a pixel and its symmetric pixel (negative of relative position)
vec3 getPiP(int i, int j) {
    return  vec3(texture(life, interUV + vec2(i*inc_x, j*inc_y)))
          + vec3(texture(life, interUV + vec2(-i*inc_x, -j*inc_y)));
}


// Scale value from 0-1 using a minimum and maximum
float scalar(float val, float mini, float maxi) {
    return (min(maxi, max(val, mini)) - mini) / (maxi-mini);
}


float singleGOL(float life, float near_life) {
    if (life > LIFE_THRESH) {
        life -= AGE_RATE;
        life = near_life * SPREAD + life * (1.0-SPREAD);
        if (near_life < SAFE_LOWER) {
            return life * scalar(life, 0, SAFE_LOWER);
        }
        else if (near_life > SAFE_HIGHER) {
            return life * (1.0 - scalar(life, SAFE_HIGHER, LIFE_MAX));
        }
        else {
            return life + SAFE_INC;
        }
    }
    else {
        if (near_life > REVIVE_LOW && near_life < REVIVE_HIGH) {
            if (near_life < REVIVE_MID) {
                return REV_RATE * scalar(life, REVIVE_LOW, REVIVE_MID);
            }
            else {
                return REV_RATE * (1.0 - scalar(life, REVIVE_MID, REVIVE_HIGH));
            }
        }
        else {
            return 0.0;
        }
    }
}

void main() {
    vec3 my_life = vec3(texture(life, interUV));

    /// GET SUM OF NEIGHBORHOOD ///
    vec3 near_life = vec3(0);
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

    near_life = near_life / pow(2*RAD+1, 2);

        /// CELL LOGIC
    
    lifeOut = vec3(singleGOL(my_life.x, near_life.y), singleGOL(my_life.y, near_life.z), singleGOL(my_life.z, near_life.x));
}
