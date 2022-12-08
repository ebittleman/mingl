
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

const char *default_vert_file = "src/shaders/default/default.vert";
const char *default_frag_file = "src/shaders/default/default.frag";
// const char *default_obj_file = "assets/star.obj";
const char *default_obj_file = "assets/arcade_obj.obj";
// const char *default_obj_file = "assets/Triceratops_base_mesh.obj";

enum Buffers
{
    VERTS = 0,
    UVS,
    NORMALS,
    ELEMENTS,
    COUNT_BUFFERS
};

enum Uniforms
{
    U_MVP = 1,
    U_MODEL,
    U_VIEW,
    U_PROJECTION,
    U_TIME,
    COUNT_UNIFORMS
};

struct Program
{
    GLuint id;
    const char *vert_file;
    const char *frag_file;
    GLint uniforms[COUNT_UNIFORMS];
    GLint buffers[COUNT_BUFFERS - 1];
};

struct vertex_buffers
{
    struct slice vertices;
    struct slice elements;
    struct slice normals;
    struct slice uvs;
};

void load_obj_file(const char *obj_file_name, struct slice buffers[COUNT_BUFFERS])
{
    struct slice vertices_slice = {0, 0, sizeof(float), NULL};
    line_callback_t *vertices_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&handle_vertex_line, &vertices_slice);

    struct slice faces = new_slice(sizeof(int));
    line_callback_t *faces_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&handle_face_line, &faces);

    struct slice normals_slice = new_slice(sizeof(float));
    line_callback_t *normals_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&handle_normal_line, &normals_slice);

    struct slice uv_slice = new_slice(sizeof(float));
    line_callback_t *uv_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&handle_uv_line, &uv_slice);

    line_reader(obj_file_name,
                vertices_callback,
                faces_callback,
                normals_callback,
                uv_callback);

    free_callback((callback_t)vertices_callback);
    free_callback((callback_t)faces_callback);
    free_callback((callback_t)normals_callback);
    free_callback((callback_t)uv_callback);

    struct slice elements_slice = faces_to_elements(faces);
    if (faces.cap > 0)
    {
        free(faces.data);
    }

    // printf("Found %d vertices\n", vertices.len);
    // for (size_t i = 0; i < vertices.len; i+=3)
    // {
    //     printf("x: %f, y: %f, z: %f\n", verts[i], verts[i+1], verts[i+2]);
    // }

    // printf("Found %d elements\n", elements_slice.len);
    // for (size_t i = 0; i < elements_slice.len; i += 3)
    // {
    //     printf("x: %d, y: %d, z: %d\n",
    //            elements[i],
    //            elements[i + 1],
    //            elements[i + 2]);
    // }

    buffers[VERTS] = vertices_slice;
    buffers[UVS] = uv_slice;
    buffers[NORMALS] = normals_slice;
    buffers[ELEMENTS] = elements_slice;
    return;
}

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
    program->buffers[VERTS] = glGetAttribLocation(id, "position");
    program->buffers[UVS] = glGetAttribLocation(id, "uv");
    program->buffers[NORMALS] = glGetAttribLocation(id, "normal");
}

size_t configure_obj_example(
    GLuint *vertex_array,
    GLuint vertex_buffers[COUNT_BUFFERS],
    struct slice buffer_slices[COUNT_BUFFERS],
    bool buffer_disabled[COUNT_BUFFERS],
    struct Program program)
{
    load_obj_file(default_obj_file, buffer_slices);

    glGenVertexArrays(1, vertex_array);
    glBindVertexArray(*vertex_array);
    glGenBuffers(COUNT_BUFFERS, vertex_buffers);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[VERTS]);
    glBufferData(GL_ARRAY_BUFFER,
                 buffer_slices[VERTS].len * buffer_slices[VERTS].size,
                 buffer_slices[VERTS].data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[UVS]);
    glBufferData(GL_ARRAY_BUFFER,
                 buffer_slices[UVS].len * buffer_slices[UVS].size,
                 buffer_slices[UVS].data, GL_STATIC_DRAW);
    if (!buffer_slices[UVS].len)
    {
        buffer_disabled[UVS] = true;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[NORMALS]);
    glBufferData(GL_ARRAY_BUFFER,
                 buffer_slices[NORMALS].len * buffer_slices[NORMALS].size,
                 buffer_slices[NORMALS].data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffers[ELEMENTS]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 buffer_slices[ELEMENTS].len * buffer_slices[ELEMENTS].size,
                 buffer_slices[ELEMENTS].data, GL_STATIC_DRAW);

    glBindVertexArray(0);
    return buffer_slices[ELEMENTS].len;
}

