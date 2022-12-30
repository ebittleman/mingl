
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
#include "shader_util.h"
#include "types.h"
#include "shaders/shaders.h"
#include "scenes/scenes.h"

#define COUNT 6

INITIALIZE_ENTITY_STORAGE(mingl)

light positional_light = {
    {0.0f, 5.0f, 5.f},
    {1.0f, 1.0f, 1.0f},
    {0.5f, 0.5f, 0.5f},
    {0.8f, 0.8f, 0.8f},
    {1.0f, 1.0f, 1.0f},
};

static shader directional_light_shader;
GLint uniforms[COUNT_UNIFORMS];
GLint mesh_locations[VERTEX_PARAM_COUNT];

static slice shaders;

string ASSET_DIR = "assets";
static slice strings = {0, 0, sizeof(char), NULL};

int list_files(string dir_name, slice *files);

int init(GLFWwindow *window)
{
    mingl_create_entity_tables();

    shaders = new_slice(sizeof(shader));

    // default shader
    shader shader1 = default_shader();
    append_slice(&shaders, &shader1);

    // directional light shader
    shader shader2 = directional_light();
    append_slice(&shaders, &shader2);

    slice files = new_slice(sizeof(size_t));
    const size_t *idx = slice_size_t_slice(&files, 0, files.len).data;
    int err = list_files(ASSET_DIR, &files);
    if (err < 0)
    {
        return err;
    }

    shader *root_shader = get_slice_item(&shaders, 0);
    for (int x = 0; x < COUNT; x++)
    {
        // load  geometry
        const char *filename = slice_char_slice(&strings, idx[x], strings.len).data;
        model *current_model;
        mingl_model_factory(&current_model);
        load_model(filename, current_model, &mingl_mesh_factory);

        // add the new model to a simple scene
        scene *current_scene;
        mingl_scene_factory(&current_scene);
        default_scene(current_scene, &current_model, root_shader, x, COUNT, x == 1);
    }

    material lamp_material = {0};
    lamp_material.shader = get_slice_item(&shaders, 1);
    lamp_material.draw = draw_lit_material;

    scene *lamp;
    mingl_scene_factory(&lamp);
    lamp_scene(lamp, &positional_light, lamp_material);

    for (size_t i = 0; i < mingl_scenes.len; i++)
    {
        scene *current_scene = get_slice_item(&mingl_scenes, i);
        current_scene->init(current_scene, &mingl_mesh_factory, &mingl_model_factory);
    }

    for (size_t i = 0; i < mingl_models.len; i++)
    {
        model *current_model = get_slice_item(&mingl_models, i);
        // TODO: ensure every mesh is only loaded into OpenGL once. Right now
        //       its possible load a mesh more than once if its referenced by
        //       multiple models
        setup_model(*current_model);
    }

    glClearColor(.25, .25, .25, 1.0);
    glEnable(GL_CULL_FACE); // cull face
    glFrontFace(GL_CCW);    // GL_CCW for counter clock-wise
    glEnable(GL_DEPTH_TEST);

    return 0;
}

bool handle_events(GLFWwindow *window)
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
    bool reloaded = handle_events(window);
    scene *scene_data = (scene *)mingl_scenes.data;
    for (int x = 0; x < mingl_scenes.len; x++)
    {
        scene *scene = &scene_data[x];
        scene->update(scene, dt, time);
    }

    shader *shader_data = (shader *)shaders.data;
    for (size_t i = 0; i < shaders.len; i++)
    {
        glUseProgram(shader_data[i].id);
        glUniform3fv(shader_data[i].uniforms[U_LIGHT_POSITION], 1, positional_light.position);
        glUniform3fv(shader_data[i].uniforms[U_LIGHT_COLOR], 1, positional_light.color);
        glUniform3fv(shader_data[i].uniforms[U_LIGHT_AMBIENT], 1, positional_light.ambient);
        glUniform3fv(shader_data[i].uniforms[U_LIGHT_DIFFUSE], 1, positional_light.diffuse);
        glUniform3fv(shader_data[i].uniforms[U_LIGHT_SPECULAR], 1, positional_light.specular);
    }
}

int main(void)
{
    GLFWwindow *window = init_opengl(&init);
    start(window, update, shaders, mingl_scenes);
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
