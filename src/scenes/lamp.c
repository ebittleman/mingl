#include "scenes.h"

typedef struct _lamp_params
{
    light *light;
} lamp_params;

void init_lamp_scene(scene *self, mesh_factory_t mesh_factory, model_factory_t model_factory)
{
    model *cube_model;
    model_factory(&cube_model);

    mesh cube_mesh = unit_cube();
    mesh *current_mesh;
    mesh_factory(&current_mesh);
    *current_mesh = cube_mesh;
    append_slice(&cube_model->meshes, &current_mesh);
    memcpy(cube_model->bounds, current_mesh->bounds, sizeof(current_mesh->bounds));

    append_slice(&self->models, &cube_model);

    mat4x4_identity(self->position);
}

void update_lamp_scene(scene *self, float dt, float time)
{
    lamp_params *params = (lamp_params *)self->parameters;
    float *position = params->light->position;
    mat4x4_translate(self->current_position,
                     position[0], position[1], position[2]);
}

void draw_lamp_scene(scene self, shader current_shader) {}

void lamp_scene(scene *scene, light *light)
{
    lamp_params *params = malloc(sizeof(lamp_params));
    params->light = light;

    scene->parameters = params;

    scene->init = &init_lamp_scene;
    scene->update = &update_lamp_scene;
    scene->draw = &draw_lamp_scene;
}
