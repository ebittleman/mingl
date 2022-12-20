
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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
