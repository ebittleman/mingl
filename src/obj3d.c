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
    float x, y, z;
    sscanf(line, "%f %f %f", &x, &y, &z);
    append_slice(vec3_data, &x);
    append_slice(vec3_data, &y);
    append_slice(vec3_data, &z);
}

void parse_vec2_line(slice *vec2_data, char *line)
{
    float u, v;
    sscanf(line, "%f %f", &u, &v);
    append_slice(vec2_data, &u);
    append_slice(vec2_data, &v);
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

void sscanf_face(char *line, size_t len, size_t vertex_count, size_t3 data[])
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
                   &data[i][FACE_VERTS], &data[i][FACE_UVS], &data[i][FACE_NORMALS]) != 3)
        {
            data[i][FACE_NORMALS] = 0;
            if (sscanf(string_buffer, "%llu/%llu/", &data[i][FACE_VERTS], &data[i][FACE_UVS]) != 2)
            {
                data[i][FACE_UVS] = 0;
                if (sscanf(string_buffer, "%llu//%llu", &data[i][FACE_VERTS], &data[i][FACE_NORMALS]) != 2)
                {
                    sscanf(string_buffer, "%llu//", &data[i][FACE_VERTS]);
                }
            }
        }

        cursor = end + 1;
    }
}

void faces_to_elements(slice *elements, size_t n, size_t3 face_data[])
{
    int iters = n - 2;

    for (int x = 0; x < n; x++)
    {
        face_data[x][FACE_VERTS] = face_data[x][FACE_VERTS] - 1;
        face_data[x][FACE_UVS] = face_data[x][FACE_UVS] - 1;
        face_data[x][FACE_NORMALS] = face_data[x][FACE_NORMALS] - 1;
    }

    if (iters <= 0)
    {

        extend_slice(elements, COUNT_FACE_METADATA, face_data[0]);
        extend_slice(elements, COUNT_FACE_METADATA, face_data[1]);
        extend_slice(elements, COUNT_FACE_METADATA, face_data[2]);
        return;
    }

    for (int x = 0; x < iters; x++)
    {
        if (x == 0)
        {
            extend_slice(elements, COUNT_FACE_METADATA, face_data[0]);
            extend_slice(elements, COUNT_FACE_METADATA, face_data[1]);
            extend_slice(elements, COUNT_FACE_METADATA, face_data[2]);
        }
        else
        {
            int offset = x + 1;

            extend_slice(elements, COUNT_FACE_METADATA, face_data[0]);
            extend_slice(elements, COUNT_FACE_METADATA, face_data[offset]);
            extend_slice(elements, COUNT_FACE_METADATA, face_data[offset + 1]);
        }
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

    size_t3 data[vertex_count];
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

    size_t *elements = (size_t *)buffer_slices[ELEMENTS].data;
    vec3 *vertices = (vec3 *)buffer_slices[VERTS].data;
    vec3 *normals = (vec3 *)buffer_slices[NORMALS].data;

    vec3 normal, a, b;
    for (int x = 0; x < buffer_slices[ELEMENTS].len; x += 9)
    {
        float *p1 = vertices[elements[x + FACE_VERTS]];
        float *p2 = vertices[elements[x + 3 + FACE_VERTS]];
        float *p3 = vertices[elements[x + 6 + FACE_VERTS]];

        vec3_sub(a, p2, p1);
        vec3_sub(b, p3, p1);
        vec3_mul_cross(normal, a, b);

        float *n1 = normals[elements[x + FACE_VERTS]];
        float *n2 = normals[elements[x + 3 + FACE_VERTS]];
        float *n3 = normals[elements[x + 6 + FACE_VERTS]];

        vec3_add(n1, n1, normal);
        vec3_add(n2, n2, normal);
        vec3_add(n3, n3, normal);

        elements[x + FACE_NORMALS] = elements[x + FACE_VERTS];
        elements[x + 3 + FACE_NORMALS] = elements[x + 3 + FACE_VERTS];
        elements[x + 6 + FACE_NORMALS] = elements[x + 6 + FACE_VERTS];
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

void load_mesh(const char *obj_file_name, mesh *mesh)
{
    slice buffers[COUNT_BUFFERS];
    load_obj_file(obj_file_name, buffers);

    size_t *elements = (size_t *)buffers[ELEMENTS].data;
    vec3 *vertices_index = (vec3 *)buffers[VERTS].data;
    vec3 *normal_index = (vec3 *)buffers[NORMALS].data;
    vec2 *uv_index = (vec2 *)buffers[UVS].data;

    // TODO: find somewhere else for measuring bounding box
    int num_verts = buffers[VERTS].len / 3;
    mesh->bounds[0] = vertices_index[0][0];
    mesh->bounds[1] = vertices_index[0][0];
    mesh->bounds[2] = vertices_index[0][1];
    mesh->bounds[3] = vertices_index[0][1];
    mesh->bounds[4] = vertices_index[0][2];
    mesh->bounds[5] = vertices_index[0][2];
    for (size_t i = 1; i < num_verts; i++)
    {
        for (int y = 0; y < 3; y++)
        {
            int offset = y * 2;
            if (vertices_index[i][y] < mesh->bounds[offset])
            {
                mesh->bounds[offset] = vertices_index[i][y];
            }
            else if (vertices_index[i][y] > mesh->bounds[offset + 1])
            {
                mesh->bounds[offset + 1] = vertices_index[i][y];
            }
        }
    }

    vec3 to_local_origin = {
        ((mesh->bounds[1] - mesh->bounds[0]) / 2) - mesh->bounds[1],
        ((mesh->bounds[3] - mesh->bounds[2]) / 2) - mesh->bounds[3],
        ((mesh->bounds[5] - mesh->bounds[4]) / 2) - mesh->bounds[5],
    };

    for (size_t i = 0; i < buffers[VERTS].len / 3; i++)
    {
        vec3_add(vertices_index[i], vertices_index[i], to_local_origin);
    }
    mesh->bounds[0] += to_local_origin[0];
    mesh->bounds[1] += to_local_origin[0];
    mesh->bounds[2] += to_local_origin[1];
    mesh->bounds[3] += to_local_origin[1];
    mesh->bounds[4] += to_local_origin[2];
    mesh->bounds[5] += to_local_origin[2];

    vertex vert = {0};
    uintptr_t position_offset = offsetof(vertex, position);
    size_t position_size = sizeof(vert.position);

    uintptr_t normal_offset = offsetof(vertex, normal);
    size_t normal_size = sizeof(vert.normal);

    uintptr_t uv_offset = offsetof(vertex, uv);
    size_t uv_size = sizeof(vert.uv);

    size_t vertices_len = buffers[ELEMENTS].len / 3;
    size_t vertices_cap = vertices_len * sizeof(vertex);
    slice vertices_slice = {
        vertices_len,
        vertices_cap,
        sizeof(vertex),
        malloc(vertices_cap)};
    memset(vertices_slice.data, 0, vertices_cap);

    int step;
    vertex *vertices = (vertex *)vertices_slice.data;
    for (int x = 0; x < buffers[ELEMENTS].len; x += 9)
    {

        step = x / 3;

        size_t idx1_src = elements[x + 0 + FACE_VERTS];
        vec3 *v1_src = &vertices_index[idx1_src];
        size_t idx2_src = elements[x + 3 + FACE_VERTS];
        vec3 *v2_src = &vertices_index[idx2_src];
        size_t idx3_src = elements[x + 6 + FACE_VERTS];
        vec3 *v3_src = &vertices_index[idx3_src];

        memcpy(((void *)&vertices[0 + step]) + position_offset, vertices_index[elements[x + FACE_VERTS]], position_size);
        memcpy(((void *)&vertices[1 + step]) + position_offset, vertices_index[elements[x + 3 + FACE_VERTS]], position_size);
        memcpy(((void *)&vertices[2 + step]) + position_offset, vertices_index[elements[x + 6 + FACE_VERTS]], position_size);
    }

    if (buffers[NORMALS].len)
    {
        for (int x = 0; x < buffers[ELEMENTS].len; x += 9)
        {
            step = x / 3;
            memcpy(((void *)&vertices[0 + step]) + normal_offset, normal_index[elements[x + FACE_NORMALS]], normal_size);
            memcpy(((void *)&vertices[1 + step]) + normal_offset, normal_index[elements[x + 3 + FACE_NORMALS]], normal_size);
            memcpy(((void *)&vertices[2 + step]) + normal_offset, normal_index[elements[x + 6 + FACE_NORMALS]], normal_size);
        }
    }

    if (buffers[UVS].len)
    {
        for (int x = 0; x < buffers[ELEMENTS].len; x += 9)
        {
            step = x / 3;
            memcpy(((void *)&vertices[0 + step]) + uv_offset, uv_index[elements[x + FACE_UVS]], uv_size);
            memcpy(((void *)&vertices[1 + step]) + uv_offset, uv_index[elements[x + 3 + FACE_UVS]], uv_size);
            memcpy(((void *)&vertices[2 + step]) + uv_offset, uv_index[elements[x + 6 + FACE_UVS]], uv_size);
        }
    }

    mesh->vertices = vertices_slice;

    free_buffers(buffers);
}
