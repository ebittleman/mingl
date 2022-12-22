
#include <dirent.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "obj3d.h"
#include "opengl.h"
#include "entities.h"
#include "cube.h"
#include "linmath.h"
#include "shaders.h"
#include "types.h"

#define TAU 6.28318530718
#define COUNT 6

typedef const char *string;

const char *default_vert_file = "src/shaders/default/default.vert";
const char *default_frag_file = "src/shaders/default/default.frag";

Program program;
GLint uniforms[COUNT_UNIFORMS];
GLint mesh_locations[VERTEX_PARAM_COUNT];

static slice shaders, models;

string ASSET_DIR = "assets";
static slice strings = {0, 0, sizeof(char), NULL};

int list_files(string dir_name, slice *files)
{
    struct dirent *de;
    DIR *dr = opendir(dir_name);
    char dir_sep = '/';
    if (dr == NULL) // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory");
        return -1;
    }

    while ((de = readdir(dr)) != NULL)
    {
        size_t len = strlen(de->d_name);
        if (strstr(de->d_name, ".obj") == NULL)
        {
            continue;
        }
        size_t dst = strings.len * strings.size;
        append_slice(files, &dst);
        extend_slice(&strings, strlen(dir_name), (void *)dir_name);
        append_slice(&strings, &dir_sep);
        extend_slice(&strings, strlen(de->d_name) + 1, de->d_name);
    }

    closedir(dr);
    return 0;
}

void cube(model *m, float bounds[6], Program *shader)
{
    vertex vertices[36];
    // float bounds[6] = {
    //     -0.5f, 0.5f,
    //     -0.5f, 0.5f,
    //     -0.5f, 0.5f};
    cube_vertices(vertices, bounds);

    mesh current_mesh;
    current_mesh.vertices = new_slice(sizeof(vertex));
    current_mesh.shader = shader;
    memcpy(current_mesh.bounds, bounds, sizeof(current_mesh.bounds));
    extend_slice(&current_mesh.vertices, sizeof(vertices) / sizeof(vertex), vertices);

    reset_slice(&m->vaos, sizeof(GLuint), 0, 1);
    reset_slice(&m->meshes, sizeof(mesh), 0, 1);

    GLuint vao = setup_mesh(current_mesh);
    append_slice(&m->vaos, &vao);
    append_slice(&m->meshes, &current_mesh);

    memcpy(m->bounds, bounds, sizeof(m->bounds));
}

// requires `glDrawArrays`
// void calculate_normals_per_face(slice buffer_slices[COUNT_BUFFERS])
// {

//     reset_slice(&buffer_slices[NORMALS],
//                 buffer_slices[VERTS].size,
//                 buffer_slices[VERTS].len,
//                 buffer_slices[VERTS].cap);

//     vec3 *vertices = (vec3 *)buffer_slices[VERTS].data;
//     vec3 *normals = (vec3 *)buffer_slices[NORMALS].data;

//     vec3 normal, a, b;
//     for (int x = 0; x < buffer_slices[VERTS].len; x += 3)
//     {
//         float *p1 = vertices[x];
//         float *p2 = vertices[x + 1];
//         float *p3 = vertices[x + 2];

//         vec3_sub(a, p2, p1);
//         vec3_sub(b, p3, p1);
//         vec3_mul_cross(normal, a, b);

//         float *n1 = normals[x];
//         float *n2 = normals[x + 1];
//         float *n3 = normals[x + 2];

//         vec3_norm(normal, normal);

//         vec3_dup(n1, normal);
//         vec3_dup(n2, normal);
//         vec3_dup(n3, normal);
//     }

//     return;
// }

void initial_position(mat4x4 m, float bounds[6], int x, int count)
{
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
        m,
        (-(bounds[0] * scale) - (ranges[0] / 2.0) * scale) + ((float)x * 2) - (float)count,
        -(bounds[2] * scale) - (ranges[1] / 2.0) * scale,
        -(bounds[4] * scale) - (ranges[2] / 2.0) * scale);
    mat4x4_identity(S);
    mat4x4_scale_aniso(S, S, scale, scale, scale);
    mat4x4_mul(m, m, S);

    // debug_mat(m);
}

