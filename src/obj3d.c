#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "obj3d.h"

#define LINE_BUF_SIZE 1024 * 64
static char line_buffer[LINE_BUF_SIZE];

char *trimString(char *str)
{
    char *end;

    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    end[1] = '\0';

    return str;
}

int line_reader(
    const char *filename,
    line_callback_t *callbacks[COUNT_BUFFERS],
    slice *buffers[COUNT_BUFFERS])
{
    FILE *f = NULL;
    int result = -1;

    f = fopen(filename, "r");
    if (f == NULL)
        goto fail;

    while (fgets(line_buffer, LINE_BUF_SIZE, f) != NULL)
    {
        if (ferror(f))
            goto fail;

        char *trimmed_line = trimString(line_buffer);

        for (size_t i = 0; i < COUNT_BUFFERS; i++)
        {
            callbacks[i](buffers[i], trimmed_line);
        }
    }

    if (f)
    {
        fclose(f);
        errno = 0;
    }

    return 0;
fail:
    if (f)
    {
        int saved_errno = errno;
        fclose(f);
        errno = saved_errno;
    }
    return result;
}

void parse_vec3_line(slice *vec3_data, char *line)
{
    vec3 position;
    sscanf(line, "%f %f %f", &position[0], &position[1], &position[2]);
    extend_slice(vec3_data, 3, position);
}

void parse_vec2_line(slice *vec2_data, char *line)
{
    vec2 uv;
    sscanf(line, "%f %f", &uv[0], &uv[1]);
    extend_slice(vec2_data, 2, uv);
}

void handle_noop(slice *any_data, char *line)
{
    return;
}

void handle_vertex_line(slice *vertex_data, char *line)
{

    char *pre = "v ";
    if (strncmp(pre, line, strlen(pre)) != 0)
    {
        return;
    }
    parse_vec3_line(vertex_data, line + 2);
}

void handle_normal_line(slice *normal_data, char *line)
{

    char *pre = "vn ";
    if (strncmp(pre, line, strlen(pre)) != 0)
    {
        return;
    }
    parse_vec3_line(normal_data, line + 3);
}

void handle_uv_line(slice *uv_data, char *line)
{

    char *pre = "vt ";
    if (strncmp(pre, line, strlen(pre)) != 0)
    {
        return;
    }
    parse_vec2_line(uv_data, line + 2);
}

void sscanf_face(char *line, size_t len, size_t vertex_count, face_elements data[])
{
    char string_buffer[256];
    char *cursor = line, *end = NULL;
    for (size_t i = 0; i < vertex_count; i++)
    {
        char *end = strchr(cursor, ' ');
        if (end == NULL)
        {
            end = line + len;
        }

        memcpy(string_buffer, cursor, end - cursor);
        string_buffer[end - cursor] = '\0';

        if (sscanf(string_buffer, "%llu/%llu/%llu",
                   &data[i].position_idx, &data[i].texture_idx, &data[i].normal_idx) != 3)
        {
            data[i].normal_idx = 0;
            if (sscanf(string_buffer, "%llu/%llu/", &data[i].position_idx, &data[i].texture_idx) != 2)
            {
                data[i].texture_idx = 0;
                if (sscanf(string_buffer, "%llu//%llu", &data[i].position_idx, &data[i].normal_idx) != 2)
                {
                    sscanf(string_buffer, "%llu//", &data[i].position_idx);
                }
            }
        }

        cursor = end + 1;
    }
}

void faces_to_elements(slice *elements, size_t n, face_elements face_data[])
{
    static const size_t face_elements_size = sizeof(face_elements) / sizeof(size_t);

    for (int x = 0; x < n; x++)
    {
        face_data[x].position_idx = face_data[x].position_idx - 1;
        face_data[x].texture_idx = face_data[x].texture_idx - 1;
        face_data[x].normal_idx = face_data[x].normal_idx - 1;
    }

    extend_slice(elements, face_elements_size, &face_data[0]);
    extend_slice(elements, face_elements_size, &face_data[1]);
    extend_slice(elements, face_elements_size, &face_data[2]);

    int iters = n - 2;
    for (int x = 1; x < iters; x++)
    {
        int offset = x + 1;

        extend_slice(elements, face_elements_size, &face_data[0]);
        extend_slice(elements, face_elements_size, &face_data[offset]);
        extend_slice(elements, face_elements_size, &face_data[offset + 1]);
    }

    return;
}

