
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

#define TAU 6.28318530718

static const char *vert_file = "src/shaders/learning.vert";
static const char *frag_file = "src/shaders/learning.frag";

static const struct vec3
{
    float x, y, z;
} vertices[] = {

    {-0.6f, -0.4f, 0.f},
    {0.6f, -0.4f, 0.f},
    {0.6f, 0.6f, 0.f},
    {-0.6f, 0.6f, 0.f},
    {-0.8f, 0.6f, 0.f},
    {-0.83f, 0.0f, 0.f}

    // {-0.6f, 0.6f, 0.f},
    // {-0.6f, -0.4f, 0.f},

    // {-0.6f, -0.4f, 0.f},
    // {0.6f, -0.4f, 0.f},
    // {0.6f, 0.6f, 0.f},

    // {0.6f, 0.6f, 0.f},
    // {-0.6f, 0.6f, 0.f},
    // {-0.6f, -0.4f, 0.f},
};

GLuint elements[] = {
    0, 1, 2,
    2, 3, 0,
    3, 4, 0,
    4, 5, 0};

static const struct
{
    float r, g, b;
} colors[] = {

    {1.f, 0.f, 0.f},
    {0.f, 1.f, 0.f},
    {0.f, 0.f, 1.f},
    {0.f, 0.f, 1.f},
    {1.f, 1.f, 1.f},
    {0.f, 0.f, 0.f},

    // {1.f, 0.f, 0.f},
    // {0.f, 1.f, 0.f},
    // {0.f, 0.f, 1.f},

    // {0.f, 0.f, 1.f},
    // {0.f, 0.f, 1.f},
    // {1.f, 0.f, 0.f},
};
static const struct
{
    float u, v;
} uvs[] = {
    {0.f, 0.f},
    {1.f, 0.f},
    {1.f, 1.f},
    {0.f, 1.f},
    {-0.25f, 1.f},
    {-0.28f, 0.5f},

    // {0.f, 0.f},
    // {1.f, 0.f},
    // {1.f, 1.f},

    // {1.f, 1.f},
    // {0.f, 1.f},
    // {0.f, 0.f},
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

struct uniforms
{
    GLuint vertex_array;
    GLuint program;
    const char *vert_file;
    const char *frag_file;
    int start;
    int count;

    GLint mvp_location;
    GLint time_location;
};

// GLfloat arcade_vertices[] = {
//     -0.134876, 0.704042, 0.194927, //
//     -0.134876, 0.783743, 0.212672,
//     -0.126545, 0.704042, 0.194927,
//     -0.126545, 0.766541, 0.208842,
//     -0.128985, 0.705323, 0.189176, //

//     -0.128985, 0.767821, 0.203092,
//     -0.134876, 0.705853, 0.186794,
//     -0.134876, 0.768352, 0.200710,
//     -0.140768, 0.705323, 0.189176,
//     -0.140768, 0.767821, 0.203092,
//     -0.143208, 0.704042, 0.194927,
//     -0.143208, 0.766541, 0.208842,
//     -0.140768, 0.702762, 0.200677,
//     -0.140768, 0.765260, 0.214593,
//     -0.134876, 0.702231, 0.203059,
//     -0.134876, 0.764730, 0.216975,
//     -0.128985, 0.702762, 0.200677,
//     -0.128985, 0.765260, 0.214593,
//     -0.140768, 0.714221, 0.191157,
//     -0.143208, 0.712941, 0.196908,
//     -0.140768, 0.711660, 0.202658,
//     -0.134876, 0.711130, 0.205040,
//     -0.128985, 0.711660, 0.202658,
//     -0.126545, 0.712941, 0.196908,
//     -0.128985, 0.714221, 0.191157,
//     -0.134876, 0.714751, 0.188775,
//     -0.108891, 0.704042, 0.194927,
//     -0.116502, 0.708036, 0.176991,
//     -0.134876, 0.709690, 0.169562,
//     -0.153251, 0.708036, 0.176991,
//     -0.160862, 0.704042, 0.194927,
//     -0.153251, 0.700049, 0.212862,
//     -0.134876, 0.698394, 0.220291,
//     -0.116502, 0.700049, 0.212862,
//     -0.153251, 0.716934, 0.178972,
//     -0.160862, 0.712941, 0.196908,
//     -0.153251, 0.708947, 0.214843,
//     -0.134876, 0.707293, 0.222272,
//     -0.116502, 0.708947, 0.214843,
//     -0.108891, 0.712941, 0.196908,
//     -0.116502, 0.716934, 0.178972,
//     -0.134876, 0.718588, 0.171543};

// GLuint arcade_elements[] = {
//     5 - 1, 3 - 1, 1 - 1,
//     28 - 1, 41 - 1, 40 - 1,
//     40 - 1, 27 - 1, 28 - 1};

// GLfloat arcade_vertices[] = {
//     -0.6f, -0.4f, 0.f,
//     0.6f, -0.4f, 0.f,
//     0.6f, 0.6f, 0.f,
//     -0.6f, 0.6f, 0.f,
//     -0.8f, 0.6f, 0.f,
//     -0.83f, 0.0f, 0.f};

// GLuint arcade_elements[] = {
//     0, 1, 2,
//     2, 3, 0,
//     3, 4, 0,
//     4, 5, 0};

const char *default_vert_file = "src/shaders/default/default.vert";
const char *default_frag_file = "src/shaders/default/default.frag";
const char *default_obj_file = "assets/arcade_obj.obj";
// const char *default_obj_file = "assets/Triceratops_base_mesh.obj";

struct uniforms configure_obj_example()
{
    struct slice vertices = {0, 0, sizeof(struct Vector3), NULL};
    line_callback_t *vertices_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&append_vertex, &vertices);

    struct slice faces = new_slice(sizeof(int));
    struct slice elements = new_slice(sizeof(int));
    line_callback_t *faces_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&read_face_line, &faces);

    line_reader(default_obj_file,
                vertices_callback,
                faces_callback);

    free_callback((callback_t)vertices_callback);
    free_callback((callback_t)faces_callback);

    printf("Found %d vertices\n", vertices.len);
    for (size_t i = 0; i < 10; i++)
    {
        struct Vector3 *vec = (struct Vector3 *)get_slice_item(&vertices, i);
        printf("x: %f, y: %f, z: %f\n", vec->x, vec->y, vec->z);
    }

    printf("Found %d face indexes\n", faces.len);
    int face_buffer[16];
    int idx_buffer[3];
    for (int i = 0; i < faces.len;)
    {
        int count = *(int *)get_slice_item(&faces, i);

        int start = i + 1;
        int end = start + count;
        for (i = start; i < end; i += 3)
        {
            int face_idx = ((end - (end - i)) - start) / 3;
            face_buffer[face_idx] = *(int *)get_slice_item(&faces, i);
        }
        for (int x = 0; x < (count / 3) - 2; x++)
        {
            if (x == 0)
            {
                idx_buffer[0] = face_buffer[0] - 1;
                idx_buffer[1] = face_buffer[1] - 1;
                idx_buffer[2] = face_buffer[2] - 1;
                // printf("%d, %d, %d\n",
                //        face_buffer[0],
                //        face_buffer[x + 1],
                //        face_buffer[x + 2]);
            }
            else
            {
                int offset = x + 1;
                idx_buffer[0] = face_buffer[offset] - 1;
                idx_buffer[1] = face_buffer[offset + 1] - 1;
                idx_buffer[2] = face_buffer[0] - 1;
                // printf("%d, %d, %d\n",
                //        face_buffer[offset],
                //        face_buffer[offset + 1],
                //        face_buffer[0]);
            }
            append_slice(&elements, &idx_buffer[0]);
            append_slice(&elements, &idx_buffer[1]);
            append_slice(&elements, &idx_buffer[2]);
        }
    }

    if (faces.cap > 0)
    {
        free(faces.data);
    }

    GLuint vertex_array, vertex_buffer, ebo;          //, color_buffer, uv_buffer;
    GLint mvp_location, time_location, vpos_location; //, vcol_location, vuv_location;

    GLuint program = create_shader_program_from_files(default_vert_file, default_frag_file);
    mvp_location = glGetUniformLocation(program, "projection_matrix");
    time_location = glGetUniformLocation(program, "time");
    vpos_location = glGetAttribLocation(program, "position");

    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    struct Vector3 *verts = (struct Vector3 *)vertices.data;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    size_t size = vertices.len * vertices.size;
    glBufferData(GL_ARRAY_BUFFER, size, verts, GL_STATIC_DRAW);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          vertices.size * 3, (void *)0);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    size = elements.len * elements.size;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 size, elements.data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(vpos_location);
    return (struct uniforms){
        vertex_array, program, default_vert_file, default_frag_file,
        0, vertices.len,
        mvp_location, time_location};
}

