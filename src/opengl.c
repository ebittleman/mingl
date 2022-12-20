
#include <stdlib.h>
#include <stdio.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "camera.h"
#include "types.h"
#include "linmath.h"
#include "opengl.h"
#include "shaders.h"

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

void _update_fps_counter(GLFWwindow *window, double current_seconds)
{
    static double previous_seconds = 0.0;
    char tmp[128];

    static int frame_count;

    double elapsed_seconds = current_seconds - previous_seconds;
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
    double time)
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

    glUniform1f(program.uniforms[U_TIME], (float)time);

    glBindVertexArray(vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, element_count);
    // glDrawElements(
    //     GL_TRIANGLES, element_count,
    //     GL_UNSIGNED_INT, (void *)0);
    glBindVertexArray(0);
}

GLFWwindow *init_opengl(init_func_t *init_func)
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
    int err = init_func(window);
    if (err != 0)
    {
        glfwTerminate();
        exit(err);
    }
    return window;
}

void update_loop(GLFWwindow *window, update_func_t *update_func, int n, object *objects)
{
    struct Cam cam = new_cam();
    mat4x4 proj;

    while (!glfwWindowShouldClose(window))
    {
        static double previous_seconds = 0.0;
        double current_seconds = glfwGetTime();
        double dt = current_seconds - previous_seconds;
        previous_seconds = current_seconds;

        _update_fps_counter(window, current_seconds);
        calculate_projection_matrix(window, &proj);
        handle_camera_events(window, dt, &cam);

        update_func(window, current_seconds, dt);

        for (int x = 0; x < n; x++)
        {
            object obj = objects[x];
            render_object(
                obj.vertex_array, obj.element_count,
                *obj.program, cam.cam_pos, proj, cam.view,
                obj.model, current_seconds);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}