void setup_vertex_attributes(
    struct Program program,
    GLuint vertex_buffers[COUNT_BUFFERS],
    bool buffer_disabled[COUNT_BUFFERS])
{

    // define an arrays of generic vertex attribute data
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[VERTS]);
    glVertexAttribPointer(program.buffers[VERTS], 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(program.buffers[VERTS]);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[NORMALS]);
    glVertexAttribPointer(program.buffers[NORMALS], 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(program.buffers[NORMALS]);

    if (buffer_disabled[UVS])
    {
        glDisableVertexAttribArray(program.buffers[UVS]);
        return;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[UVS]);
    glVertexAttribPointer(program.buffers[UVS], 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(program.buffers[UVS]);
}

void render_object(
    GLuint vertex_array,
    size_t element_count,
    GLuint vertex_buffers[COUNT_BUFFERS],
    bool buffer_disabled[COUNT_BUFFERS],
    struct Program program,
    mat4x4 *proj,
    mat4x4 *view,
    mat4x4 *model,
    float time)
{

    mat4x4 mvp;
    mat4x4_mul(mvp, *proj, *view);
    mat4x4_mul(mvp, mvp, *model);

    glUseProgram(program.id);
    glBindVertexArray(vertex_array);

    glUniformMatrix4fv(program.uniforms[U_MVP], 1, GL_FALSE, (const GLfloat *)&mvp);
    glUniformMatrix4fv(program.uniforms[U_MODEL], 1, GL_FALSE, (const GLfloat *)model);
    glUniformMatrix4fv(program.uniforms[U_VIEW], 1, GL_FALSE, (const GLfloat *)view);
    glUniformMatrix4fv(program.uniforms[U_PROJECTION], 1, GL_FALSE, (const GLfloat *)proj);
    glUniform1f(program.uniforms[U_TIME], time);
    setup_vertex_attributes(program, vertex_buffers, buffer_disabled);

    glDrawElements(
        GL_TRIANGLES, element_count,
        GL_UNSIGNED_INT, (void *)0);
    glBindVertexArray(0);
    // glDrawArrays(GL_TRIANGLES, _uniforms->start, _uniforms->count);
    // glDrawArrays(GL_POINTS, _uniforms->start, _uniforms->count);
}

// struct uniforms configure_learning_example()
// {
//     GLuint vertex_array, vertex_buffer, color_buffer, uv_buffer, ebo;
//     GLint mvp_location, proj_location, view_location, model_location, time_location, vpos_location, vcol_location, vuv_location;

//     GLuint program = create_shader_program_from_files(vert_file, frag_file);
//     // pull variable location
//     mvp_location = glGetUniformLocation(program, "mvp_matrix");
//     proj_location = glGetUniformLocation(program, "proj_matrix");
//     view_location = glGetUniformLocation(program, "view_matrix");
//     model_location = glGetUniformLocation(program, "model_matrix");
//     time_location = glGetUniformLocation(program, "time");
//     vpos_location = glGetAttribLocation(program, "vPos");
//     vcol_location = glGetAttribLocation(program, "vCol");
//     vuv_location = glGetAttribLocation(program, "vUV");

//     glGenVertexArrays(1, &vertex_array);
//     glBindVertexArray(vertex_array);

//     glGenBuffers(1, &vertex_buffer);
//     glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//     glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
//                           sizeof(vertices[0]), (void *)0);

//     glGenBuffers(1, &color_buffer);
//     glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
//     glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
//                           sizeof(colors[0]), (void *)0);

//     glGenBuffers(1, &uv_buffer);
//     glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
//     glVertexAttribPointer(vuv_location, 2, GL_FLOAT, GL_FALSE,
//                           sizeof(uvs[0]), (void *)0);

//     glGenBuffers(1, &ebo);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//                  sizeof(elements), elements, GL_STATIC_DRAW);

//     // enable attributes
//     glEnableVertexAttribArray(vpos_location);
//     glEnableVertexAttribArray(vcol_location);
//     glEnableVertexAttribArray(vuv_location);

//     int num_elements = sizeof(elements) / sizeof(elements[0]);
//     return (struct uniforms){
//         vertex_array, program, vert_file, frag_file, 0, num_elements,
//         mvp_location, proj_location, view_location, model_location, time_location};
// }

struct Cam
{
    float cam_speed;
    float cam_yaw_speed;
    float cam_pos[3];
    float cam_yaw;
    mat4x4 T;
    mat4x4 R;
    mat4x4 view;
};

void update_cam(struct Cam *cam);

void generate_model(mat4x4 *m, float time)
{
    mat4x4_identity(*m);
    // mat4x4_scale(m, m, 1.0f / 12.0f);
    // mat4x4_translate_in_place(m, .1f, .2f, .5f);

    // mat4x4_rotate_Z(*m, *m, time / .5);
    // mat4x4_rotate_Y(*m, *m, time * TAU * .1);
    // mat4x4_rotate_X(*m, *m, time * TAU * .1);
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
    mat4x4_perspective(*p, 90.0f, ratio, 0.1f, 100.0f);
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

struct Cam new_cam()
{
    struct Cam cam;

    cam.cam_speed = 1.0f;
    cam.cam_yaw_speed = 5.0f;
    cam.cam_pos[0] = -1.1f;
    cam.cam_pos[1] = 1.62f;
    cam.cam_pos[2] = 4.72f;
    // cam.cam_pos[0] = 0.0f;
    // cam.cam_pos[1] = 0.0f;
    // cam.cam_pos[2] = 0.0f;
    cam.cam_yaw = 0.0f;

    update_cam(&cam);

    return cam;
}

void update_cam(struct Cam *cam)
{
    mat4x4_translate(cam->T, -cam->cam_pos[0], -cam->cam_pos[1], -cam->cam_pos[2]);
    mat4x4_identity(cam->R);
    mat4x4_rotate_Y(cam->R, cam->R, -cam->cam_yaw);
    mat4x4_mul(cam->view, cam->R, cam->T);
}

void handle_camera_events(
    GLFWwindow *window, double elapsed_seconds, struct Cam *cam)
{
    bool cam_moved = false;
    if (glfwGetKey(window, GLFW_KEY_A))
    {
        cam->cam_pos[0] -= cam->cam_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D))
    {
        cam->cam_pos[0] += cam->cam_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP))
    {
        cam->cam_pos[1] += cam->cam_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN))
    {
        cam->cam_pos[1] -= cam->cam_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_W))
    {
        cam->cam_pos[2] -= cam->cam_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S))
    {
        cam->cam_pos[2] += cam->cam_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT))
    {
        cam->cam_yaw += cam->cam_yaw_speed * elapsed_seconds;
        cam_moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT))
    {
        cam->cam_yaw -= cam->cam_yaw_speed * elapsed_seconds;
        cam_moved = true;
    }

    if (cam_moved)
    {
        update_cam(cam);
        // printf("Cam Position: %f, %f, %f\n",
        //        cam->cam_pos[0],
        //        cam->cam_pos[1],
        //        cam->cam_pos[2]);
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

    struct Program program;
    GLuint vertex_array;
    GLuint vertex_buffers[COUNT_BUFFERS];
    struct slice buffer_slices[COUNT_BUFFERS];
    bool buffer_disabled[COUNT_BUFFERS] = {0};

    load_program(&program, default_vert_file, default_frag_file);
    size_t element_count = configure_obj_example(
        &vertex_array, vertex_buffers, buffer_slices, buffer_disabled, program);

    glClearColor(.25, .25, .25, 1.0);
    // glPolygonMode(GL_FRONT, GL_LINE);
    // glPolygonMode(GL_BACK, GL_LINE);
    // glEnable(GL_CULL_FACE); // cull face
    // glCullFace(GL_BACK);    // cull back face
    // glFrontFace(GL_CCW);    // GL_CCW for counter clock-wise
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
        generate_model(&model, time);

        render_object(
            vertex_array, element_count, vertex_buffers, buffer_disabled,
            program, &proj, &cam.view, &model, time);

        glfwPollEvents();

        glfwSwapBuffers(window);

        handle_camera_events(window, elapsed_seconds, &cam);
        handle_events(window, &program);
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
