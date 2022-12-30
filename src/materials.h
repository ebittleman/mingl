#ifndef MATERIALS_C
#define MATERIALS_C
#include "shaders/shaders.h"

static void draw_lit_material(material material, shader shader)
{
    glUniform3fv(shader.uniforms[U_MATERIAL_AMBIENT], 1, material.ambient);
    glUniform3fv(shader.uniforms[U_MATERIAL_DIFFUSE], 1, material.diffuse);
    glUniform3fv(shader.uniforms[U_MATERIAL_SPECULAR], 1, material.specular);
    glUniform1f(shader.uniforms[U_MATERIAL_SHININESS], material.shininess);
}

void debug_draw(material self, shader shader)
{
    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);

    glDisable(GL_CULL_FACE); // cull face
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    draw_lit_material(self, shader);
}

material materials[] = {
    // all static objects
    {
        {1.0f, 0.5f, 0.31f},
        {1.0f, 0.5f, 0.31f},
        {0.5f, 0.5f, 0.5f}, // specular lighting doesn't have full effect on this object's material
        32.0f,
        1.0f,
        {1.0f, 1.0f, 1.0f},
        1.0f,
        HighlightOn,
        0,
        &draw_lit_material},

    // debugged static objects
    {
        {1.0f, 0.5f, 0.31f},
        {1.0f, 0.5f, 0.31f},
        {0.5f, 0.5f, 0.5f},
        32.0f,
        1.0f,
        {1.0f, 1.0f, 1.0f},
        1.0f,
        HighlightOn,
        0,
        &debug_draw},
    // empty material
    {0},
};

enum MaterialIDS
{
    ALL_STATIC_OBJECTS = 0,
    DEBUGGED_STATIC_OBJECTS,
    EMPTY_MATERIAL,
};

#endif
