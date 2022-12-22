
#include <stdlib.h>
#include <stdio.h>

#include "opengl.h"

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

void load_mesh_shader(Program *program, const char *vert_file, const char *frag_file)
{
    GLuint id = create_shader_program_from_files(vert_file, frag_file);
    program->id = id;
    program->vert_file = vert_file;
    program->frag_file = frag_file;

    program->uniforms[U_MVP] = glGetUniformLocation(id, "mvp_matrix");
    program->uniforms[U_MODEL] = glGetUniformLocation(id, "model_matrix");
    program->uniforms[U_VIEW] = glGetUniformLocation(id, "view_matrix");
    program->uniforms[U_PROJECTION] = glGetUniformLocation(id, "proj_matrix");
    program->uniforms[U_CAM_POS] = glGetUniformLocation(id, "cam_pos");
    program->uniforms[U_TIME] = glGetUniformLocation(id, "time");

    // buffer attributes
    for (int x = 0; x < VERTEX_PARAM_COUNT; x++)
    {
        program->input_locations[x] = glGetAttribLocation(id, vertex_param_names[x]);
    }
}

void render_mesh(Program shader, GLuint vao, mat4x4 vp, mat4x4 position, mesh mesh)
{
    mat4x4 mvp;
    mat4x4_mul(mvp, vp, position);

    glUniformMatrix4fv(shader.uniforms[U_MVP], 1, GL_FALSE, (const GLfloat *)mvp);
    glUniformMatrix4fv(shader.uniforms[U_MODEL], 1, GL_FALSE, (const GLfloat *)position);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.len);
    glBindVertexArray(0);
}

void render_model(mat4x4 vp, model model)
{
    GLuint *vaos = (GLuint *)model.vaos.data;
    mesh *meshes = (mesh *)model.meshes.data;
    for (size_t i = 0; i < model.vaos.len; i++)
    {
        // TODO: only render mesh if its on-screen
        glUseProgram(meshes[i].shader->id);
        render_mesh(*meshes[i].shader, vaos[i], vp, model.current_position, meshes[i]);
    }
}

void render(mat4x4 projection, Cam cam, double time, slice shaders, slice models)
{
    mat4x4 vp;
    mat4x4_mul(vp, projection, cam.view);

    Program *shader_data = (Program *)shaders.data;
    for (size_t i = 0; i < shaders.len; i++)
    {
        glUseProgram(shader_data[i].id);

        glUniformMatrix4fv(shader_data[i].uniforms[U_VIEW], 1, GL_FALSE, (const GLfloat *)cam.view);
        glUniformMatrix4fv(shader_data[i].uniforms[U_PROJECTION], 1, GL_FALSE, (const GLfloat *)&projection);
        glUniform3fv(shader_data[i].uniforms[U_CAM_POS], 1, cam.cam_pos);
        glUniform1f(shader_data[i].uniforms[U_TIME], (float)time);
    }

    model *models_data = (model *)models.data;
    for (size_t i = 0; i < models.len; i++)
    {
        render_model(vp, models_data[i]);
    }
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

void update_loop(GLFWwindow *window, update_func_t *update_func, slice shaders, slice models)
{
    Cam cam = new_cam();
    mat4x4 projection;
    while (!glfwWindowShouldClose(window))
    {

        static double previous_seconds = 0.0;
        double current_seconds = glfwGetTime();
        double dt = current_seconds - previous_seconds;
        previous_seconds = current_seconds;

        _update_fps_counter(window, current_seconds);
        calculate_projection_matrix(window, &projection);
        handle_camera_events(window, dt, &cam);

        update_func(window, current_seconds, dt);

        render(projection, cam, current_seconds, shaders, models);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