void parse_face_line(slice *element_data, char *line)
{
    int len = strlen(line);
    int vertex_count = 0;

    for (size_t i = 0; i < len; i++)
    {
        vertex_count += line[i] == ' ';
    }
    vertex_count += 1;

    face_elements data[vertex_count];
    sscanf_face(line, len, vertex_count, data);
    faces_to_elements(element_data, vertex_count, data);
}

void handle_face_line(slice *element_data, char *line)
{

    char *pre = "f ";
    if (strncmp(pre, line, strlen(pre)) != 0)
    {
        return;
    }
    parse_face_line(element_data, line + 2);
}

void calculate_normal_per_vertex(slice buffer_slices[COUNT_BUFFERS])
{

    reset_slice(&buffer_slices[NORMALS],
                buffer_slices[VERTS].size,
                buffer_slices[VERTS].len,
                buffer_slices[VERTS].cap);

    face *faces = (face *)buffer_slices[ELEMENTS].data;
    size_t num_faces = buffer_slices[ELEMENTS].len / 9;
    vec3 *vertices = (vec3 *)buffer_slices[VERTS].data;
    vec3 *normals = (vec3 *)buffer_slices[NORMALS].data;

    vec3 normal, a, b;
    for (int i = 0; i < num_faces; i++)
    {
        face_elements *current_face = (face_elements *)faces[i];

        float *p1 = vertices[current_face[0].position_idx];
        float *p2 = vertices[current_face[1].position_idx];
        float *p3 = vertices[current_face[2].position_idx];

        vec3_sub(a, p2, p1);
        vec3_sub(b, p3, p1);
        vec3_mul_cross(normal, a, b);

        float *n1 = normals[current_face[0].position_idx];
        float *n2 = normals[current_face[1].position_idx];
        float *n3 = normals[current_face[2].position_idx];

        vec3_add(n1, n1, normal);
        vec3_add(n2, n2, normal);
        vec3_add(n3, n3, normal);

        current_face[0].normal_idx = current_face[0].position_idx;
        current_face[1].normal_idx = current_face[1].position_idx;
        current_face[2].normal_idx = current_face[2].position_idx;
    }

    for (int x = 0; x < buffer_slices[NORMALS].len / 3; x++)
    {
        vec3_norm(normals[x], normals[x]);
    }
    return;
}

void load_obj_file(const char *obj_file_name, slice buffers[COUNT_BUFFERS])
{
    line_callback_t *callbacks[COUNT_BUFFERS];

    callbacks[VERTS] = &handle_vertex_line;
    callbacks[UVS] = &handle_uv_line;
    callbacks[NORMALS] = &handle_normal_line;
    callbacks[ELEMENTS] = &handle_face_line;

    slice v = new_slice(sizeof(float)), u = new_slice(sizeof(float)),
          n = new_slice(sizeof(float)), e = new_slice(sizeof(size_t));

    slice *buff_ptr[COUNT_BUFFERS];
    buff_ptr[VERTS] = &v;
    buff_ptr[UVS] = &u;
    buff_ptr[NORMALS] = &n;
    buff_ptr[ELEMENTS] = &e;

    line_reader(obj_file_name, callbacks, buff_ptr);

    // set output
    buffers[VERTS] = *buff_ptr[VERTS];
    buffers[UVS] = *buff_ptr[UVS];
    buffers[NORMALS] = *buff_ptr[NORMALS];
    buffers[ELEMENTS] = *buff_ptr[ELEMENTS];

    if (buffers[NORMALS].len < 1)
    {
        calculate_normal_per_vertex(buffers);
    }
}

void free_buffers(slice buffers[COUNT_BUFFERS])
{
    for (size_t i = 0; i < COUNT_BUFFERS; i++)
    {
        free(buffers[i].data);
    }
}

static inline void bounding_box(float bounds[6], int n, vec3 *vertices)
{
    vec3 *min_max = (vec3 *)bounds;

    min_max[0][0] = vertices[0][0];
    min_max[0][1] = vertices[0][1];
    min_max[0][2] = vertices[0][2];
    min_max[1][0] = vertices[0][0];
    min_max[1][1] = vertices[0][1];
    min_max[1][2] = vertices[0][2];

    for (size_t i = 1; i < n; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (vertices[i][j] < min_max[0][j])
            {
                min_max[0][j] = vertices[i][j];
            }
            if (vertices[i][j] > min_max[1][j])
            {
                min_max[1][j] = vertices[i][j];
            }
        }
    }
}

