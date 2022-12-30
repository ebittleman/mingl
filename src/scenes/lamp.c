#include "scenes.h"

typedef struct _lamp_params
{
    light *light;
    material *material;
} lamp_params;

void init_lamp_scene(scene *self, mesh_factory_t mesh_factory, model_factory_t model_factory)
{
    lamp_params *params = (lamp_params *)self->parameters;

    mesh *current_mesh;
    mesh_factory(&current_mesh);

    mesh cube_mesh = unit_cube();
    *current_mesh = cube_mesh;

    model *cube_model;
    model_factory(&cube_model);
    cube_model->material = params->material;
    memcpy(cube_model->bounds, current_mesh->bounds, sizeof(current_mesh->bounds));

    append_slice(&cube_model->meshes, &current_mesh);

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

void lamp_scene(scene *scene, light *light, material *material)
{
    lamp_params *params = malloc(sizeof(lamp_params));
    params->light = light;
    params->material = material;

    scene->parameters = params;

    scene->init = &init_lamp_scene;
    scene->update = &update_lamp_scene;
}
