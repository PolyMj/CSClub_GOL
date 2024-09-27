#version 430 core
// 410 for mac

layout(location=0) out vec4 out_color;

in vec2 interUV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 cameraPos;

struct PointLight {
    vec4 pos;
    vec4 color;
};

// If distance/brightness reduces influence of the objects color to this fraction or lower, ignore light source
const float LIGHT_MIN_STRENGTH = 1.0/64.0;
const int LIGHT_CNT = 4;
const float SHINNINESS = 500.0;
const float SPECULAR_STRENGTH = 5.0;
const float AMBIENT_STRENGTH = 0.05;

uniform PointLight lights[LIGHT_CNT];

void main() {
    vec3 interPos = vec3(texture(gPosition, interUV));
    vec4 interAlbedoSpec = texture(gAlbedoSpec, interUV);
    vec3 interNorm = vec3(texture(gNormal, interUV));
    
    vec3 albedo = interAlbedoSpec.rgb;
    vec3 V = normalize(cameraPos-vec3(interPos));
    vec3 N = normalize(interNorm);

    vec3 finalColor = vec3(0,0,0);
    for(int i = 0; i < LIGHT_CNT; i++) {
        float strength = lights[i].color.a * pow(distance(lights[i].pos.xyz, interPos), -2);
        
        // If the light's influence is too small, skip to next light source
        if (strength < LIGHT_MIN_STRENGTH) {
            continue;
        }

        vec3 L = normalize(lights[i].pos.xyz-interPos);
        vec3 H = normalize(V+L);

        float diffCoef = max(0, dot(N, L));

        // Add diffuse component
        finalColor += strength * diffCoef * lights[i].color.rgb * albedo;
        
        // If viewing back of face, skip specular lighting
        if (dot(V, N) < 0) {
            continue;
        }

        float specCoef = pow(max(0, dot(N,H)), SHINNINESS);
        vec3 specColor = mix(vec3(1.0), albedo, specCoef);
        finalColor += SPECULAR_STRENGTH * strength * specCoef * specColor * lights[i].color.rgb;
    }

    finalColor += AMBIENT_STRENGTH * albedo;
    
    finalColor = finalColor / (finalColor + vec3(1.0));
    out_color = vec4(finalColor, interAlbedoSpec.a);
}
