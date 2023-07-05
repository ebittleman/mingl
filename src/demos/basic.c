
#ifdef DEMOS_BASIC

#include <stdlib.h>
#include <stdio.h>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

const char *vertex_shader_text =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec4 aColor;\n"
    "layout (location = 2) in vec2 aUV;\n"
    "out vec4 VertColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   VertColor = aColor;\n"
    "}\0";
const char *fragment_shader_text =
    "#version 330 core\n"
    "in vec4 VertColor;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = VertColor;\n"
    "}\n\0";

typedef void(init_func_t)(void);
typedef void(update_func_t)(void);

GLFWwindow *init_graphics(init_func_t *init_func);
void start_drawing(GLFWwindow *window, update_func_t *update_func);

static GLuint vao;
static GLuint program;

typedef struct
{
    float x, y, z;
    float r, g, b, a;
    float u, v;
} vertex;

void init(void)
{
    vertex vertices[] = {
        {0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},   // top right
        {0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},  // bottom right
        {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f}, // bottom left
        {-0.5f, 0.5f, 0.0f, 1.0f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f}   // top left
    };

    GLuint indices[] = {
        // note that we start from 0!
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
    };

    // initialize buffers
    GLuint vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // load memory
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // compile and link shader program
    GLuint vertex_shader, fragment_shader;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // set vertex attributes
    GLint apos_location;
    apos_location = glGetAttribLocation(program, "aPos");

    glEnableVertexAttribArray(apos_location);
    glVertexAttribPointer(apos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)0);

    GLint color_location;
    color_location = glGetAttribLocation(program, "aColor");

    glEnableVertexAttribArray(color_location);
    glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)offsetof(vertex, r));

    GLint uv_location;
    uv_location = glGetAttribLocation(program, "aUV");

    glEnableVertexAttribArray(uv_location);
    glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)offsetof(vertex, u));

    // unbind buffers
    glBindVertexArray(0);                     // unbind the VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);         // unbind the VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind the EBO
}

void update(void)
{
    // set the shaders
    glUseProgram(program);
    // bind the vertex array object
    glBindVertexArray(vao);
    // draw the vao
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // unbind the vao
    glBindVertexArray(0);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void run(void)
{
    GLFWwindow *window = init_graphics(&init);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to initialize GLFW window\n");
        exit(EXIT_FAILURE);
    }

    start_drawing(window, &update);
}

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

GLFWwindow *init_graphics(init_func_t *init_func)
{
    GLFWwindow *window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to create GLFW windows\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    int glad_version = gladLoadGL(glfwGetProcAddress);
    if (glad_version == 0)
    {
        fprintf(stderr, "Failed to initialize OpenGL context\n");
        exit(EXIT_FAILURE);
    }
    printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(glad_version), GLAD_VERSION_MINOR(glad_version));

    glfwSwapInterval(1);

    init_func();

    return window;
}

void start_drawing(GLFWwindow *window, update_func_t *update_func)
{
    while (!glfwWindowShouldClose(window))
    {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        update_func();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

#endif
