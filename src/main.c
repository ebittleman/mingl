
#include <dirent.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "camera.h"
#include "linmath.h"
#include "obj3d.h"
#include "shaders.h"
#include "types.h"

#include "uv.h"

#define TAU 6.28318530718

typedef const char *string;

static const char *vert_file = "src/shaders/learning.vert";
static const char *frag_file = "src/shaders/learning.frag";

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

void calculate_normals(slice buffer_slices[COUNT_BUFFERS]);

void calculate_normals(slice buffer_slices[COUNT_BUFFERS])
{

    reset_slice(&buffer_slices[NORMALS],
                buffer_slices[VERTS].size,
                buffer_slices[VERTS].len,
                buffer_slices[VERTS].cap);

    // int *elements = (int *)buffer_slices[ELEMENTS].data;
    vec3 *vertices = (vec3 *)buffer_slices[VERTS].data;
    vec3 *normals = (vec3 *)buffer_slices[NORMALS].data;
    // memset(normals, 0, buffer_slices[NORMALS].size * buffer_slices[NORMALS].cap);

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
        // vec3_add(n1, n1, normal);
        // vec3_add(n2, n2, normal);
        // vec3_add(n3, n3, normal);
    }

    // for (int x = 0; x < buffer_slices[NORMALS].len / 3; x++)
    // {
    //     vec3_norm(normals[x], normals[x]);
    // }
    return;
}

void inject_uv(slice buffer_slices[COUNT_BUFFERS], float *uvs)
{
    if (buffer_slices[UVS].data != NULL)
    {
        free(buffer_slices[UVS].data);
    }
    buffer_slices[UVS].data = (void *)uvs;
    buffer_slices[UVS].size = sizeof(float);
    buffer_slices[UVS].cap = sizeof(s_uv);
    buffer_slices[UVS].len = buffer_slices[UVS].cap / buffer_slices[UVS].size;
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

    debug_mat(m);

    // printf("%f %f %f %f\n", m[0][0], m[0][1], m[0][2], m[0][3]);
    // printf("%f %f %f %f\n", m[1][0], m[1][1], m[1][2], m[1][3]);
    // printf("%f %f %f %f\n", m[2][0], m[2][1], m[2][2], m[2][3]);
    // printf("%f %f %f %f\n", m[3][0], m[3][1], m[3][2], m[3][3]);
}

void calculate_model_position(mat4x4 m, mat4x4 obj, float time)
{
    mat4x4_dup(m, obj);
    // mat4x4_identity(m);
    // mat4x4_scale(m, m, 1.0f / 12.0f);
    // mat4x4_translate_in_place(m, .1f, .2f, .5f);

    // mat4x4_rotate_X(m, m, time * TAU * .1);
    // mat4x4_rotate_Y(m, m, time * TAU * .1);
    // mat4x4_rotate_Z(m, m, time * TAU * .1);

    // mat4x4_mul(m, m, obj);
    // debug_mat(m);
}

void calculate_projection_matrix(GLFWwindow *window, mat4x4 *p)
{
    float ratio;
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // mat4x4_ortho(*p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_perspective(*p, 120.0f, ratio, 0.1f, 1000.0f);
}

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "OpenGL Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static double previous_seconds;

void _update_fps_counter(GLFWwindow *window, float current_seconds)
{
    char tmp[128];

    static int frame_count;

    float elapsed_seconds = current_seconds - previous_seconds;
    if (elapsed_seconds > 0.25)
    {
        previous_seconds = current_seconds;

        float fps = (float)frame_count / elapsed_seconds;
        sprintf(tmp, "opengl @ fps: %.2f", fps);
        glfwSetWindowTitle(window, tmp);
        frame_count = 0;
    }
    frame_count++;
}

const char *default_vert_file = "src/shaders/default/default.vert";
const char *default_frag_file = "src/shaders/default/default.frag";

void set_uniforms_and_inputs(
    GLuint id, GLint uniforms[COUNT_UNIFORMS],
    GLint input_locations[ELEMENTS])
{
    uniforms[U_MVP] = glGetUniformLocation(id, "mvp_matrix");
    uniforms[U_MODEL] = glGetUniformLocation(id, "model_matrix");
    uniforms[U_VIEW] = glGetUniformLocation(id, "view_matrix");
    uniforms[U_PROJECTION] = glGetUniformLocation(id, "proj_matrix");
    uniforms[U_CAM_POS] = glGetUniformLocation(id, "cam_pos");
    uniforms[U_TIME] = glGetUniformLocation(id, "time");

    // buffer attributes
    for (int x = 0; x < ELEMENTS; x++)
    {
        input_locations[x] = glGetAttribLocation(id, IN_NAMES[x]);
    }
}

void load_program(Program *program, const char *vert_file, const char *frag_file)
{
    GLuint id = create_shader_program_from_files(vert_file, frag_file);
    program->id = id;
    program->vert_file = vert_file;
    program->frag_file = frag_file;

    set_uniforms_and_inputs(id, program->uniforms, program->input_locations);
}

void configure_vertex_attributes(
    GLuint vertex_buffers[COUNT_BUFFERS],
    GLint input_locations[ELEMENTS],
    bool buffer_disabled[COUNT_BUFFERS])
{
    for (int x = 0; x < ELEMENTS; x++)
    {
        if (buffer_disabled[x])
        {
            // printf("disabled: %s\n", IN_NAMES[x]);
            glDisableVertexAttribArray(input_locations[x]);
        }
        else
        {
            // printf("enabled: %s\n", IN_NAMES[x]);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[x]);
            glVertexAttribPointer(
                input_locations[x], BUFFER_SIZES[x], GL_FLOAT,
                GL_FALSE, 0, (void *)0);
            glEnableVertexAttribArray(input_locations[x]);
        }
    }
}

