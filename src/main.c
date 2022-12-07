
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
    GLint proj_location;
    GLint view_location;
    GLint model_location;
    GLint time_location;
};

const char *default_vert_file = "src/shaders/default/default.vert";
const char *default_frag_file = "src/shaders/default/default.frag";
// const char *default_obj_file = "assets/star1.obj";
// const char *default_obj_file = "assets/star.obj";
const char *default_obj_file = "assets/arcade_obj.obj";
// const char *default_obj_file = "assets/Triceratops_base_mesh.obj";

struct uniforms configure_obj_example()
{
    struct slice vertices = {0, 0, sizeof(struct Vector3), NULL};
    line_callback_t *vertices_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&append_vertex, &vertices);

    struct slice faces = new_slice(sizeof(int));
    line_callback_t *faces_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&read_face_line, &faces);

    struct slice normals_slice = new_slice(sizeof(float));
    line_callback_t *normals_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&handle_normal_line, &normals_slice);

    struct slice uv_slice = new_slice(sizeof(float));
    line_callback_t *uv_callback = (line_callback_t *)alloc_callback(
        (callback_function_t)&handle_uv_line, &uv_slice);

    line_reader(default_obj_file,
                vertices_callback,
                faces_callback,
                normals_callback,
                uv_callback);

    free_callback((callback_t)vertices_callback);
    free_callback((callback_t)faces_callback);
    free_callback((callback_t)normals_callback);
    free_callback((callback_t)uv_callback);

    struct Vector3 *verts = (struct Vector3 *)vertices.data;
    size_t vert_size = vertices.len * vertices.size;

    float *normals = (float *)normals_slice.data;
    size_t normals_size = normals_slice.len * normals_slice.size;

    float *uv = (float *)uv_slice.data;
    size_t uv_size = uv_slice.len * uv_slice.size;

    struct slice elements_slice = faces_to_elements(faces);
    int *elements = (int *)elements_slice.data;
    size_t element_size = elements_slice.len * elements_slice.size;

    // printf("Found %d vertices\n", vertices.len);
    // for (size_t i = 0; i < vertices.len; i++)
    // {
    //     struct Vector3 vec = verts[i];
    //     printf("x: %f, y: %f, z: %f\n", vec.x, vec.y, vec.z);
    // }

    // printf("Found %d elements\n", elements_slice.len);
    // for (size_t i = 0; i < elements_slice.len; i += 3)
    // {
    //     printf("x: %d, y: %d, z: %d\n",
    //            elements[i],
    //            elements[i + 1],
    //            elements[i + 2]);
    // }

    if (faces.cap > 0)
    {
        free(faces.data);
    }

    GLuint vertex_array, vertex_buffer, normal_buffer, uv_buffer, ebo; //, color_buffer, uv_buffer;
    GLint mvp_location, proj_location, view_location, model_location,
        time_location, vpos_location, vnorm_location, uv_location; //, vcol_location, vuv_location;

    GLuint program = create_shader_program_from_files(default_vert_file, default_frag_file);
    mvp_location = glGetUniformLocation(program, "mvp_matrix");
    proj_location = glGetUniformLocation(program, "proj_matrix");
    view_location = glGetUniformLocation(program, "view_matrix");
    model_location = glGetUniformLocation(program, "model_matrix");
    time_location = glGetUniformLocation(program, "time");
    vpos_location = glGetAttribLocation(program, "position");
    vnorm_location = glGetAttribLocation(program, "normal");
    uv_location = glGetAttribLocation(program, "uv");

    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vert_size, verts, GL_STATIC_DRAW);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          vertices.size, (void *)0);
    glEnableVertexAttribArray(vpos_location);

    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, normals_size, normals, GL_STATIC_DRAW);
    glVertexAttribPointer(vnorm_location, 3, GL_FLOAT, GL_FALSE,
                          normals_slice.size * 3, (void *)0);
    glEnableVertexAttribArray(vnorm_location);

    glGenBuffers(1, &uv_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, uv_size, uv, GL_STATIC_DRAW);
    glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE,
                          uv_slice.size * 3, (void *)0);
    glEnableVertexAttribArray(uv_location);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 element_size, elements_slice.data, GL_STATIC_DRAW);

    return (struct uniforms){
        vertex_array, program, default_vert_file, default_frag_file,
        0, elements_slice.len,
        mvp_location, proj_location, view_location, model_location, time_location};
}

struct uniforms configure_learning_example()
{
    GLuint vertex_array, vertex_buffer, color_buffer, uv_buffer, ebo;
    GLint mvp_location, proj_location, view_location, model_location, time_location, vpos_location, vcol_location, vuv_location;

    GLuint program = create_shader_program_from_files(vert_file, frag_file);
    // pull variable location
    mvp_location = glGetUniformLocation(program, "mvp_matrix");
    proj_location = glGetUniformLocation(program, "proj_matrix");
    view_location = glGetUniformLocation(program, "view_matrix");
    model_location = glGetUniformLocation(program, "model_matrix");
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

    int num_elements = sizeof(elements) / sizeof(elements[0]);
    return (struct uniforms){
        vertex_array, program, vert_file, frag_file, 0, num_elements,
        mvp_location, proj_location, view_location, model_location, time_location};
}
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

void render_object(
    mat4x4 *proj,
    mat4x4 *view,
    mat4x4 *model,
    float time,
    struct uniforms *_uniforms)
{

    mat4x4 mvp;
    mat4x4_mul(mvp, *proj, *view);
    mat4x4_mul(mvp, mvp, *model);

    glUseProgram(_uniforms->program);
    glUniformMatrix4fv(_uniforms->mvp_location, 1, GL_FALSE, (const GLfloat *)&mvp);
    glUniformMatrix4fv(_uniforms->proj_location, 1, GL_FALSE, (const GLfloat *)proj);
    glUniformMatrix4fv(_uniforms->view_location, 1, GL_FALSE, (const GLfloat *)view);
    glUniformMatrix4fv(_uniforms->model_location, 1, GL_FALSE, (const GLfloat *)model);
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
    GLFWwindow *window, double elapsed_seconds, struct Cam *cam,
    struct uniforms *_uniforms)
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

    // struct uniforms uniforms = configure_learning_example();
    struct uniforms uniforms = configure_obj_example();
    // struct uniforms uniforms = configure_simple_example();

    glClearColor(.25, .25, .25, 1.0);
    // glPolygonMode(GL_FRONT, GL_LINE);
    // glPolygonMode(GL_BACK, GL_LINE);
    // glEnable(GL_CULL_FACE); // cull face
    // glCullFace(GL_BACK);    // cull back face
    // glFrontFace(GL_CW);     // GL_CCW for counter clock-wise
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

        render_object(&proj, &cam.view, &model, time, &uniforms);

        glfwPollEvents();

        handle_camera_events(window, elapsed_seconds, &cam, &uniforms);
        glfwSwapBuffers(window);
        handle_events(window, &uniforms);
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
