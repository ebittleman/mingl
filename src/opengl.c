#include "opengl.h"

static camera cam;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void calculate_projection_matrix(GLFWwindow *window, mat4x4 *p, float degrees)
{
    float ratio;
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;

    // mat4x4_ortho(*p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_perspective(*p, radians(degrees), ratio, 0.1f, 1000.0f);
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

void process_keyboard(GLFWwindow *window, float dt)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_process_keyboard(&cam, FORWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_process_keyboard(&cam, BACKWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_process_keyboard(&cam, LEFT, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_process_keyboard(&cam, RIGHT, dt);
}

static void mouse_callback(GLFWwindow *window, double x_pos, double y_pos)
{
    static double lastX, lastY;
    static bool firstMouse = true;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    float yaw = x_pos / (float)width;
    float pitch = y_pos / (float)height;

    yaw *= 720;
    yaw -= 360;
    pitch *= 178;
    pitch -= 89;

    // TODO: figure out how to make this smooth...probably some interpolation
    // process_mouse_movement(&cam, yaw, -pitch, true);

    if (firstMouse)
    {
        lastX = x_pos;
        lastY = y_pos;
        firstMouse = false;
    }

    float x_offset = x_pos - lastX;
    float y_offset = lastY - y_pos;
    lastX = x_pos;
    lastY = y_pos;
    process_mouse_movement_by_offset(&cam, x_offset, y_offset, true);
}

static void scroll_callback(GLFWwindow *window, double x_offset, double y_offset)
{
    process_mouse_scroll(&cam, y_offset);
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

GLuint setup_mesh(mesh mesh)
{
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 mesh.vertices.len * mesh.vertices.size,
                 mesh.vertices.data, GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(VERTEX_POSITION);
    glVertexAttribPointer(VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, mesh.vertices.size, (void *)0);

    // vertex normals
    glEnableVertexAttribArray(VERTEX_NORMAL);
    glVertexAttribPointer(VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, mesh.vertices.size, (void *)offsetof(vertex, normal));

    // vertex texture coords
    glEnableVertexAttribArray(VERTEX_UV);
    glVertexAttribPointer(VERTEX_UV, 2, GL_FLOAT, GL_FALSE, mesh.vertices.size, (void *)offsetof(vertex, uv));

    // vertex tangent
    glEnableVertexAttribArray(VERTEX_TANGENT);
    glVertexAttribPointer(VERTEX_TANGENT, 3, GL_FLOAT, GL_FALSE, mesh.vertices.size, (void *)offsetof(vertex, tangent));

    // vertex bitangent
    glEnableVertexAttribArray(VERTEX_BITANGENT);
    glVertexAttribPointer(VERTEX_BITANGENT, 3, GL_FLOAT, GL_FALSE, mesh.vertices.size, (void *)offsetof(vertex, bitangent));

    // ids
    glEnableVertexAttribArray(VERTEX_BONE_IDS);
    glVertexAttribIPointer(VERTEX_BONE_IDS, 4, GL_INT, mesh.vertices.size, (void *)offsetof(vertex, bones));

    // weights
    glEnableVertexAttribArray(VERTEX_BONE_WEIGHTS);
    glVertexAttribPointer(VERTEX_BONE_WEIGHTS, 4, GL_FLOAT, GL_FALSE, mesh.vertices.size, (void *)offsetof(vertex, bone_weight));

    glBindVertexArray(0);
    return vao;
}

void setup_model(model model)
{
    mesh **meshes_data = (mesh **)model.meshes.data;
    for (size_t i = 0; i < model.meshes.len; i++)
    {
        // TODO: only render mesh if its on-screen
        mesh *current_mesh = meshes_data[i];
        current_mesh->vao = setup_mesh(*current_mesh);
    }
}

void render_mesh(shader current_shader, mesh mesh)
{
    glBindVertexArray(mesh.vao);
    glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.len);
    glBindVertexArray(0);
}

void render_model(
    mat4x4 mvp,
    mat3x3 normal_matrix,
    scene parent,
    shader current_shader, model model, void *parameters)
{

    if (model.material != NULL && model.material->shader != NULL)
    {
        current_shader = *model.material->shader;
    }

    glUseProgram(current_shader.id);
    if (current_shader.draw != NULL)
    {
        current_shader.draw(current_shader);
    }

    glUniformMatrix4fv(current_shader.uniforms[U_MVP], 1, GL_FALSE, (const GLfloat *)mvp);
    glUniformMatrix4fv(current_shader.uniforms[U_MODEL], 1, GL_FALSE, (const GLfloat *)parent.current_position);
    glUniformMatrix3fv(current_shader.uniforms[U_NORMAL_MATRIX], 1, GL_FALSE, (const GLfloat *)normal_matrix);

    if (model.material != NULL && model.material->draw != NULL)
    {
        model.material->draw(*model.material, current_shader);
    }

    mesh **meshes_data = (mesh **)model.meshes.data;
    for (size_t i = 0; i < model.meshes.len; i++)
    {
        // TODO: only render mesh if its on-screen
        mesh *current_mesh = meshes_data[i];
        render_mesh(current_shader, *current_mesh);
    }
}

void render_scene(shader current_shader, mat4x4 vp, scene scene)
{
    model **models_data = (model **)scene.models.data;

    mat4x4 mvp;
    mat4x4_mul(mvp, vp, scene.current_position);

    mat3x3 normal_matrix;
    mat4x4 inverse_model;
    mat4x4 transpose_model;
    mat4x4_invert(inverse_model, scene.current_position);
    mat4x4_invert(transpose_model, inverse_model);
    mat3x3_from_4x4(normal_matrix, transpose_model);

    for (size_t i = 0; i < scene.models.len; i++)
    {
        model *current_model = models_data[i];
        render_model(mvp, normal_matrix, scene, current_shader, *current_model, scene.parameters);
    }
}

void render(mat4x4 projection, camera cam, double time, slice shaders, slice scenes)
{
    mat4x4 vp;
    mat4x4 view;
    get_view_matrix(view, cam);
    mat4x4_mul(vp, projection, view);

    shader *shader_data = (shader *)shaders.data;
    for (size_t i = 0; i < shaders.len; i++)
    {
        glUseProgram(shader_data[i].id);

        glUniformMatrix4fv(shader_data[i].uniforms[U_VIEW], 1, GL_FALSE, (const GLfloat *)view);
        glUniformMatrix4fv(shader_data[i].uniforms[U_PROJECTION], 1, GL_FALSE, (const GLfloat *)projection);
        glUniform3fv(shader_data[i].uniforms[U_VIEW_POSITION], 1, cam.position);
        glUniform1f(shader_data[i].uniforms[U_TIME], (float)time);
    }

    shader root_shader = shader_data[0];
    scene *scene_data = (scene *)scenes.data;
    for (size_t i = 0; i < scenes.len; i++)
    {
        render_scene(root_shader, vp, scene_data[i]);
    }
}

GLFWwindow *init_opengl(init_func_t *init_func)
{
    GLFWwindow *window;
    init_camera(&cam);

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

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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

void start(GLFWwindow *window, update_func_t *update_func, slice shaders, slice scenes)
{

    mat4x4 projection;
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static double previous_seconds = 0.0;
        double current_seconds = glfwGetTime();
        double dt = current_seconds - previous_seconds;
        previous_seconds = current_seconds;

        _update_fps_counter(window, current_seconds);
        process_keyboard(window, dt);

        calculate_projection_matrix(window, &projection, cam.zoom);
        update_func(window, current_seconds, dt);

        render(projection, cam, current_seconds, shaders, scenes);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// requires `glDrawArrays`
// void calculate_normals_per_face(slice buffer_slices[COUNT_BUFFERS])
// {

//     reset_slice(&buffer_slices[NORMALS],
//                 buffer_slices[VERTS].size,
//                 buffer_slices[VERTS].len,
//                 buffer_slices[VERTS].cap);

//     vec3 *vertices = (vec3 *)buffer_slices[VERTS].data;
//     vec3 *normals = (vec3 *)buffer_slices[NORMALS].data;

//     vec3 normal, a, b;
//     for (int x = 0; x < buffer_slices[VERTS].len; x += 3)
//     {
//         float *p1 = vertices[x];
//         float *p2 = vertices[x + 1];
//         float *p3 = vertices[x + 2];

//         vec3_sub(a, p2, p1);
//         vec3_sub(b, p3, p1);
//         vec3_mul_cross(normal, a, b);

//         float *n1 = normals[x];
//         float *n2 = normals[x + 1];
//         float *n3 = normals[x + 2];

//         vec3_norm(normal, normal);

//         vec3_dup(n1, normal);
//         vec3_dup(n2, normal);
//         vec3_dup(n3, normal);
//     }

//     return;
// }