struct uniforms configure_learning_example()
{
    GLuint vertex_array, vertex_buffer, color_buffer, uv_buffer, ebo;
    GLint mvp_location, time_location, vpos_location, vcol_location, vuv_location;

    GLuint program = create_shader_program_from_files(vert_file, frag_file);
    // pull variable location
    mvp_location = glGetUniformLocation(program, "MVP");
    time_location = glGetUniformLocation(program, "time");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");
    vuv_location = glGetAttribLocation(program, "vUV");

    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)0);

    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(colors[0]), (void *)0);

    glGenBuffers(1, &uv_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
    glVertexAttribPointer(vuv_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(uvs[0]), (void *)0);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(elements), elements, GL_STATIC_DRAW);

    // enable attributes
    glEnableVertexAttribArray(vpos_location);
    glEnableVertexAttribArray(vcol_location);
    glEnableVertexAttribArray(vuv_location);

    return (struct uniforms){
        vertex_array, program, vert_file, frag_file, 0, sizeof(elements) / sizeof(elements[0]),
        mvp_location, time_location};
}

void generate_mvp(GLFWwindow *window, mat4x4 *mvp, float time)
{
    float ratio;
    int width, height;
    mat4x4 m, p;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    mat4x4_identity(m);
    mat4x4_rotate_Y(m, m, time / .5);
    mat4x4_rotate_Z(m, m, time / .5);
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(*mvp, p, m);
}

