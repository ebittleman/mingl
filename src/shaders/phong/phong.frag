#version 460

struct Material {
    vec3 ambient;    // usually the surface's color
    vec3 diffuse;    // usually the surface's color
    vec3 specular;   // color of the specular highlight
    float shininess; // scattering/radius of the specular highlight
};

struct Light {
    vec3 position;
    vec3 color;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 view_position;
uniform Material material;
uniform Light light;

in vec4 colorInterp;
in vec3 fragPosInterp;
in vec3 normalInterp;

out vec4 outColor;

#define TAU 6.28318530718
#define RED vec4(1,0,0,1)

void main() {

    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse 
    vec3 norm = normalize(normalInterp);
    vec3 lightDir = normalize(light.position - fragPosInterp);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // specular
    vec3 viewDir = normalize(view_position - fragPosInterp);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);
    vec3 result = (ambient + diffuse + specular);

    outColor = vec4(result * colorInterp.rgb, 1.0f);
}
