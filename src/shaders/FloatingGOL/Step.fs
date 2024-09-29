#version 430 core
// 410 for mac

layout(location=0) out float lifeOut;

in vec2 interUV;

uniform sampler2D life;
uniform float inc_x;
uniform float inc_y;

#define RAD         24


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



// Scale value from 0-1 using a minimum and maximum
float scalar(float val, float mini, float maxi) {
    return (min(maxi, max(val, mini)) - mini) / (maxi-mini);
}

void main() {
    float my_life = float(texture(life, interUV));
    bool is_alive = my_life > LIFE_THRESH;

    float near_life = 0;
    for (int i = 0; i <= RAD; ++i) {
        for (int j = (i == 0 ? 1 : 0); j <= RAD; ++j) {
            float iinc = i*inc_x, jinc = j*inc_y;
            near_life += float(texture(life, interUV + vec2(iinc, jinc)));
            near_life += float(texture(life, interUV + vec2(-iinc, -jinc)));
        }
    }

    near_life = near_life / (pow(RAD+1, 2) - 1);

    if (is_alive) {
        my_life -= AGE_RATE;
        my_life = near_life * SPREAD + my_life * (1.0-SPREAD);
        if (near_life < SAFE_LOWER) {
            lifeOut = my_life * scalar(my_life, 0, SAFE_LOWER);
        }
        else if (near_life > SAFE_HIGHER) {
            lifeOut = my_life * (1.0 - scalar(my_life, SAFE_HIGHER, LIFE_MAX));
        }
        else {
            lifeOut = my_life + SAFE_INC;
        }
    }
    else {
        if (near_life > REVIVE_LOW && near_life < REVIVE_HIGH) {
            if (near_life < REVIVE_MID) {
                lifeOut = REV_RATE * scalar(my_life, REVIVE_LOW, REVIVE_MID);
            }
            else {
                lifeOut = REV_RATE * (1.0 - scalar(my_life, REVIVE_MID, REVIVE_HIGH));
            }
        }
        else {
            lifeOut = 0.0;
        }
    }
}
