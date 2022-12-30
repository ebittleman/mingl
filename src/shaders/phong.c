#include "shaders.h"

const char *default_vert_file = "src/shaders/phong/phong.vert";
const char *default_frag_file = "src/shaders/phong/phong.frag";

enum PhongUniforms
{
    U_MATERIAL_AMBIENT = COUNT_UNIFORMS,
    U_MATERIAL_DIFFUSE,
    U_MATERIAL_SPECULAR,
    U_MATERIAL_SHININESS,
    U_LIGHT_POSITION,
    U_LIGHT_COLOR,
    U_LIGHT_AMBIENT,
    U_LIGHT_DIFFUSE,
    U_LIGHT_SPECULAR,
    PHONG_UNIFORM_COUNT,
};

static const char *PhongUniformNames[] = {
    // phong uniform names
    "material.ambient",
    "material.diffuse",
    "material.specular",
    "material.shininess",
    "light.position",
    "light.color",
    "light.ambient",
    "light.diffuse",
    "light.specular",
};

static void draw_phong_material(material material, shader shader)
{
    phong_params *mat = (phong_params *)material.parameters;
    if (mat == NULL)
    {
        return;
    }

    if (mat->phong_material != NULL)
    {
        glUniform3fv(shader.uniforms[U_MATERIAL_AMBIENT], 1, mat->phong_material->ambient);
        glUniform3fv(shader.uniforms[U_MATERIAL_DIFFUSE], 1, mat->phong_material->diffuse);
        glUniform3fv(shader.uniforms[U_MATERIAL_SPECULAR], 1, mat->phong_material->specular);
        glUniform1f(shader.uniforms[U_MATERIAL_SHININESS], mat->phong_material->shininess);
    }

    if (mat->light != NULL)
    {
        glUniform3fv(shader.uniforms[U_LIGHT_POSITION], 1, mat->light->position);
        glUniform3fv(shader.uniforms[U_LIGHT_COLOR], 1, mat->light->color);
        glUniform3fv(shader.uniforms[U_LIGHT_AMBIENT], 1, mat->light->ambient);
        glUniform3fv(shader.uniforms[U_LIGHT_DIFFUSE], 1, mat->light->diffuse);
        glUniform3fv(shader.uniforms[U_LIGHT_SPECULAR], 1, mat->light->specular);
    }
}

void draw_debug_phong(material self, shader shader)
{
    draw_phong_material(self, shader);

    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);

    glDisable(GL_CULL_FACE); // cull face
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
}

shader phong_shader()
{
    shader shader = {0};
    shader.uniforms = malloc(sizeof(GLint) * PHONG_UNIFORM_COUNT);
    shader.input_locations = malloc(sizeof(GLint) * VERTEX_PARAM_COUNT);
    load_mesh_shader(&shader, default_vert_file, default_frag_file);
    for (size_t i = COUNT_UNIFORMS; i < PHONG_UNIFORM_COUNT; i++)
    {
        const char *name = PhongUniformNames[i - COUNT_UNIFORMS];
        shader.uniforms[i] = glGetUniformLocation(shader.id, name);
    }
    shader.draw = &simple_draw_shader;

    return shader;
}

material new_debug_phong_material(shader *shader, phong_params *params)
{
    material material = {
        shader,
        params,
        &draw_debug_phong};

    return material;
}

material new_phong_material(shader *shader, phong_params *params)
{
    material material = {
        shader,
        params,
        draw_phong_material};

    return material;
}