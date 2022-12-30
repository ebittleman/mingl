#include "shaders.h"

const char *default_vert_file = "src/shaders/phong/phong.vert";
const char *default_frag_file = "src/shaders/phong/phong.frag";

static void draw_phong_material(material material, shader shader)
{
    phong_material *mat = (phong_material *)material.parameters;

    glUniform3fv(shader.uniforms[U_MATERIAL_AMBIENT], 1, mat->ambient);
    glUniform3fv(shader.uniforms[U_MATERIAL_DIFFUSE], 1, mat->diffuse);
    glUniform3fv(shader.uniforms[U_MATERIAL_SPECULAR], 1, mat->specular);
    glUniform1f(shader.uniforms[U_MATERIAL_SHININESS], mat->shininess);
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
    shader.uniforms = malloc(sizeof(GLint) * COUNT_UNIFORMS);
    shader.input_locations = malloc(sizeof(GLint) * VERTEX_PARAM_COUNT);
    load_mesh_shader(&shader, default_vert_file, default_frag_file);
    shader.draw = &simple_draw_shader;

    return shader;
}

material new_debug_phong_material(shader *shader, phong_material *params)
{
    material material = {
        shader,
        params,
        &draw_debug_phong};

    return material;
}

material new_phong_material(shader *shader, phong_material *params)
{
    material material = {
        shader,
        params,
        draw_phong_material};

    return material;
}