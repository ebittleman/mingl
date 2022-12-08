#include <glad/gl.h>

// #include "linmath.h"
// #include "obj3d.h"

// typedef struct _colors
// {
//     float r, g, b;
// } color;

// typedef struct _uv
// {
//     float u, v;
// } uv;

// static const vec3 vertices[] = {

//     {-0.6f, -0.4f, 0.f},
//     {0.6f, -0.4f, 0.f},
//     {0.6f, 0.6f, 0.f},
//     {-0.6f, 0.6f, 0.f},
//     {-0.8f, 0.6f, 0.f},
//     {-0.83f, 0.0f, 0.f}};

// static const vec3 normals[] = {
//     {0.0f, 0.0f, 1.0f},
//     {0.0f, 0.0f, 1.0f},
//     {0.0f, 0.0f, 1.0f},
//     {0.0f, 0.0f, 1.0f},
//     {0.0f, 0.0f, 1.0f},
//     {0.0f, 0.0f, 1.0f}};

// GLuint elements[] = {
//     0, 1, 2,
//     2, 3, 0,
//     3, 4, 0,
//     4, 5, 0};

// static const color colors[] = {
//     {1.f, 0.f, 0.f},
//     {0.f, 1.f, 0.f},
//     {0.f, 0.f, 1.f},
//     {0.f, 0.f, 1.f},
//     {1.f, 1.f, 1.f},
//     {0.f, 0.f, 0.f},
// };

// static const uv uvs[] = {
//     {0.f, 0.f},
//     {1.f, 0.f},
//     {1.f, 1.f},
//     {0.f, 1.f},
//     {-0.25f, 1.f},
//     {-0.28f, 0.5f},
// };

// // struct uniforms configure_learning_example()
// // {
// //     GLuint vertex_array, vertex_buffer, color_buffer, uv_buffer, ebo;
// //     GLint mvp_location, proj_location, view_location, model_location, time_location, vpos_location, vcol_location, vuv_location;

// //     GLuint program = create_shader_program_from_files(vert_file, frag_file);
// //     // pull variable location
// //     mvp_location = glGetUniformLocation(program, "mvp_matrix");
// //     proj_location = glGetUniformLocation(program, "proj_matrix");
// //     view_location = glGetUniformLocation(program, "view_matrix");
// //     model_location = glGetUniformLocation(program, "model_matrix");
// //     time_location = glGetUniformLocation(program, "time");
// //     vpos_location = glGetAttribLocation(program, "vPos");
// //     vcol_location = glGetAttribLocation(program, "vCol");
// //     vuv_location = glGetAttribLocation(program, "vUV");

// //     glGenVertexArrays(1, &vertex_array);
// //     glBindVertexArray(vertex_array);

// //     glGenBuffers(1, &vertex_buffer);
// //     glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
// //     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// //     glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
// //                           sizeof(vertices[0]), (void *)0);

// //     glGenBuffers(1, &color_buffer);
// //     glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
// //     glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
// //     glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
// //                           sizeof(colors[0]), (void *)0);

// //     glGenBuffers(1, &uv_buffer);
// //     glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
// //     glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
// //     glVertexAttribPointer(vuv_location, 2, GL_FLOAT, GL_FALSE,
// //                           sizeof(uvs[0]), (void *)0);

// //     glGenBuffers(1, &ebo);
// //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
// //     glBufferData(GL_ELEMENT_ARRAY_BUFFER,
// //                  sizeof(elements), elements, GL_STATIC_DRAW);

// //     // enable attributes
// //     glEnableVertexAttribArray(vpos_location);
// //     glEnableVertexAttribArray(vcol_location);
// //     glEnableVertexAttribArray(vuv_location);

// //     int num_elements = sizeof(elements) / sizeof(elements[0]);
// //     return (struct uniforms){
// //         vertex_array, program, vert_file, frag_file, 0, num_elements,
// //         mvp_location, proj_location, view_location, model_location, time_location};
// // }
