
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
#include "scenes/scenes.h"

#define COUNT 6

typedef const char *string;

const char *default_vert_file = "src/shaders/default/default.vert";
const char *default_frag_file = "src/shaders/default/default.frag";

shader program;
GLint uniforms[COUNT_UNIFORMS];
GLint mesh_locations[VERTEX_PARAM_COUNT];

static slice shaders, models, scenes;

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

void cube(model *m, float bounds[6], shader *shader)
{
    vertex vertices[36];
    cube_vertices(vertices, bounds);

    mesh current_mesh;
    current_mesh.vertices = new_slice(sizeof(vertex));
    memcpy(current_mesh.bounds, bounds, sizeof(current_mesh.bounds));
    extend_slice(&current_mesh.vertices, sizeof(vertices) / sizeof(vertex), vertices);

    reset_slice(&m->meshes, sizeof(mesh), 0, 1);

    current_mesh.vao = setup_mesh(current_mesh);
    append_slice(&m->meshes, &current_mesh);

    memcpy(m->bounds, bounds, sizeof(m->bounds));
}

int init(GLFWwindow *window)
{

    shaders = new_slice(sizeof(shader));
    models = new_slice(sizeof(model));
    scenes = new_slice(sizeof(scene));

    scene empty_scene;

    model empty_model = {0};
    mesh empty_mesh = {0};

    shader default_shader;
    default_shader.uniforms = uniforms;
    default_shader.input_locations = mesh_locations;
    load_mesh_shader(&default_shader, default_vert_file, default_frag_file);
    append_slice(&shaders, &default_shader);

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

        append_slice(&current_model->meshes, &empty_mesh);
        mesh *current_mesh = &current_model->meshes.data[0];

        load_mesh(filename, current_mesh);

        // send geometry to OpenGL
        current_mesh->vao = setup_mesh(*current_mesh);

        // todo: calculate normals, tangents, and bitangents for meshes
        // if (!objects[x].buffer_slices[NORMALS].len)
        // {
        //     printf("Calculating normals for: %s\n", filename);
        //     calculate_normals_per_face(objects[x].buffer_slices);
        // }

        memcpy(current_model->bounds, current_mesh->bounds, sizeof(current_mesh->bounds));

        scene empty_scene = default_scene(&models, x);
        append_slice(&scenes, &empty_scene);
        scene *current_scene = (scene *)get_slice_item(&scenes, scenes.len - 1);

        current_scene->models = new_slice(sizeof(size_t));
        append_slice_size_t(&current_scene->models, models.len - 1);

        // model cube_model = {0};
        // cube(&cube_model, current_model->bounds, (shader *)shaders.data);
        // append_slice(&models, &cube_model);
        // append_slice_size_t(&current_scene->models, models.len - 1);

        current_scene->init(current_scene, x, COUNT);
    }

    glClearColor(.25, .25, .25, 1.0);
    // glPolygonMode(GL_FRONT, GL_LINE);
    // glPolygonMode(GL_BACK, GL_LINE);
    glEnable(GL_CULL_FACE); // cull face
    // glCullFace(GL_BACK);    // cull back face
    // glFrontFace(GL_CCW);    // GL_CCW for counter clock-wise
    glEnable(GL_DEPTH_TEST);

    return 0;
}

bool handle_events(GLFWwindow *window, shader *program)
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

void update(GLFWwindow *window, double time, double dt)
{
    bool reloaded = handle_events(window, &program);
    scene *scene_data = (scene *)scenes.data;
    for (int x = 0; x < scenes.len; x++)
    {
        scene *scene = &scene_data[x];
        scene->update(scene, x, COUNT, dt, time);
    }
}

int main(void)
{
    GLFWwindow *window = init_opengl(&init);
    update_loop(window, update, shaders, scenes);
    exit(EXIT_SUCCESS);
}