void render_object(
    mat4x4 *mvp,
    float time,
    struct uniforms *_uniforms)
{
    glUseProgram(_uniforms->program);
    glUniformMatrix4fv(_uniforms->mvp_location, 1, GL_FALSE, (const GLfloat *)mvp);
    glUniform1f(_uniforms->time_location, time);

    glBindVertexArray(_uniforms->vertex_array);

    glDrawElements(
        GL_TRIANGLES, _uniforms->count,
        GL_UNSIGNED_INT, (void *)(intptr_t)_uniforms->start);
    // glDrawArrays(GL_TRIANGLES, _uniforms->start, _uniforms->count);
    // glDrawArrays(GL_POINTS, _uniforms->start, _uniforms->count);
}

void handle_events(GLFWwindow *window, struct uniforms *_uniforms)
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
            &_uniforms->program,
            _uniforms->vert_file,
            _uniforms->frag_file);
        printf("reloaded shaders\n");
    }
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

    // struct uniforms uniforms = configure_learning_example();
    struct uniforms uniforms = configure_obj_example();

    glClearColor(.25, .25, .25, 1.0);
    // glPolygonMode(GL_FRONT, GL_LINE);
    mat4x4 mvp;
    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();
        _update_fps_counter(window, time);
        generate_mvp(window, &mvp, time);

        render_object(&mvp, time, &uniforms);

        glfwSwapBuffers(window);
        glfwPollEvents();

        handle_events(window, &uniforms);
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

int loading_example(void)
{
    struct slice vertices = {0, 0, sizeof(struct Vector3), NULL};
    line_callback_t *vertices_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&append_vertex, &vertices);

    struct slice faces = new_slice(sizeof(int));
    line_callback_t *faces_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&read_face_line, &faces);

    // line_reader("assets/Triceratops_base_mesh.obj",
    //             vertices_callback,
    //             faces_callback);
    line_reader("assets/arcade_obj.obj",
                vertices_callback,
                faces_callback);

    printf("Found %d vertices\n", vertices.len);
    for (size_t i = 0; i < 10; i++)
    {
        struct Vector3 *vec = (struct Vector3 *)get_slice_item(&vertices, i);
        printf("x: %f, y: %f, z: %f\n", vec->x, vec->y, vec->z);
    }

    printf("Found %d face indexes\n", faces.len);
    int face_buffer[16];
    for (int i = 0; i < 30;)
    {
        int count = *(int *)get_slice_item(&faces, i);

        int start = i + 1;
        int end = start + count;
        for (i = start; i < end; i += 3)
        {
            int face_idx = ((end - (end - i)) - start) / 3;
            face_buffer[face_idx] = *(int *)get_slice_item(&faces, i);

            printf("x: %d, y: %d, z: %d\n",
                   *(int *)get_slice_item(&faces, i),
                   *(int *)get_slice_item(&faces, i + 1),
                   *(int *)get_slice_item(&faces, i + 2));
        }
        for (int x = 0; x < (count / 3) - 2; x++)
        {
            if (x == 0)
            {
                printf("%d, %d, %d\n",
                       face_buffer[0],
                       face_buffer[x + 1],
                       face_buffer[x + 2]);
            }
            else
            {
                int offset = x + 1;
                printf("%d, %d, %d\n",
                       face_buffer[offset],
                       face_buffer[offset + 1],
                       face_buffer[0]);
            }
        }
        printf("Num Indexes: %d\n", count / 3);
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
