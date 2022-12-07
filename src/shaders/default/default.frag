#version 330

in vec4 colorInterp;
in vec4 uvInterp;
out vec4 outColor;

void main() {
    // outColor = vec4(1.0, 1.0, 1.0, 1.0);
    outColor = uvInterp;
}
