
#ifdef DEMOS_BASIC
#include <stdlib.h>
#include <dirent.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "obj3d.h"
#include "scenes/scenes.h"
#include "data_structures.h"
#include "materials.h"
#include "opengl.h"

INITIALIZE_ENTITY_STORAGE(mingl_basic)

string ASSET_DIR = "assets/models";
static slice strings = {0, 0, sizeof(char), NULL};

light positional_light = {
    {0.0f, 5.0f, 5.f},
    {1.0f, 1.0f, 1.0f},

    {1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
};

static slice shaders, materials;

int list_files(string dir_name, slice *files);

void container_update(scene *self, float dt, float time)
{
    mat4x4_translate(self->current_position, 0.0f, -2.0f, 0.0f);
}

int init(GLFWwindow *window)
{
    // create entity tables
    mingl_basic_create_entity_tables();

    // initialize shaders
    shaders = new_slice(sizeof(shader));

    // default shader
    shader shader1 = phong_shader();
    append_slice(&shaders, &shader1);
    shader *root_shader = get_slice_item(&shaders, shaders.len - 1);

    // directional light shader
    shader shader2 = lamp_shader();
    append_slice(&shaders, &shader2);
    shader *lamp_shader = get_slice_item(&shaders, shaders.len - 1);

    // textured shader
    shader shader3 = textured_shader();
    append_slice(&shaders, &shader3);
    shader *textured_shader = get_slice_item(&shaders, shaders.len - 1);

    // bind materials to shader instances
    materials = new_slice(sizeof(material));

    // debug material
    phong_params *material2_params = malloc(sizeof(phong_params));
    material2_params->phong_material = &phong_materials[PHONG_OBSIDIAN];
    material2_params->light = &positional_light;
    material material2 = new_debug_phong_material(
        root_shader, material2_params);
    append_slice(&materials, &material2);
    material *debug_material = get_slice_item(&materials, materials.len - 1);

    // lamp material
    lamp_shader_params *material3_params = malloc(sizeof(lamp_shader_params));
    material3_params->light = &positional_light;
    material material3 = lamp_material(lamp_shader, material3_params);
    append_slice(&materials, &material3);
    material *lamp_material = get_slice_item(&materials, materials.len - 1);

    // box texture
    textured_shader_params *material4_params = malloc(sizeof(textured_shader_params));
    GLuint container_id = loadTexture("assets/textures/container2.png");
    material4_params->texture_id = container_id;
    material material4 = textured_material(textured_shader, material4_params);
    append_slice(&materials, &material4);
    material *container_material = get_slice_item(&materials, materials.len - 1);

    // load in static objects
    slice files = new_slice(sizeof(size_t));
    const size_t *idx = slice_size_t_slice(&files, 0, files.len).data;
    int err = list_files(ASSET_DIR, &files);
    if (err < 0)
    {
        return err;
    }

    // register all found obj files as "static_objects"
    for (int x = 0; x < files.len; x++)
    {
        // create a material
        phong_params *obj_material_params = malloc(sizeof(phong_params));
        obj_material_params->phong_material = &phong_materials[x % PHONG_MATERIAL_COUNT];
        obj_material_params->light = &positional_light;
        material obj_material = new_phong_material(
            root_shader, obj_material_params);
        append_slice(&materials, &obj_material);
        material *obj_material_ptr = get_slice_item(&materials, materials.len - 1);

        // load  geometry
        const char *filename = slice_char_slice(&strings, idx[x], strings.len).data;
        model *current_model;
        mingl_basic_model_factory(&current_model);
        load_model(filename, current_model, &mingl_basic_mesh_factory);

        // add the new model to a simple scene
        scene *current_scene;
        mingl_basic_scene_factory(&current_scene);
        static_object(current_scene, &current_model, obj_material_ptr,
                      debug_material, x, files.len, x == 1);
    }

    // register a lamp into the world
    scene *lamp;
    mingl_basic_scene_factory(&lamp);
    lamp_scene(lamp, &positional_light, lamp_material);

    // register a box into the world
    scene *box;
    mingl_basic_scene_factory(&box);
    lamp_scene(box, NULL, container_material);
    box->update = &container_update;

    // initialize all scenes
    for (size_t i = 0; i < mingl_basic_scenes.len; i++)
    {
        scene *current_scene = get_slice_item(&mingl_basic_scenes, i);
        current_scene->init(current_scene, &mingl_basic_mesh_factory, &mingl_basic_model_factory);
    }

    // send all models to opengl
    for (size_t i = 0; i < mingl_basic_models.len; i++)
    {
        model *current_model = get_slice_item(&mingl_basic_models, i);
        // TODO: ensure every mesh is only loaded into OpenGL once. Right now
        //       its possible load a mesh more than once if its referenced by
        //       multiple models
        setup_model(*current_model);
    }

    // set some sane opengl defaults
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
    scene *scene_data = (scene *)mingl_basic_scenes.data;
    for (int x = 0; x < mingl_basic_scenes.len; x++)
    {
        scene *scene = &scene_data[x];
        scene->update(scene, dt, time);
    }
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
#endif
