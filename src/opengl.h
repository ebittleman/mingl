
#ifndef _MINGL_H
#define _MINGL_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "entities.h"
#include "shaders/shaders.h"

typedef int(init_func_t)(GLFWwindow *window);
typedef void(update_func_t)(GLFWwindow *window, double time, double dt);

GLFWwindow *init_opengl(init_func_t *init_func);

void load_mesh_shader(shader *program, const char *vert_file, const char *frag_file);
GLuint setup_mesh(mesh mesh);
void setup_model(model model);
void start(GLFWwindow *window, update_func_t *update_func, slice shaders, slice scenes);

// mat3x3
typedef vec3 mat3x3[3];

LINMATH_H_FUNC void mat3x3_from_4x4(mat3x3 R, mat4x4 const M)
{
    R[0][0] = M[0][0];
    R[0][1] = M[0][1];
    R[0][2] = M[0][2];

    R[1][0] = M[1][0];
    R[1][1] = M[1][1];
    R[1][2] = M[1][2];

    R[2][0] = M[2][0];
    R[2][1] = M[2][1];
    R[2][2] = M[2][2];
}

#endif
