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
    if (!strncmp(pre, line, strlen(pre)) == 0)
    {
        return;
    }
    parse_vec3_line(vertex_data, line + 2);
}

void handle_normal_line(slice *normal_data, char *line)
{

    char *pre = "vn ";
    if (!strncmp(pre, line, strlen(pre)) == 0)
    {
        return;
    }
    parse_vec3_line(normal_data, line + 3);
}

void handle_uv_line(slice *uv_data, char *line)
{

    char *pre = "vt ";
    if (!strncmp(pre, line, strlen(pre)) == 0)
    {
        return;
    }
    parse_uv_line(uv_data, line + 2);
}

void handle_face_line(slice *element_data, char *line)
{

    char *pre = "f ";
    if (!strncmp(pre, line, strlen(pre)) == 0)
    {
        return;
    }
    parse_face_line(element_data, line + 2);
}

void parse_face_line(slice *element_data, char *line)
{

    int num = -1;

    bool no_slashes = strchr(line, '/') == NULL;

    size_t face_data[18];
    size_t count = 0;

    char num_buff[32] = {0}; // 32 chars + terminator */
    int len = strlen(line);

    int i = 0;
    int start = 0;
    int end = 0;
    bool reading = false;
    char *last_seen = NULL;
    for (i = 0; i < len; i++)
    {
        char ch = line[i];
        if (ch != 0x20 && ch != 0x2f)
        {
            last_seen = &line[i];
            if (i == 0 || line[i - 1] == 0x20 || line[i - 1] == 0x2f)
            {
                start = i;
                reading = true;
            }
        }
        else if (ch == 0x20)
        {
            if (!reading)
            {
                if (last_seen != NULL && *last_seen == 0x2f)
                {
                    face_data[count++] = 0;
                }
                continue;
            }

            end = i;
            memset(num_buff, 0, sizeof(num_buff));
            strncpy(num_buff, line + start, end - start);
            num = atoi(num_buff);
            face_data[count++] = num;
            if (no_slashes)
            {
                face_data[count++] = 0;
                face_data[count++] = 0;
            }

            reading = false;
        }
        else if (ch == 0x2f)
        {
            last_seen = &line[i];
            if (!reading)
            {
                face_data[count++] = 0;
                continue;
            }
            end = i;
            memset(num_buff, 0, sizeof(num_buff));
            strncpy(num_buff, line + start, end - start);
            num = atoi(num_buff);
            face_data[count++] = num;

            reading = false;
        }
    }

    if (last_seen != NULL && *last_seen == 0x2f)
    {
        face_data[count++] = 0;
    }
    else if (reading)
    {
        end = len;
        memset(num_buff, 0, sizeof(num_buff));
        strncpy(num_buff, line + start, end - start);
        num = atoi(num_buff);
        face_data[count++] = num;
        if (no_slashes)
        {
            face_data[count++] = 0;
            face_data[count++] = 0;
        }
        reading = false;
    }

    faces_to_elements(element_data, count, face_data);
}

void faces_to_elements(slice *elements, size_t n, size_t face_data[])
{
    int face_buffer[32];
    for (int x = 0; x < n; x += 3)
    {
        int step = (n - (n - x)) / 3;
        face_buffer[step] = face_data[x] - 1;
        // printf("line_idx: %d, vert_idx: %d\n", step, face_data[x]);
    }
    // printf("count: %d\n", (n / 3));

    int iters = (n / 3) - 2;

    if (iters <= 0)
    {
        append_slice(elements, &face_buffer[0]);
        append_slice(elements, &face_buffer[1]);
        append_slice(elements, &face_buffer[2]);
        return;
    }

    for (int x = 0; x < iters; x++)
    {
        if (x == 0)
        {
            append_slice(elements, &face_buffer[0]);
            append_slice(elements, &face_buffer[1]);
            append_slice(elements, &face_buffer[2]);
            // printf("%d, %d, %d\n",
            //        face_buffer[0],
            //        face_buffer[1],
            //        face_buffer[2]);
        }
        else
        {
            int offset = x + 1;
            append_slice(elements, &face_buffer[0]);
            append_slice(elements, &face_buffer[offset]);
            append_slice(elements, &face_buffer[offset + 1]);
            // printf("%d, %d, %d\n",
            //        face_buffer[offset],
            //        face_buffer[offset + 1],
            //        face_buffer[0]);
        }
    }

    return;
}

void load_obj_file(const char *obj_file_name, slice buffers[COUNT_BUFFERS], float bounds[6])
{
    slice vertices_slice = new_slice(sizeof(float));
    slice normals_slice = new_slice(sizeof(float));
    slice uv_slice = new_slice(sizeof(float));
    slice elements_slice = new_slice(sizeof(int));

    line_callback_t *callbacks[COUNT_BUFFERS];
    callbacks[VERTS] = &handle_vertex_line;
    callbacks[UVS] = &handle_uv_line;
    // callbacks[NORMALS] = &handle_normal_line;
    callbacks[NORMALS] = &handle_noop;
    callbacks[COLORS] = &handle_noop;
    callbacks[ELEMENTS] = &handle_face_line;

    slice *buff_ptr[COUNT_BUFFERS];
    buffers[VERTS] = vertices_slice;
    buffers[UVS] = uv_slice;
    buffers[NORMALS] = normals_slice;
    buffers[COLORS] = new_slice(sizeof(float));
    buffers[ELEMENTS] = elements_slice;

    buff_ptr[VERTS] = &buffers[VERTS];
    buff_ptr[UVS] = &buffers[UVS];
    buff_ptr[NORMALS] = &buffers[NORMALS];
    buff_ptr[COLORS] = &buffers[COLORS];
    buff_ptr[ELEMENTS] = &buffers[ELEMENTS];

    line_reader(obj_file_name, callbacks, buff_ptr);

    vertices_slice = buffers[VERTS];
    printf("Found %d vertices\n", vertices_slice.len);
    vec3 *verts = (vec3 *)vertices_slice.data;
    int num_verts = vertices_slice.len / 3;

    bounds[0] = verts[0][0];
    bounds[1] = verts[0][0];
    bounds[2] = verts[0][1];
    bounds[3] = verts[0][1];
    bounds[4] = verts[0][2];
    bounds[5] = verts[0][2];

    for (size_t i = 1; i < num_verts; i++)
    {
        for (int y = 0; y < 3; y++)
        {
            int offset = y * 2;
            if (verts[i][y] < bounds[offset])
            {
                bounds[offset] = verts[i][y];
            }
            else if (verts[i][y] > bounds[offset + 1])
            {
                bounds[offset + 1] = verts[i][y];
            }
        }

        // printf("x: %f, y: %f, z: %f\n", verts[0], verts[1], verts[2]);
    }

    printf("min x: %f, max x: %f\n", bounds[0], bounds[1]);
    printf("min y: %f, max y: %f\n", bounds[2], bounds[3]);
    printf("min z: %f, max z: %f\n", bounds[4], bounds[5]);

    printf("Found %d elements\n", buffers[ELEMENTS].len);
    // for (size_t i = 0; i < elements_slice.len; i += 3)
    // {
    //     printf("x: %d, y: %d, z: %d\n",
    //            elements[i],
    //            elements[i + 1],
    //            elements[i + 2]);
    // }

    return;
}
