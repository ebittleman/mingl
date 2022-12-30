#version 460

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
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
uniform float time;

in vec4 colorInterp;
in vec2 uvInterp;
in vec3 fragPosInterp;
in vec3 normalInterp;

out vec4 outColor;

#define TAU 6.28318530718
#define RED vec4(1,0,0,1)

// vec3 lightColor = vec3(1.0f);
// vec3 lightPos = vec3(100.0f, 100.0f, 100.0f);

void main() {

    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse 
    vec3 norm = normalize(normalInterp);
    vec3 lightDir = normalize(light.position - fragPosInterp);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    vec3 viewDir = normalize(view_position - fragPosInterp);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);
    vec3 result = (ambient + diffuse + specular);
    outColor = vec4(result * colorInterp.rgb, 1.0f);

    // vec2 col = mix(colorInterp.xy, uvInterp, 1.0);
    // vec4 col2 = vec4(col, colorInterp.z, 1);
    // outColor = vec4(mix(uvInterp.xxx, colorInterp.xyz, 1.0), 1.0f);

    // vec3 xOffest = cos(colorInterp.yyy) * 2;
    // vec3 outc = cos(colorInterp.xxx * TAU * 2 + timeInterp * 10) * .5 + .5;
    // outColor = vec4(outc.x, outc.x + xOffest.x, outc.z, 1);

    // outColor = RED;
}
