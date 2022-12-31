#version 460

uniform sampler2D texture1;

in vec2 uvInterp;

out vec4 outColor;

void main() {
    outColor = texture(texture1, uvInterp);
}
