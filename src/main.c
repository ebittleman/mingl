
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
#include "linmath.h"
#include "shaders.h"
#include "types.h"
#include "scenes/scenes.h"

#define COUNT 6

INITIALIZE_ENTITY_STORAGE()

const char *default_vert_file = "src/shaders/default/default.vert";
const char *default_frag_file = "src/shaders/default/default.frag";

static shader default_shader;
GLint uniforms[COUNT_UNIFORMS];
GLint mesh_locations[VERTEX_PARAM_COUNT];

static slice shaders;

string ASSET_DIR = "assets";
static slice strings = {0, 0, sizeof(char), NULL};

int list_files(string dir_name, slice *files);

int init(GLFWwindow *window)
{
    create_entity_tables();

    shaders = new_slice(sizeof(shader));
    default_shader = (shader){0};
    default_shader.uniforms = uniforms;
    default_shader.input_locations = mesh_locations;
    load_mesh_shader(&default_shader, default_vert_file, default_frag_file);
    append_slice(&shaders, &default_shader);

    slice files = new_slice(sizeof(size_t));
    const size_t *idx = slice_size_t_slice(&files, 0, files.len).data;
    int err = list_files(ASSET_DIR, &files);
    if (err < 0)
    {
        return err;
    }

    for (int x = 0; x < COUNT; x++)
    {
        // load  geometry
        const char *filename = slice_char_slice(&strings, idx[x], strings.len).data;
        model *current_model;
        size_t current_model_id = model_factory(&current_model);

        load_model(filename, current_model, mesh_factory);
        // todo: calculate normals, tangents, and bitangents for meshes

        // add the new model to a simple scene
        size_t current_scene_id = scene_factory(NULL);
        scene *current_scene = get_slice_item(&scenes, current_scene_id);
        default_scene(current_scene, current_model_id, x, COUNT, false);
    }

    for (size_t i = 0; i < scenes.len; i++)
    {
        scene *current_scene = get_slice_item(&scenes, i);
        current_scene->init(current_scene);
    }

    for (size_t i = 0; i < models.len; i++)
    {
        model *current_model = get_slice_item(&models, i);
        setup_model(*current_model);
    }

    glClearColor(.25, .25, .25, 1.0);
    // glPolygonMode(GL_FRONT, GL_LINE);
    // glPolygonMode(GL_BACK, GL_LINE);
    // glEnable(GL_CULL_FACE); // cull face
    // glCullFace(GL_BACK);    // cull back face
    // glFrontFace(GL_CCW);    // GL_CCW for counter clock-wise
    // glEnable(GL_DEPTH_TEST);

    return 0;
}

bool handle_events(GLFWwindow *window, shader *default_shader)
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
        //     &default_shader->id,
        //     default_shader->vert_file,
        //     default_shader->frag_file);
        // set_uniforms_and_inputs(
        //     default_shader->id, default_shader->uniforms,
        //     default_shader->input_locations);
        // printf("reloaded shaders\n");
        return true;
    }

    return false;
}

void update(GLFWwindow *window, double time, double dt)
{
    bool reloaded = handle_events(window, &default_shader);
    scene *scene_data = (scene *)scenes.data;
    for (int x = 0; x < scenes.len; x++)
    {
        scene *scene = &scene_data[x];
        scene->update(scene, dt, time);
    }
}

int main(void)
{
    GLFWwindow *window = init_opengl(&init);
    update_loop(window, update, shaders, scenes);
    exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

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