int init(GLFWwindow *window)
{

    shaders = new_slice(sizeof(Program));
    models = new_slice(sizeof(model));

    model empty_model = {0};
    mesh empty_mesh = {0};

    Program shader;
    shader.uniforms = uniforms;
    shader.input_locations = mesh_locations;
    load_mesh_shader(&shader, default_vert_file, default_frag_file);
    append_slice(&shaders, &shader);

    slice files = new_slice(sizeof(size_t));
    size_t *idx = (size_t *)files.data;
    int err = list_files(ASSET_DIR, &files);
    if (err < 0)
    {
        return err;
    }

    for (int x = 0; x < COUNT; x++)
    {
        const char *filename = strings.data + idx[x];

        append_slice(&models, &empty_model);

        model *current_model = (model *)get_slice_item(&models, models.len - 1);
        current_model->meshes = new_slice(sizeof(empty_mesh));
        current_model->vaos = new_slice(sizeof(GLuint));

        slice meshes = current_model->meshes;

        append_slice(&current_model->meshes, &empty_mesh);
        mesh *current_mesh = &current_model->meshes.data[0];

        current_mesh->shader = (Program *)shaders.data;
        load_mesh(filename, current_mesh);

        // send geometry to OpenGL
        GLuint vao = setup_mesh(*current_mesh);
        append_slice(&current_model->vaos, &vao);

        // todo: calculate normals, tangents, and bitangents for meshes
        // if (!objects[x].buffer_slices[NORMALS].len)
        // {
        //     printf("Calculating normals for: %s\n", filename);
        //     calculate_normals_per_face(objects[x].buffer_slices);
        // }

        memcpy(current_model->bounds, current_mesh->bounds, sizeof(current_mesh->bounds));
        initial_position(current_model->position, current_model->bounds, x, COUNT);

        model cube_model = {0};
        cube(&cube_model, current_model->bounds, (Program *)shaders.data);
        initial_position(cube_model.position, cube_model.bounds, x, COUNT);
        append_slice(&models, &cube_model);
    }

    glClearColor(.25, .25, .25, 1.0);
    glPolygonMode(GL_FRONT, GL_LINE);
    // glPolygonMode(GL_BACK, GL_LINE);
    glEnable(GL_CULL_FACE); // cull face
    // glCullFace(GL_BACK);    // cull back face
    // glFrontFace(GL_CCW);    // GL_CCW for counter clock-wise
    glEnable(GL_DEPTH_TEST);

    return 0;
}

bool handle_events(GLFWwindow *window, Program *program)
{
    static bool reload_key_pressed = false;
    bool down = glfwGetKey(window, GLFW_KEY_R);
    if (down && !reload_key_pressed)
    {
        reload_key_pressed = true;
    }
    else if (!down && reload_key_pressed)
    {
        // TODO: get reloading working again
        // reload_key_pressed = false;
        // reload_shader_program_from_files(
        //     &program->id,
        //     program->vert_file,
        //     program->frag_file);
        // set_uniforms_and_inputs(
        //     program->id, program->uniforms,
        //     program->input_locations);
        // printf("reloaded shaders\n");
        return true;
    }

    return false;
}

void calculate_model_position(mat4x4 destination_position, mat4x4 start_position, double time)
{
    mat4x4_dup(destination_position, start_position);
    // mat4x4_identity(destination_position);
    // mat4x4_scale(destination_position, destination_position, 1.0f / 12.0f);
    // mat4x4_translate_in_place(destination_position, .1f, .2f, .5f);

    // mat4x4_rotate_X(destination_position, destination_position, time * TAU * .1);
    mat4x4_rotate_Y(destination_position, destination_position, time * TAU * .1);
    // mat4x4_rotate_Z(destination_position, destination_position, time * TAU * .1);

    // mat4x4_mul(destination_position, destination_position, obj);
    // debug_mat(destination_position);
}

void update(GLFWwindow *window, double time, double dt)
{
    bool reloaded = handle_events(window, &program);
    model *model_data = (model *)models.data;
    for (int x = 0; x < models.len; x++)
    {
        model *model = &model_data[x];
        calculate_model_position(model->current_position, model->position, time);
    }
}

int main(void)
{
    GLFWwindow *window = init_opengl(&init);
    update_loop(window, update, shaders, models);
    exit(EXIT_SUCCESS);
}
