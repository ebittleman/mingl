#ifndef _SHADER_UTIL_H
#define _SHADER_UTIL_H

#include <glad/gl.h>

GLuint create_shader_program_from_strings(const char *vertex_shader_str, const char *fragment_shader_str);
GLuint create_shader_program_from_files(const char *vertex_shader_filename, const char *fragment_shader_filename);
void reload_shader_program_from_files(GLuint *program, const char *vertex_shader_filename, const char *fragment_shader_filename);
unsigned int loadTexture(char const *path);

#endif
