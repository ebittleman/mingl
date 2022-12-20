#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linmath.h"
#include "obj3d.h"

#define LINE_BUF_SIZE 1024

int line_reader(
    const char *filename,
    line_callback_t *callbacks[COUNT_BUFFERS],
    slice *buffers[COUNT_BUFFERS])
{
    FILE *f = NULL;
    int result = -1;
    char line_buffer[LINE_BUF_SIZE];

    f = fopen(filename, "r");
    if (f == NULL)
        goto fail;

    while (fgets(line_buffer, LINE_BUF_SIZE, f) != NULL)
    {
        if (ferror(f))
            goto fail;

        for (size_t i = 0; i < COUNT_BUFFERS; i++)
        {
            callbacks[i](buffers[i], line_buffer);
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

void parse_uv_line(slice *normal_data, char *line)
{
    float u, v;
    sscanf(line, "%f %f", &u, &v);
    append_slice(normal_data, &u);
    append_slice(normal_data, &v);
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
    parse_uv_line(uv_data, line + 2);
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

void handle_face_line(slice *element_data, char *line)
{

    char *pre = "f ";
    if (strncmp(pre, line, strlen(pre)) != 0)
    {
        return;
    }
    parse_face_line(element_data, line + 2);
}

void load_obj_file(const char *obj_file_name, slice buffers[COUNT_BUFFERS], float bounds[6])
{
    slice vertices_index_slice = new_slice(sizeof(float));
    slice normals_index_slice = new_slice(sizeof(float));
    slice uv_index_slice = new_slice(sizeof(float));
    slice elements_slice = new_slice(sizeof(size_t));

    line_callback_t *callbacks[COUNT_BUFFERS];
    callbacks[VERTS] = &handle_vertex_line;

    callbacks[UVS] = &handle_uv_line;
    callbacks[NORMALS] = &handle_normal_line;
    callbacks[COLORS] = &handle_noop;

    callbacks[ELEMENTS] = &handle_face_line;

    buffers[VERTS] = vertices_index_slice;
    buffers[UVS] = uv_index_slice;
    buffers[NORMALS] = normals_index_slice;
    buffers[COLORS] = new_slice(sizeof(float));
    buffers[ELEMENTS] = elements_slice;

    slice *buff_ptr[COUNT_BUFFERS];
    buff_ptr[VERTS] = &buffers[VERTS];
    buff_ptr[UVS] = &buffers[UVS];
    buff_ptr[NORMALS] = &buffers[NORMALS];
    buff_ptr[COLORS] = &buffers[COLORS];
    buff_ptr[ELEMENTS] = &buffers[ELEMENTS];

    line_reader(obj_file_name, callbacks, buff_ptr);

    vertices_index_slice = buffers[VERTS];
    // uv_index_slice = buffers[UVS];
    // normals_index_slice = buffers[NORMALS];

    if (buffers[NORMALS].len < 1)
    {
        calculate_normal_per_vertex(buffers);
    }

    size_t *elements = (size_t *)buffers[ELEMENTS].data;

    vec3 *vertices_index = (vec3 *)buffers[VERTS].data;
    bounds[0] = vertices_index[0][0];
    bounds[1] = vertices_index[0][0];
    bounds[2] = vertices_index[0][1];
    bounds[3] = vertices_index[0][1];
    bounds[4] = vertices_index[0][2];
    bounds[5] = vertices_index[0][2];

    int num_verts = buffers[VERTS].len / 3;
    for (size_t i = 1; i < num_verts; i++)
    {
        for (int y = 0; y < 3; y++)
        {
            int offset = y * 2;
            if (vertices_index[i][y] < bounds[offset])
            {
                bounds[offset] = vertices_index[i][y];
            }
            else if (vertices_index[i][y] > bounds[offset + 1])
            {
                bounds[offset + 1] = vertices_index[i][y];
            }
        }
    }

    // printf("min x: %f, max x: %f\n", bounds[0], bounds[1]);
    // printf("min y: %f, max y: %f\n", bounds[2], bounds[3]);
    // printf("min z: %f, max z: %f\n", bounds[4], bounds[5]);

    // TODO: expand verts, normals, and uvs, then free everything not leaving
    slice vertices_slice = new_slice(sizeof(vec3));
    for (int x = 0; x < buffers[ELEMENTS].len; x += 9)
    {
        float *p1 = vertices_index[elements[x + FACE_VERTS]];
        float *p2 = vertices_index[elements[x + 3 + FACE_VERTS]];
        float *p3 = vertices_index[elements[x + 6 + FACE_VERTS]];
        append_slice(&vertices_slice, p1);
        append_slice(&vertices_slice, p2);
        append_slice(&vertices_slice, p3);
    }
    free(buffers[VERTS].data);
    buffers[VERTS] = vertices_slice;

    slice uv_slice = new_slice(sizeof(vec2));
    vec2 *uv_index = (vec2 *)buffers[UVS].data;
    if (buffers[UVS].len)
    {
        for (int x = 0; x < buffers[ELEMENTS].len; x += 9)
        {
            float *p1 = uv_index[elements[x + FACE_UVS]];
            float *p2 = uv_index[elements[x + 3 + FACE_UVS]];
            float *p3 = uv_index[elements[x + 6 + FACE_UVS]];
            append_slice(&uv_slice, p1);
            append_slice(&uv_slice, p2);
            append_slice(&uv_slice, p3);
        }
    }
    free(buffers[UVS].data);
    buffers[UVS] = uv_slice;

    slice normals_slice = new_slice(sizeof(vec3));
    vec3 *normal_index = (vec3 *)buffers[NORMALS].data;
    for (int x = 0; x < buffers[ELEMENTS].len; x += 9)
    {
        float *p1 = normal_index[elements[x + FACE_NORMALS]];
        float *p2 = normal_index[elements[x + 3 + FACE_NORMALS]];
        float *p3 = normal_index[elements[x + 6 + FACE_NORMALS]];
        append_slice(&normals_slice, p1);
        append_slice(&normals_slice, p2);
        append_slice(&normals_slice, p3);
    }
    free(buffers[NORMALS].data);
    buffers[NORMALS] = normals_slice;

    // printf("Found %d elements\n", buffers[ELEMENTS].len);
    // for (size_t i = 0; i < elements_slice.len; i += 3)
    // {
    //     printf("x: %d, y: %d, z: %d\n",
    //            elements[i],
    //            elements[i + 1],
    //            elements[i + 2]);
    // }

    return;
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