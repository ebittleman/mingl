#include <stdlib.h>

#include "scenes.h"
#include "geometry/geometry.h"

typedef struct _lamp_params
{
    light *light;
    material *material;
    vec3 original_position;
    vec3 velocity;
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

    // initialize positions
    if (params->light == NULL)
    {
        return;
    }

    float *position = params->light->position;
    mat4x4_translate(self->position,
                     position[0], position[1], position[2]);
    vec3_dup(params->original_position, position);
    mat4x4_dup(self->current_position, self->position);
    params->velocity[0] = 2.0f;
    params->velocity[1] = 0.0f;
    params->velocity[2] = 0.0f;

    vec3_scale(params->light->diffuse, params->light->color, 0.5f);
    vec3_scale(params->light->ambient, params->light->diffuse, 0.2f);
}

void update_lamp_scene(scene *self, float dt, float time)
{
    lamp_params *params = (lamp_params *)self->parameters;
    float *position = params->original_position;

    vec4 origin = {0.0f, 0.0f, 0.0f, 1.0f};
    float x = cosf(time * TAU * .1f) * position[1];
    float z = sinf(time * TAU * .1f) * position[1];
    float y = cosf(time * TAU * .1f) * position[1];

    mat4x4_translate(self->current_position,
                     x, y, z);
    // mat4x4_translate(self->current_position,
    //                  position[0], position[1], position[2]);
    // mat4x4_translate_in_place(self->current_position,
    //                           -position[0], 0.0, -position[2]);
    // mat4x4_rotate_Y(self->current_position, self->current_position, time * TAU * .1);
    // mat4x4_translate_in_place(self->current_position,
    //                           position[0], 0.0, position[2]);

    vec4 result;
    mat4x4_mul_vec4(
        result, self->current_position,
        origin);
    vec3_dup(params->light->position, result);

    // debug_mat(self->current_position);
    // debug_vec3(params->light->position);
    // debug_vec4(result);
    // exit(0);
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
