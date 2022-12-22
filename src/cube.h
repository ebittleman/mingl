#include <strings.h>
#include "entities.h"

void cube_vertices(vertex dst[36], float bounds[6])
{
    float minX = bounds[0], maxX = bounds[1],
          minY = bounds[2], maxY = bounds[3],
          minZ = bounds[4], maxZ = bounds[5];
    vertex vertices[] = {
        // front face
        {minX, minY, maxZ,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {maxX, minY, maxZ,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {maxX, maxY, maxZ,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {minX, minY, maxZ,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {maxX, maxY, maxZ,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {minX, maxY, maxZ,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},

        // back face
        {maxX, maxY, minZ,
         0.0f, 0.0f, -1.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {maxX, minY, minZ,
         0.0f, 0.0f, -1.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {minX, minY, minZ,
         0.0f, 0.0f, -1.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {maxX, maxY, minZ,
         0.0f, 0.0f, -1.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {minX, minY, minZ,
         0.0f, 0.0f, -1.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {minX, maxY, minZ,
         0.0f, 0.0f, -1.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},

        // top face
        {minX, maxY, minZ,
         0.0f, 1.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {maxX, maxY, maxZ,
         0.0f, 1.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {maxX, maxY, minZ,
         0.0f, 1.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {minX, maxY, minZ,
         0.0f, 1.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {minX, maxY, maxZ,
         0.0f, 1.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {maxX, maxY, maxZ,
         0.0f, 1.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},

        // bottom face
        {maxX, minY, maxZ,
         0.0f, -1.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {minX, minY, minZ,
         0.0f, -1.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {maxX, minY, minZ,
         0.0f, -1.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {minX, minY, maxZ,
         0.0f, -1.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {minX, minY, minZ,
         0.0f, -1.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {maxX, minY, maxZ,
         0.0f, -1.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},

        // Right Face
        {maxX, maxY, maxZ,
         1.0f, 0.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {maxX, minY, minZ,
         1.0f, 0.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {maxX, maxY, minZ,
         1.0f, 0.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {maxX, maxY, maxZ,
         1.0f, 0.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {maxX, minY, maxZ,
         1.0f, 0.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {maxX, minY, minZ,
         1.0f, 0.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},

        // Left Face
        {minX, maxY, maxZ,
         -1.0f, 0.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {minX, maxY, minZ,
         -1.0f, 0.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {minX, minY, minZ,
         -1.0f, 0.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {minX, maxY, maxZ,
         -1.0f, 0.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {minX, minY, minZ,
         -1.0f, 0.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
        {minX, minY, maxZ,
         -1.0f, 0.0f, 0.0f,
         0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f,
         0, 0, 0, 0,
         0.0f, 0.0f, 0.0f, 0.0f},
    };

    memcpy(dst, vertices, sizeof(vertices));
}