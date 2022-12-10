#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "shaders.h"

// set a max limit on shader length to avoid dynamic memory allocation
#define MAX_SHADER_SZ 100000

// function creates a shader program from a vertex and fragment shader, stored in provided strings
// vertex_shader_str - a null-terminated string of text containing a vertex shader
// fragment_shader_str - a null-terminated string of text containing a fragment shader
// returns a new, valid shader program handle, or 0 if there was a problem finding/loading/compiling/linking
// asserts on NULL parameters
GLuint create_shader_program_from_strings(const char *vertex_shader_str, const char *fragment_shader_str)
{
    assert(vertex_shader_str && fragment_shader_str);

    GLuint shader_program = glCreateProgram();
    GLuint vertex_shader_handle = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_handle = glCreateShader(GL_FRAGMENT_SHADER);
    { // compile shader and check for errors
        glShaderSource(vertex_shader_handle, 1, &vertex_shader_str, NULL);
        glCompileShader(vertex_shader_handle);
        int params = -1;
        glGetShaderiv(vertex_shader_handle, GL_COMPILE_STATUS, &params);
        if (GL_TRUE != params)
        {
            fprintf(stderr, "ERROR: vertex shader index %u did not compile\n", vertex_shader_handle);
            const int max_length = 2048;
            int actual_length = 0;
            char slog[2048];
            glGetShaderInfoLog(vertex_shader_handle, max_length, &actual_length, slog);
            fprintf(stderr, "shader info log for GL index %u:\n%s\n", vertex_shader_handle, slog);

            glDeleteShader(vertex_shader_handle);
            glDeleteShader(fragment_shader_handle);
            glDeleteProgram(shader_program);
            return 0;
        }
    }

    { // compile shader and check for errors
        glShaderSource(fragment_shader_handle, 1, &fragment_shader_str, NULL);
        glCompileShader(fragment_shader_handle);
        int params = -1;
        glGetShaderiv(fragment_shader_handle, GL_COMPILE_STATUS, &params);
        if (GL_TRUE != params)
        {
            fprintf(stderr, "ERROR: fragment shader index %u did not compile\n", fragment_shader_handle);
            const int max_length = 2048;
            int actual_length = 0;
            char slog[2048];
            glGetShaderInfoLog(fragment_shader_handle, max_length, &actual_length, slog);
            fprintf(stderr, "shader info log for GL index %u:\n%s\n", fragment_shader_handle, slog);

            glDeleteShader(vertex_shader_handle);
            glDeleteShader(fragment_shader_handle);
            glDeleteProgram(shader_program);
            return 0;
        }
    }

    glAttachShader(shader_program, fragment_shader_handle);
    glAttachShader(shader_program, vertex_shader_handle);
    { // link program and check for errors
        glLinkProgram(shader_program);
        glDeleteShader(vertex_shader_handle);
        glDeleteShader(fragment_shader_handle);
        int params = -1;
        glGetProgramiv(shader_program, GL_LINK_STATUS, &params);
        if (GL_TRUE != params)
        {
            fprintf(stderr, "ERROR: could not link shader program GL index %u\n", shader_program);
            const int max_length = 2048;
            int actual_length = 0;
            char plog[2048];
            glGetProgramInfoLog(shader_program, max_length, &actual_length, plog);
            fprintf(stderr, "program info log for GL index %u:\n%s", shader_program, plog);

            glDeleteProgram(shader_program);
            return 0;
        }
    }

    return shader_program;
}

GLuint create_shader_program_from_files(const char *vertex_shader_filename, const char *fragment_shader_filename)
{
    assert(vertex_shader_filename && fragment_shader_filename);

    printf("loading shader from files `%s` and `%s`\n", vertex_shader_filename, fragment_shader_filename);

    char vs_shader_str[MAX_SHADER_SZ];
    char fs_shader_str[MAX_SHADER_SZ];
    vs_shader_str[0] = fs_shader_str[0] = '\0';
    { // read vertex shader file into a buffer
        FILE *fp = fopen(vertex_shader_filename, "r");
        if (!fp)
        {
            fprintf(stderr, "ERROR: could not open vertex shader file `%s`\n", vertex_shader_filename);
            return 0;
        }
        size_t count = fread(vs_shader_str, 1, MAX_SHADER_SZ - 1, fp);
        assert(count < MAX_SHADER_SZ - 1); // file was too long
        vs_shader_str[count] = '\0';
        fclose(fp);
    }
    { // read fragment shader file into a buffer
        FILE *fp = fopen(fragment_shader_filename, "r");
        if (!fp)
        {
            fprintf(stderr, "ERROR: could not open fragment shader file `%s`\n", fragment_shader_filename);
            return 0;
        }
        size_t count = fread(fs_shader_str, 1, MAX_SHADER_SZ - 1, fp);
        assert(count < MAX_SHADER_SZ - 1); // file was too long
        fs_shader_str[count] = '\0';
        fclose(fp);
    }

    return create_shader_program_from_strings(vs_shader_str, fs_shader_str);
}

void reload_shader_program_from_files(GLuint *program, const char *vertex_shader_filename, const char *fragment_shader_filename)
{
    assert(program && vertex_shader_filename && fragment_shader_filename);

    GLuint reloaded_program = create_shader_program_from_files(vertex_shader_filename, fragment_shader_filename);

    if (reloaded_program)
    {
        glDeleteProgram(*program);
        *program = reloaded_program;
    }
}
