#version 330

in vec4 colorInterp;
in vec2 uvInterp;
in float timInterp;
out vec4 outColor;

#define TAU 6.28318530718

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
    // outColor = vec4(1.0, 1.0, 1.0, 1.0);

    // float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    float xOffest = cos(uvInterp.y * TAU * 8) * .02;
    vec3 color = .5 * cos((uvInterp.xxx + xOffest + timInterp * .1f) * 5 * TAU) + .5;
    vec4 output1 = vec4(color, 1.0) * abs(colorInterp);
    outColor = output1;
    return;

    // float xOffest = cos(uvInterp.y * TAU * 8) * .01;
    // vec4 mixed = mix(vec4(uvInterp, 0, 0), colorInterp, 1.0);
    // outColor = .5 * cos((mixed + xOffest + timInterp * .1f) * 5 * TAU) + .5;

    // outColor = vec4(vec3(depth), 1.0);

    // outColor = vec4(uvInterp.xxx * vec3(1, 0, 0), 1.0);
    // outColor = mix(uvInterp, colorInterp, 1.0);
}
