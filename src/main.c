
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "camera.h"
#include "linmath.h"
#include "obj3d.h"
#include "shaders.h"
#include "types.h"

#define TAU 6.28318530718

static const char *vert_file = "src/shaders/learning.vert";
static const char *frag_file = "src/shaders/learning.frag";

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

struct Program
{
    GLuint id;
    const char *vert_file;
    const char *frag_file;
    GLint uniforms[COUNT_UNIFORMS];
    GLint buffers[COUNT_BUFFERS - 1];
};

void load_program(struct Program *program, const char *vert_file, const char *frag_file)
{
    GLuint id = create_shader_program_from_files(vert_file, frag_file);
    program->id = id;
    program->vert_file = vert_file;
    program->frag_file = frag_file;

    program->uniforms[U_MVP] = glGetUniformLocation(id, "mvp_matrix");
    program->uniforms[U_MODEL] = glGetUniformLocation(id, "model_matrix");
    program->uniforms[U_VIEW] = glGetUniformLocation(id, "view_matrix");
    program->uniforms[U_PROJECTION] = glGetUniformLocation(id, "proj_matrix");
    program->uniforms[U_TIME] = glGetUniformLocation(id, "time");

    // buffer attributes
    for (int x = 0; x < ELEMENTS; x++)
    {
        program->buffers[x] = glGetAttribLocation(id, IN_NAMES[x]);
    }
}

void setup_vertex_attributes(
    struct Program program,
    GLuint vertex_buffers[COUNT_BUFFERS],
    bool buffer_disabled[COUNT_BUFFERS])
{
    for (int x = 0; x < ELEMENTS; x++)
    {
        if (buffer_disabled[x])
        {
            glDisableVertexAttribArray(program.buffers[x]);
        }
        else
        {
            // printf("enabled: %s\n", IN_NAMES[x]);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[x]);
            glVertexAttribPointer(
                program.buffers[x], BUFFER_SIZES[x], GL_FLOAT,
                GL_FALSE, 0, (void *)0);
            glEnableVertexAttribArray(program.buffers[x]);
        }
    }
}

typedef struct _object
{
    struct Program program;
    GLuint vertex_array;
    GLuint vertex_buffers[COUNT_BUFFERS];
    struct slice buffer_slices[COUNT_BUFFERS];
    bool buffer_disabled[COUNT_BUFFERS];
    size_t element_count;
    mat4x4 model;
} object;

void calculate_normals(struct slice buffer_slices[COUNT_BUFFERS]);

void setup_object_buffers(object *o)
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o->vertex_buffers[ELEMENTS]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 o->buffer_slices[ELEMENTS].len * o->buffer_slices[ELEMENTS].size,
                 o->buffer_slices[ELEMENTS].data, GL_STATIC_DRAW);
    o->buffer_disabled[ELEMENTS] = !o->buffer_slices[ELEMENTS].len;

    o->element_count = o->buffer_slices[ELEMENTS].len;

    glBindVertexArray(0);
    return;
}

void calculate_normals(struct slice buffer_slices[COUNT_BUFFERS])
{
    if (buffer_slices[NORMALS].data != NULL)
    {
        free(buffer_slices[NORMALS].data);
        buffer_slices[NORMALS].data = NULL;
    }
    buffer_slices[NORMALS].size = buffer_slices[VERTS].size;
    buffer_slices[NORMALS].cap = buffer_slices[VERTS].cap;
    buffer_slices[NORMALS].len = buffer_slices[VERTS].len;

    buffer_slices[NORMALS].data = malloc(
        buffer_slices[NORMALS].cap * buffer_slices[NORMALS].size);
    memset(
        buffer_slices[NORMALS].data, 0,
        buffer_slices[NORMALS].cap * buffer_slices[NORMALS].size);

    printf("Memory Initialized for normal calculations.\n");

    int *elements = (int *)buffer_slices[ELEMENTS].data;
    vec3 *vertices = (vec3 *)buffer_slices[VERTS].data;
    vec3 *normals = (vec3 *)buffer_slices[NORMALS].data;

    for (int x = 0; x < buffer_slices[ELEMENTS].len; x += 3)
    {
        vec3 normal, a, b;
        float *p1 = vertices[elements[x]];
        float *p2 = vertices[elements[x + 1]];
        float *p3 = vertices[elements[x + 2]];

        vec3_sub(a, p2, p1);
        vec3_sub(a, p3, p1);
        vec3_mul_cross(normal, a, b);

        float *n1 = normals[elements[x]];
        float *n2 = normals[elements[x + 1]];
        float *n3 = normals[elements[x + 2]];

        vec3_add(n1, n1, normal);
        vec3_add(n2, n2, normal);
        vec3_add(n3, n3, normal);
    }

    for (int x = 0; x < buffer_slices[NORMALS].len / 3; x++)
    {
        vec3_norm(normals[x], normals[x]);
    }
}

