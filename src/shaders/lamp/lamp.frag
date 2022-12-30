#version 460

struct Light {
    vec3 position;
    vec3 color;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

out vec4 outColor;

void main() {
    outColor = vec4(light.color, 1.0f);
}
