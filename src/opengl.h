
#ifndef _MINGL_H
#define _MINGL_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "camera.h"
#include "entities.h"
#include "linmath.h"
#include "shader_util.h"
#include "shaders/shaders.h"
#include "types.h"

typedef int(init_func_t)(GLFWwindow *window);
typedef void(update_func_t)(GLFWwindow *window, double time, double dt);

GLFWwindow *init_opengl(init_func_t *init_func);

void load_mesh_shader(shader *program, const char *vert_file, const char *frag_file);
GLuint setup_mesh(mesh mesh);
void setup_model(model model);
void start(GLFWwindow *window, update_func_t *update_func, slice shaders, slice scenes);

#endif