void render_object(
    GLuint vertex_array,
    size_t element_count,
    GLuint vertex_buffers[COUNT_BUFFERS],
    bool buffer_disabled[COUNT_BUFFERS],
    struct Program program,
    mat4x4 proj,
    mat4x4 view,
    mat4x4 model,
    float time)
{

    mat4x4 mvp;
    mat4x4_mul(mvp, proj, view);
    mat4x4_mul(mvp, mvp, model);

    glUseProgram(program.id);
    glBindVertexArray(vertex_array);

    glUniformMatrix4fv(program.uniforms[U_MVP], 1, GL_FALSE, (const GLfloat *)&mvp);
    glUniformMatrix4fv(program.uniforms[U_MODEL], 1, GL_FALSE, (const GLfloat *)model);
    glUniformMatrix4fv(program.uniforms[U_VIEW], 1, GL_FALSE, (const GLfloat *)view);
    glUniformMatrix4fv(program.uniforms[U_PROJECTION], 1, GL_FALSE, (const GLfloat *)proj);
    glUniform1f(program.uniforms[U_TIME], time);
    setup_vertex_attributes(program, vertex_buffers, buffer_disabled);

    // glDrawElements(
    //     GL_QUADS, element_count,
    //     GL_UNSIGNED_INT, (void *)0);
    glDrawElements(
        GL_TRIANGLES, element_count,
        GL_UNSIGNED_INT, (void *)0);
    glBindVertexArray(0);
    // glDrawArrays(GL_TRIANGLES, _uniforms->start, _uniforms->count);
    // glDrawArrays(GL_POINTS, _uniforms->start, _uniforms->count);
}

void debug_mat(mat4x4 m)
{
    printf("%f %f %f %f\n", m[0][0], m[0][1], m[0][2], m[0][3]);
    printf("%f %f %f %f\n", m[1][0], m[1][1], m[1][2], m[1][3]);
    printf("%f %f %f %f\n", m[2][0], m[2][1], m[2][2], m[2][3]);
    printf("%f %f %f %f\n", m[3][0], m[3][1], m[3][2], m[3][3]);
}

void generate_model(mat4x4 m, mat4x4 obj, float time)
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

void generate_projection(GLFWwindow *window, mat4x4 *p)
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

void handle_events(GLFWwindow *window, struct Program *program)
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
        printf("reloaded shaders\n");
    }
}

void init_obj_model(mat4x4 m, float bounds[6], int x)
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
    mat4x4_identity(m);
    mat4x4_translate_in_place(
        m,
        (-(bounds[0] * scale) - (ranges[0] / 2.0) * scale) + (x * 2),
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

    struct Program program;
    load_program(&program, default_vert_file, default_frag_file);

#define COUNT 6
    const char *obj_files[] = {
        "assets/only_quad_sphere.obj",
        "assets/arcade_obj.obj",
        "assets/star.obj",
        "assets/Triceratops_base_mesh.obj",
        "assets/abstract_object.obj",
        "assets/math_form_1_obj.obj",
    };

    object objects[COUNT];
    for (int x = 0; x < COUNT; x++)
    {
        float bounds[6] = {0};
        objects[x].program = program;
        memset(objects[x].buffer_slices, 0, sizeof(objects[x].buffer_slices));
        load_obj_file(obj_files[x], objects[x].buffer_slices, bounds);
        init_obj_model(objects[x].model, bounds, x);
        if (!objects[x].buffer_slices[NORMALS].len)
        {
            printf("Need to calculate normals for this one.\n");
            calculate_normals(objects[x].buffer_slices);
        }
        setup_object_buffers(&objects[x]);
    }

    glClearColor(.25, .25, .25, 1.0);
    // glPolygonMode(GL_FRONT, GL_LINE);
    // glPolygonMode(GL_BACK, GL_LINE);
    // glEnable(GL_CULL_FACE); // cull face
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
        generate_projection(window, &proj);

        for (int x = 0; x < COUNT; x++)
        {
            object obj = objects[x];
            generate_model(model, obj.model, time);
            render_object(
                obj.vertex_array, obj.element_count, obj.vertex_buffers,
                obj.buffer_disabled, program, proj, cam.view,
                model, time);
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
        handle_camera_events(window, elapsed_seconds, &cam);
        handle_events(window, &program);
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
