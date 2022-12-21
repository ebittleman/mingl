
#ifndef _MINGL_H
#define _MINGL_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "camera.h"
#include "entities.h"
#include "linmath.h"
#include "shaders.h"
#include "types.h"

typedef int(init_func_t)(GLFWwindow *window);
typedef void(update_func_t)(GLFWwindow *window, double time, double dt);

GLFWwindow *init_opengl(init_func_t *init_func);
void update_loop(GLFWwindow *window, update_func_t *update_func, int n, object *objects);

void load_program(Program *program, const char *vert_file, const char *frag_file);
void set_uniforms_and_inputs(
    GLuint id, GLint uniforms[COUNT_UNIFORMS],
    GLint input_locations[ELEMENTS]);
void buffer_object_to_vao(object *o);
void configure_vertex_attributes(
    GLuint vertex_buffers[COUNT_BUFFERS],
    GLint input_locations[ELEMENTS],
    bool buffer_disabled[COUNT_BUFFERS]);

typedef struct _vao
{
    GLuint id;
    GLenum mode;
    GLint first;
    GLsizei count;
} vao;

typedef struct _shader
{
    GLuint id;
    const char *vert_file;
    const char *frag_file;
    GLint *uniforms;
    GLint *inputs;
} shader;

typedef struct _scene
{
    vao vao;
    shader shader;
    model model;
} scene;

#endif
