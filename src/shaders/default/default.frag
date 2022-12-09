#version 330

in vec4 colorInterp;
in vec2 uvInterp;
in float timInterp;
out vec4 outColor;

#define TAU 6.28318530718
#define RED vec4(1,0,0,1)

float near = 0.1;
float far = 100.0;

void main() {

    // vec2 col = mix(colorInterp.xy, uvInterp, 1.0);
    // vec4 col2 = vec4(col, colorInterp.z, 1);
    // outColor = vec4(mix(uvInterp.xxx, colorInterp.xyz, 1.0), 1.0f);
    // outColor = colorInterp;
    outColor = vec4(uvInterp.xxx, 1);
}
