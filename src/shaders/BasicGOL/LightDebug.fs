#version 430 core
// 410 for mac

layout(location=0) out vec4 out_color;

in vec2 interUV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct PointLight {
    vec4 pos;
    vec4 color;
};

// If distance/brightness reduces influence of the objects color to this fraction or lower, ignore light source
const float LIGHT_MIN_STRENGTH = 1.0/16.0;
const int LIGHT_CNT = 4;
const float SHINNINESS = 100.0;
const float SPECULAR_STRENGTH = 1.0;

uniform PointLight lights[LIGHT_CNT];

#define INCREMENT   0.001
#define RAD         24

float len(vec3 v) {
    v.x = max(0, v.x);
    v.y = max(0, v.y);
    v.z = max(0, v.z);
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

void main() {
    vec4 interAlbedoSpec = texture(gAlbedoSpec, interUV);
    vec4 interNormal = texture(gNormal, interUV);
    vec4 interPosition = texture(gPosition, interUV);


    for (int i = 1; i <= RAD; ++i) {
        for (int j = 1; j <= RAD; ++j) {
            float iinc = i*INCREMENT, jinc = j*INCREMENT;
            interAlbedoSpec += texture(gAlbedoSpec, interUV + vec2(iinc, jinc));
            interAlbedoSpec += texture(gAlbedoSpec, interUV + vec2(iinc, -jinc));
            interAlbedoSpec += texture(gAlbedoSpec, interUV + vec2(-iinc, jinc));
            interAlbedoSpec += texture(gAlbedoSpec, interUV + vec2(-iinc, -jinc));
        }
    }

    interAlbedoSpec /= pow(2*RAD-1,2);


    out_color = vec4(len(interAlbedoSpec.rgb), len(interNormal.rgb), len(interPosition.rgb), 1.0);
}
