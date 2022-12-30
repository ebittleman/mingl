
#include "scenes.h"

typedef struct _default_params
{
    int x;
    int count;
    bool cube_enabled;
    shader *shader;
} default_params;

// material debug_cube_material;

material default_model_material = {
    {1.0f, 0.5f, 0.31f},
    {1.0f, 0.5f, 0.31f},
    {0.5f, 0.5f, 0.5f}, // specular lighting doesn't have full effect on this object's material
    32.0f,
    1.0f,
    {1.0f, 1.0f, 1.0f},
    1.0f,
    HighlightOn,
    0,
    0};

material default_model_debug_material = {
    {1.0f, 0.5f, 0.31f},
    {1.0f, 0.5f, 0.31f},
    {0.5f, 0.5f, 0.5f}, // specular lighting doesn't have full effect on this object's material
    32.0f,
    1.0f,
    {1.0f, 1.0f, 1.0f},
    1.0f,
    HighlightOn,
    0,
    0};

void debug_draw(material self, shader shader)
{
    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);

    glDisable(GL_CULL_FACE); // cull face
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    draw_lit_material(self, shader);
}

void init_default_geometry(scene *self)
{
    default_params *params = (default_params *)self->parameters;
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

void init_default_scene(scene *self, mesh_factory_t mesh_factory, model_factory_t model_factory)
{
    default_params *params = (default_params *)self->parameters;
    model *first_model = *(model **)get_slice_item(&self->models, 0);

    default_model_material.draw = &draw_lit_material;
    default_model_debug_material.draw = debug_draw;

    first_model->material = &default_model_material;
    float *bounds = first_model->bounds;

    // debug_cube_material.shader = first_model->material->shader;
    // debug_cube_material.shader->draw = &debug_draw;

    // scene->draw = &draw_default_scene;

    if (params->cube_enabled)
    {
        model *cube_model;
        model_factory(&cube_model);
        cube_model->material = &default_model_debug_material;

        mesh cube_mesh = cube(bounds);
        mesh *current_mesh;
        mesh_factory(&current_mesh);
        *current_mesh = cube_mesh;
        append_slice(&cube_model->meshes, &current_mesh);

        append_slice(&self->models, &cube_model);
    }

    init_default_geometry(self);
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

    // float scale = (sinf(time * TAU * .25) + 1.1) * ((params->x + params->count) / params->count);

    // mat4x4_scale_aniso(S, S, scale, scale, scale);

    mat4x4_rotate_X(R, R, time * TAU * .1);
    mat4x4_rotate_Y(R, R, time * TAU * .1);
    mat4x4_rotate_Z(R, R, time * TAU * .1);

    mat4x4_mul(self->current_position, self->position, S);
    mat4x4_mul(self->current_position, self->current_position, R);
    // debug_mat(destination_position);
}

void draw_default_scene(model self, shader current_shader)
{
    // set any shader specific uniforms for this scene here
}

void default_scene(scene *scene, model **model, shader *scene_shader, int x, int count, bool display_bounds)
{

    default_params *params = malloc(sizeof(default_params));
    params->x = x;
    params->count = count;
    params->cube_enabled = display_bounds;
    params->shader = scene_shader;

    default_model_material.shader = scene_shader;

    scene->init = &init_default_scene;
    scene->update = &update_default_scene;
    scene->parameters = params;

    append_slice(&scene->models, model);
}
