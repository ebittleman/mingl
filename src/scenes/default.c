
#include "scenes.h"

typedef struct _default_params
{
    int x;
    int count;
    bool cube_enabled;
} default_params;

void init_default_scene(scene *self)
{
    default_params *params = (default_params *)self->parameters;
    int x = params->x, count = params->count;

    size_t *first_model_idx = (size_t *)get_slice_item(&self->models_idx, 0);
    model *first_model = (model *)get_slice_item(self->models_table, *first_model_idx);
    float *bounds = first_model->bounds;

    mat4x4 S, T;
    float ranges[3] = {
        bounds[1] - bounds[0],
        bounds[3] - bounds[2],
        bounds[5] - bounds[4],
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
        ((ranges[0] / 2.0f - bounds[1]) * scale) + ((float)x * 2) - (float)count,
        (ranges[1] / 2.0f - bounds[3]) * scale,
        (ranges[2] / 2.0f - bounds[5]) * scale);

    mat4x4_identity(S);
    mat4x4_scale_aniso(S, S, scale, scale, scale);
    mat4x4_mul(self->position, self->position, S);

    // debug_mat(m);
    // debug_bounds(bounds);
    // debug_vec3(ranges);
}

void update_default_scene(scene *self, float dt, float time)
{
    default_params *params = (default_params *)self->parameters;

    mat4x4 S, R;
    mat4x4_identity(S);
    mat4x4_identity(R);

    // mat4x4_dup(self->current_position, self->position);
    // mat4x4_identity(destination_position);
    // mat4x4_translate_in_place(destination_position, .1f, .2f, .5f);

    float scale = (sinf(time * TAU * .25) + 1.1) * ((params->x + params->count) / params->count);

    mat4x4_scale_aniso(S, S, scale, scale, scale);

    // mat4x4_rotate_X(destination_position, destination_position, time * TAU * .1);
    mat4x4_rotate_Y(R, R, time * TAU * .1);
    // mat4x4_rotate_Z(destination_position, destination_position, time * TAU * .1);

    mat4x4_mul(self->current_position, self->position, S);
    mat4x4_mul(self->current_position, self->current_position, R);
    // debug_mat(destination_position);
}

void draw_default_scene(scene self, shader current_shader)
{
    // set any shader specific uniforms for this scene here
}

scene default_scene(slice *models_table, int x, int count, bool display_bounds)
{
    scene result = {0};

    default_params *params = malloc(sizeof(default_params));
    params->x = x;
    params->count = count;
    params->cube_enabled = display_bounds;

    result.models_table = models_table;
    result.init = &init_default_scene;
    result.update = &update_default_scene;
    result.draw = &draw_default_scene;
    result.parameters = params;

    return result;
}
