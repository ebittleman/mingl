#version 330

uniform vec3 cam_pos;

in vec4 colorInterp;
in vec2 uvInterp;
in vec3 fragPosInterp;
in vec3 normalInterp;
in float timeInterp;

out vec4 outColor;

#define TAU 6.28318530718
#define RED vec4(1,0,0,1)

float near = 0.1;
float far = 100.0;
vec3 lightColor = vec3(1.0f);
vec3 lightPos = vec3(100.0f, 100.0f, 100.0f);

float ambientStrength = 0.1;
float specularStrength = 0.5;

void main() {

    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(normalInterp);
    vec3 lightDir = normalize(lightPos - fragPosInterp);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(cam_pos - fragPosInterp);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
    vec3 specular = specularStrength * spec * lightColor;
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