void load_mesh(const char *obj_file_name, mesh *mesh)
{
    slice buffers[COUNT_BUFFERS];
    load_obj_file(obj_file_name, buffers);

    face *faces = (face *)buffers[ELEMENTS].data;
    vec3 *vertices_index = (vec3 *)buffers[VERTS].data;
    vec3 *normal_index = (vec3 *)buffers[NORMALS].data;
    vec2 *uv_index = (vec2 *)buffers[UVS].data;

    // TODO: find somewhere else for measuring bounding box
    int num_verts = buffers[VERTS].len / 3;
    bounding_box(mesh->bounds, num_verts, vertices_index);

    vec3 to_local_origin = {
        ((mesh->bounds[3] - mesh->bounds[0]) / 2) - mesh->bounds[3],
        ((mesh->bounds[4] - mesh->bounds[1]) / 2) - mesh->bounds[4],
        ((mesh->bounds[5] - mesh->bounds[2]) / 2) - mesh->bounds[5],
    };

    for (size_t i = 0; i < buffers[VERTS].len / 3; i++)
    {
        vec3_add(vertices_index[i], vertices_index[i], to_local_origin);
    }
    vec3_add(mesh->bounds, mesh->bounds, to_local_origin);
    vec3_add(&mesh->bounds[3], &mesh->bounds[3], to_local_origin);

    vertex vert = {0};
    size_t position_size = sizeof(vert.position);
    size_t normal_size = sizeof(vert.normal);
    size_t uv_size = sizeof(vert.uv);

    size_t num_faces = buffers[ELEMENTS].len / 9;
    size_t triangles_cap = sizeof(triangle) * num_faces;
    triangle *triangles = malloc(triangles_cap);
    memset(triangles, 0, triangles_cap);

    for (int i = 0; i < num_faces; i++)
    {
        face_elements *current_face = (face_elements *)faces[i];
        vertex *current_triangle = (vertex *)triangles[i];

        size_t v1_idx = current_face[0].position_idx,
               v2_idx = current_face[1].position_idx,
               v3_idx = current_face[2].position_idx;

        vertex *vert1 = (vertex *)&current_triangle[0],
               *vert2 = (vertex *)&current_triangle[1],
               *vert3 = (vertex *)&current_triangle[2];

        memcpy(vert1->position, vertices_index[v1_idx], position_size);
        memcpy(vert2->position, vertices_index[v2_idx], position_size);
        memcpy(vert3->position, vertices_index[v3_idx], position_size);
    }

    if (buffers[NORMALS].len)
    {
        for (int i = 0; i < num_faces; i++)
        {
            face_elements *current_face = (face_elements *)faces[i];
            vertex *current_triangle = (vertex *)triangles[i];

            size_t n1_idx = current_face[0].normal_idx,
                   n2_idx = current_face[1].normal_idx,
                   n3_idx = current_face[2].normal_idx;

            vertex *vert1 = (vertex *)&current_triangle[0],
                   *vert2 = (vertex *)&current_triangle[1],
                   *vert3 = (vertex *)&current_triangle[2];

            memcpy(vert1->normal, normal_index[n1_idx], normal_size);
            memcpy(vert2->normal, normal_index[n2_idx], normal_size);
            memcpy(vert3->normal, normal_index[n3_idx], normal_size);
        }
    }

    if (buffers[UVS].len)
    {
        for (int i = 0; i < num_faces; i++)
        {
            face_elements *current_face = (face_elements *)faces[i];
            vertex *current_triangle = (vertex *)triangles[i];

            size_t text1_idx = current_face[0].texture_idx,
                   text2_idx = current_face[1].texture_idx,
                   text3_idx = current_face[2].texture_idx;

            vertex *vert1 = (vertex *)&current_triangle[0],
                   *vert2 = (vertex *)&current_triangle[1],
                   *vert3 = (vertex *)&current_triangle[2];

            memcpy(vert1->uv, uv_index[text1_idx], uv_size);
            memcpy(vert2->uv, uv_index[text2_idx], uv_size);
            memcpy(vert3->uv, uv_index[text3_idx], uv_size);
        }
    }

    mesh->vertices = (slice){num_faces * 3, triangles_cap, sizeof(vertex), triangles};

    free_buffers(buffers);
}
