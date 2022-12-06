
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <callback.h>
#include <stdarg.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "linmath.h"
#include "shaders.h"
#include "obj3d.h"

int main(void)
{
    struct slice vertices = {0, 0, sizeof(struct Vector3), NULL};
    line_callback_t *vertices_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&append_vertex, &vertices);

    struct slice faces = new_slice(sizeof(int));
    line_callback_t *faces_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&read_face_line, &faces);

    line_reader("assets/Triceratops_base_mesh.obj",
                vertices_callback,
                faces_callback);
    // line_reader("assets/arcade_obj.obj",
    //             vertices_callback,
    //             faces_callback);

    printf("Found %d vertices\n", vertices.len);
    for (size_t i = 0; i < 10; i++)
    {
        struct Vector3 *vec = (struct Vector3 *)get_slice_item(&vertices, i);
        printf("x: %f, y: %f, z: %f\n", vec->x, vec->y, vec->z);
    }

    printf("Found %d face indexes\n", faces.len);
    for (int i = 0; i < 30;)
    {
        int indexes = *(int *)get_slice_item(&faces, i);
        int end = i + indexes;
        for (i = i + 1; i < end; i += 3)
        {
            printf("x: %d, y: %d, z: %d\n",
                   *(int *)get_slice_item(&faces, i),
                   *(int *)get_slice_item(&faces, i + 1),
                   *(int *)get_slice_item(&faces, i + 2));
        }
        printf("Num Indexes: %d\n", indexes / 3);
    }

    free_callback((callback_t)vertices_callback);
    free_callback((callback_t)faces_callback);

    if (vertices.cap > 0)
    {
        free(vertices.data);
    }
    if (faces.cap > 0)
    {
        free(faces.data);
    }
}

int test2(void)
{
    // parsing with error handling
    // const char *p = "   111.11 -2.22 Nan nan(2)\0\0 inF 0X1.BC70A3D70A3D7P+6  1.18973e+4932zzz";
    const char *p = "vn 4.500502 1.153829 0.924606\nv 4.445763 1.159163 0.949913";
    struct Vector3 result;
    int num = read_vec3(p + 1, &result);
    printf("x: %f, y: %f, z: %f':\n", result.x, result.y, result.z);
    printf("%d: '%s'", num, p + 1 + num);
    // printf("'%.*s'", 0, p + num);
    // printf("Parsing '%s':\n", p);
    // char *end;
    // p += 1;
    // for (double f = strtod(p, &end); p != end; f = strtod(p, &end))
    // {
    //     printf("'%.*s' -> ", (int)(end - p), p);
    //     p = end;
    //     if (errno == ERANGE)
    //     {
    //         printf("range error, got ");
    //         errno = 0;
    //     }
    //     printf("%f\n", f);
    // }
    // parsing without error handling
    // printf("\"  -0.0000000123junk\"  -->  %g\n", strtod("  -0.0000000123junk", NULL));
    // printf("\"junk\"                 -->  %g\n", strtod("junk", NULL));
}

#define TAU 6.28318530718

static const char *vert_file = "src/shaders/learning.vert";
static const char *frag_file = "src/shaders/learning.frag";

static const struct
{
    float x, y, z;
    float r, g, b;
    float u, v;
} vertices[6] = {

    {-0.6f, -0.4f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f},
    {0.6f, -0.4f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f},
    {0.6f, 0.6f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f},

    {0.6f, 0.6f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f},
    {-0.6f, 0.6f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f},
    {-0.6f, -0.4f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f},
};

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static double previous_seconds;

void _update_fps_counter(GLFWwindow *window)
{
    char tmp[128];

    static int frame_count;

    double current_seconds = glfwGetTime();
    double elapsed_seconds = current_seconds - previous_seconds;
    if (elapsed_seconds > 0.25)
    {
        previous_seconds = current_seconds;

        double fps = (double)frame_count / elapsed_seconds;
        sprintf(tmp, "opengl @ fps: %.2f", fps);
        glfwSetWindowTitle(window, tmp);
        frame_count = 0;
    }
    frame_count++;
}

struct _learning
{
    GLint mvp_location;
    GLint time_location;
};

struct _learning
configure_learning_example(GLuint *program)
{
    GLint mvp_location, time_location, vpos_location, vcol_location, vuv_location;
    GLuint program_val = create_shader_program_from_files(vert_file, frag_file);
    *program = program_val;

    // pull variable location
    mvp_location = glGetUniformLocation(program_val, "MVP");
    time_location = glGetUniformLocation(program_val, "time");
    vpos_location = glGetAttribLocation(program_val, "vPos");
    vcol_location = glGetAttribLocation(program_val, "vCol");
    vuv_location = glGetAttribLocation(program_val, "vUV");

    // map buffer to variables
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)0);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)(sizeof(float) * 3));
    glVertexAttribPointer(vuv_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)(sizeof(float) * 6));

    // enable attributes
    glEnableVertexAttribArray(vpos_location);
    glEnableVertexAttribArray(vcol_location);
    glEnableVertexAttribArray(vuv_location);

    struct _learning res;
    res.mvp_location = mvp_location;
    res.time_location = time_location;
    return res;
}

void render_learning_example(
    GLFWwindow *window,
    GLuint program,
    void *input)
{
    struct _learning *uniforms = (struct _learning *)input;
    float ratio;
    float time;
    int width, height;
    mat4x4 m, p, mvp;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;
    time = (float)glfwGetTime();
    // printf("%f\n", time);

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    mat4x4_identity(m);
    mat4x4_rotate_Z(m, m, time);
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(mvp, p, m);

    glUseProgram(program);
    glUniformMatrix4fv(uniforms->mvp_location, 1, GL_FALSE, (const GLfloat *)mvp);
    glUniform1f(uniforms->time_location, time);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main1(void)
{
    GLFWwindow *window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    // GLint mvp_location, time_location, vpos_location, vcol_location, vuv_location;

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

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    struct _learning uniforms = configure_learning_example(&program);

    glClearColor(.25, .25, .25, 1.0);
    // glPolygonMode(GL_FRONT, GL_LINE);
    while (!glfwWindowShouldClose(window))
    {
        _update_fps_counter(window);

        render_learning_example(window, program, &uniforms);

        glfwSwapBuffers(window);
        glfwPollEvents();

        static bool reload_key_pressed = false;
        bool down = glfwGetKey(window, GLFW_KEY_R);
        if (down && !reload_key_pressed)
        {
            reload_key_pressed = true;
        }
        else if (!down && reload_key_pressed)
        {
            reload_key_pressed = false;
            reload_shader_program_from_files(&program, vert_file, frag_file);
            printf("reloaded shaders\n");
        }
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
