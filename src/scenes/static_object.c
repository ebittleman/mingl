
#include "scenes.h"

typedef struct _static_object_params
{
    int x;
    int count;
    bool cube_enabled;
    material *material;
    material *debug_material;
} static_object_params;

void init_static_geometry(scene *self)
{
    static_object_params *params = (static_object_params *)self->parameters;
    int x = params->x, count = params->count;

    model *first_model = *(model **)get_slice_item(&self->models, 0);
    float *bounds = first_model->bounds;

    mat4x4 S, T;
    float ranges[3] = {
        bounds[3] - bounds[0],
        bounds[4] - bounds[1],
        bounds[5] - bounds[2],
    };
    float max = ranges[0];
    for (int x = 1; x < 3; x++)
    {
        if (ranges[x] > max)
        {
            max = ranges[x];
        }
    }

    float scale = 1.0f / max;
    mat4x4_translate( // starts with identity matrix then sets translation
        self->position,
        ((ranges[0] / 2.0f - bounds[3]) * scale) + ((float)x * 2) - (float)count,
        (ranges[1] / 2.0f - bounds[4]) * scale,
        (ranges[2] / 2.0f - bounds[5]) * scale);

    mat4x4_identity(S);
    mat4x4_scale_aniso(S, S, scale, scale, scale);
    mat4x4_mul(self->position, self->position, S);

    // debug_mat(m);
    // debug_bounds(bounds);
    // debug_vec3(ranges);
}

void init_static_object(scene *self, mesh_factory_t mesh_factory, model_factory_t model_factory)
{
    static_object_params *params = (static_object_params *)self->parameters;
    model *first_model = *(model **)get_slice_item(&self->models, 0);

    first_model->material = params->material;
    float *bounds = first_model->bounds;

    if (params->cube_enabled)
    {
        model *cube_model;
        model_factory(&cube_model);
        cube_model->material = params->debug_material;

        mesh cube_mesh = cube(bounds);
        mesh *current_mesh;
        mesh_factory(&current_mesh);
        *current_mesh = cube_mesh;
        append_slice(&cube_model->meshes, &current_mesh);

        append_slice(&self->models, &cube_model);
    }

    init_static_geometry(self);
}

void update_static_object(scene *self, float dt, float time)
{
    static_object_params *params = (static_object_params *)self->parameters;

    mat4x4 S, R;
    mat4x4_identity(S);
    mat4x4_identity(R);

    // mat4x4_dup(self->current_position, self->position);
    // mat4x4_identity(destination_position);
    // mat4x4_translate_in_place(destination_position, .1f, .2f, .5f);

    // float scale = (sinf(time * TAU * .25) + 1.1) * ((params->x + params->count) / params->count);

    // mat4x4_scale_aniso(S, S, scale, scale, scale);

    mat4x4_rotate_X(R, R, time * TAU * .1);
    mat4x4_rotate_Y(R, R, time * TAU * .1);
    mat4x4_rotate_Z(R, R, time * TAU * .1);

    mat4x4_mul(self->current_position, self->position, S);
    mat4x4_mul(self->current_position, self->current_position, R);
    // debug_mat(destination_position);
}

void static_object(scene *scene, model **model, material *base_material,
                   material *debug_material, int x, int count,
                   bool display_bounds)
{

    static_object_params *params = malloc(sizeof(static_object_params));
    params->x = x;
    params->count = count;
    params->cube_enabled = display_bounds;
    params->material = base_material;
    params->debug_material = debug_material;

    scene->init = &init_static_object;
    scene->update = &update_static_object;
    scene->parameters = params;

    append_slice(&scene->models, model);
}