void buffer_object_to_vao(object *o)
{

    glGenVertexArrays(1, &o->vertex_array);
    glBindVertexArray(o->vertex_array);
    glGenBuffers(COUNT_BUFFERS, o->vertex_buffers);

    for (int x = 0; x < ELEMENTS; x++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, o->vertex_buffers[x]);
        glBufferData(GL_ARRAY_BUFFER,
                     o->buffer_slices[x].len * o->buffer_slices[x].size,
                     o->buffer_slices[x].data, GL_STATIC_DRAW);
        o->buffer_disabled[x] = !o->buffer_slices[x].len;
    }

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o->vertex_buffers[ELEMENTS]);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    //              o->buffer_slices[ELEMENTS].len * o->buffer_slices[ELEMENTS].size,
    //              o->buffer_slices[ELEMENTS].data, GL_STATIC_DRAW);
    // o->buffer_disabled[ELEMENTS] = !o->buffer_slices[ELEMENTS].len;

    // o->element_count = o->buffer_slices[ELEMENTS].len;
    o->element_count = o->buffer_slices[VERTS].len;

    configure_vertex_attributes(
        o->vertex_buffers, o->program->input_locations,
        o->buffer_disabled);

    glBindVertexArray(0);
    return;
}

void render_object(
    GLuint vertex_array,
    size_t element_count,

    Program program,
    vec3 cam_pos,

    mat4x4 proj,
    mat4x4 view,
    mat4x4 model,
    float time)
{

    mat4x4 mvp;
    mat4x4_mul(mvp, proj, view);
    mat4x4_mul(mvp, mvp, model);

    glUseProgram(program.id);

    glUniformMatrix4fv(program.uniforms[U_MVP], 1, GL_FALSE, (const GLfloat *)&mvp);
    glUniformMatrix4fv(program.uniforms[U_MODEL], 1, GL_FALSE, (const GLfloat *)model);
    glUniformMatrix4fv(program.uniforms[U_VIEW], 1, GL_FALSE, (const GLfloat *)view);
    glUniformMatrix4fv(program.uniforms[U_PROJECTION], 1, GL_FALSE, (const GLfloat *)proj);

    glUniform3fv(program.uniforms[U_CAM_POS], 1, cam_pos);

    glUniform1f(program.uniforms[U_TIME], time);

    glBindVertexArray(vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, element_count);
    // glDrawElements(
    //     GL_TRIANGLES, element_count,
    //     GL_UNSIGNED_INT, (void *)0);
    glBindVertexArray(0);
}

void render_shape()
{
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

size_t d_strlen(const char *in)
{
    size_t len = 0;
    while (!(in[len] == '\0' && in[len + 1] == '\0'))
        ++len;
    return len;
}

int main(void)
{
    GLFWwindow *window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 16);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    int gl_ver_major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    int gl_ver_minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    printf("OpenGL %d.%d\n", gl_ver_major, gl_ver_minor);
    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version: %s\n", glGetString(GL_VERSION));
    // printf("Extensions: %s\n", glGetString(GL_EXTENSIONS));

    // NOTE: OpenGL error checks have been omitted for brevity

    Program program;
    load_program(&program, default_vert_file, default_frag_file);

    slice files = new_slice(sizeof(size_t));
    size_t *idx = (size_t *)files.data;
    int err = list_files(ASSET_DIR, &files);
    if (err < 0)
    {
        return err;
    }

    object objects[COUNT];
    for (int x = 0; x < COUNT; x++)
    {
        char *filename = strings.data + idx[x];
        float bounds[6] = {0};
        objects[x].program = &program;
        memset(objects[x].buffer_slices, 0, sizeof(objects[x].buffer_slices));
        load_obj_file(filename, objects[x].buffer_slices, bounds);

        if (!objects[x].buffer_slices[NORMALS].len)
        {
            calculate_normals(objects[x].buffer_slices);
        }

        if (x == 0)
        {
            inject_uv(objects[x].buffer_slices, s_uv);
        }
        if (x == 3)
        {
            inject_uv(objects[x].buffer_slices, ttop_uv);
        }
        init_obj_model(objects[x].model, bounds, x, COUNT);

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

    struct Cam cam = new_cam();
    mat4x4 proj;

    mat4x4 model;
    while (!glfwWindowShouldClose(window))
    {
        static double previous_seconds = 0.0;
        double current_seconds = glfwGetTime();
        double elapsed_seconds = current_seconds - previous_seconds;
        previous_seconds = current_seconds;

        float time = (float)current_seconds;
        _update_fps_counter(window, time);
        calculate_projection_matrix(window, &proj);
        handle_camera_events(window, elapsed_seconds, &cam);
        bool reloaded = handle_events(window, &program);

        for (int x = 0; x < COUNT; x++)
        {
            object obj = objects[x];
            calculate_model_position(model, obj.model, time);
            if (reloaded)
            {
                glUseProgram(obj.program->id);
                glBindVertexArray(obj.vertex_array);
                configure_vertex_attributes(
                    obj.vertex_buffers, obj.program->input_locations,
                    obj.buffer_disabled);
            }
            render_object(
                obj.vertex_array, obj.element_count,
                *obj.program, cam.cam_pos, proj, cam.view,
                model, time);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
