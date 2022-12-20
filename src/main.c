
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
#include "linmath.h"
#include "shaders.h"
#include "types.h"

#define TAU 6.28318530718

typedef const char *string;

const char *default_vert_file = "src/shaders/default/default.vert";
const char *default_frag_file = "src/shaders/default/default.frag";

#define COUNT 6

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

// requires `glDrawArrays`
void calculate_normals_per_face(slice buffer_slices[COUNT_BUFFERS])
{

    reset_slice(&buffer_slices[NORMALS],
                buffer_slices[VERTS].size,
                buffer_slices[VERTS].len,
                buffer_slices[VERTS].cap);

    vec3 *vertices = (vec3 *)buffer_slices[VERTS].data;
    vec3 *normals = (vec3 *)buffer_slices[NORMALS].data;

    vec3 normal, a, b;
    for (int x = 0; x < buffer_slices[VERTS].len; x += 3)
    {
        float *p1 = vertices[x];
        float *p2 = vertices[x + 1];
        float *p3 = vertices[x + 2];

        vec3_sub(a, p2, p1);
        vec3_sub(b, p3, p1);
        vec3_mul_cross(normal, a, b);

        float *n1 = normals[x];
        float *n2 = normals[x + 1];
        float *n3 = normals[x + 2];

        vec3_norm(normal, normal);

        vec3_dup(n1, normal);
        vec3_dup(n2, normal);
        vec3_dup(n3, normal);
    }

    return;
}

void init_obj_model(mat4x4 m, float bounds[6], int x, int count)
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

void calculate_model_position(mat4x4 m, mat4x4 obj, double time)
{
    mat4x4_dup(m, obj);
    // mat4x4_identity(m);
    // mat4x4_scale(m, m, 1.0f / 12.0f);
    // mat4x4_translate_in_place(m, .1f, .2f, .5f);

    // mat4x4_rotate_X(m, m, time * TAU * .1);
    mat4x4_rotate_Y(m, m, time * TAU * .1);
    // mat4x4_rotate_Z(m, m, time * TAU * .1);

    // mat4x4_mul(m, m, obj);
    // debug_mat(m);
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
        reload_key_pressed = false;
        reload_shader_program_from_files(
            &program->id,
            program->vert_file,
            program->frag_file);
        set_uniforms_and_inputs(
            program->id, program->uniforms,
            program->input_locations);
        printf("reloaded shaders\n");
        return true;
    }

    return false;
}

Program program;
object objects[COUNT];

int init(GLFWwindow *window)
{
    load_program(&program, default_vert_file, default_frag_file);

    slice files = new_slice(sizeof(size_t));
    size_t *idx = (size_t *)files.data;
    int err = list_files(ASSET_DIR, &files);
    if (err < 0)
    {
        return err;
    }

    for (int x = 0; x < COUNT; x++)
    {
        char *filename = strings.data + idx[x];
        float bounds[6] = {0};
        objects[x].program = &program;
        memset(objects[x].buffer_slices, 0, sizeof(objects[x].buffer_slices));
        load_obj_file(filename, objects[x].buffer_slices, bounds);

        if (!objects[x].buffer_slices[NORMALS].len)
        {
            printf("Calculating normals for: %s\n", filename);
            calculate_normals_per_face(objects[x].buffer_slices);
        }

        init_obj_model(objects[x].init_model, bounds, x, COUNT);

        // send geometry to OpenGL
        buffer_object_to_vao(&objects[x]);
    }

    glClearColor(.25, .25, .25, 1.0);
    // glPolygonMode(GL_FRONT, GL_LINE);
    // glPolygonMode(GL_BACK, GL_LINE);
    // glPolygonMode(GL_BACK, GL_LINE);
    // glEnable(GL_CULL_FACE); // cull face
    // glCullFace(GL_BACK);    // cull back face
    // glFrontFace(GL_CW); // GL_CCW for counter clock-wise
    glEnable(GL_DEPTH_TEST);

    return 0;
}

void update(GLFWwindow *window, double time, double dt)
{
    bool reloaded = handle_events(window, &program);
    for (int x = 0; x < COUNT; x++)
    {
        object *obj = &objects[x];
        calculate_model_position(obj->model, obj->init_model, time);
        if (reloaded)
        {
            glUseProgram(obj->program->id);
            glBindVertexArray(obj->vertex_array);
            configure_vertex_attributes(
                obj->vertex_buffers, obj->program->input_locations,
                obj->buffer_disabled);
        }
    }
}

int main(void)
{
    GLFWwindow *window = init_opengl(&init);
    update_loop(window, update, COUNT, objects);
    exit(EXIT_SUCCESS);
}
