#ifndef MATERIALS_C
#define MATERIALS_C
#include "shaders/shaders.h"

// http://devernay.free.fr/cours/opengl/materials.html
phong_material phong_materials[] = {
    // Emerald
    {{0.215f, 0.1745f, 0.0215f},
     {0.07568f, 0.61424f, 0.07568f},
     {0.633f, 0.633f, 0.633f},
     0.6f * 128.0f,
     1.0f,
     {1.0f, 1.0f, 1.0f},
     1.0f,
     HighlightOn},

    // Jade
    {{0.135f, 0.2225f, 0.1575f},
     {0.54f, 0.89f, 0.63f},
     {0.316228f, 0.316228f, 0.316228f},
     0.1f * 128.0f,
     1.0f,
     {1.0f, 1.0f, 1.0f},
     1.0f,
     HighlightOn},

    // Obsidian
    {{0.05375f, 0.05f, 0.06625f},
     {0.18275f, 0.17f, 0.22525f},
     {0.332741f, 0.328634f, 0.346435f},
     0.3f * 128.0f,
     1.0f,
     {1.0f, 1.0f, 1.0f},
     1.0f,
     HighlightOn},

    // Pearl
    {{0.25f, 0.20725f, 0.20725f},
     {1.0f, 0.829f, 0.829f},
     {0.296648f, 0.296648f, 0.296648f},
     0.088f * 128.0f,
     1.0f,
     {1.0f, 1.0f, 1.0f},
     1.0f,
     HighlightOn},

    // Ruby
    {{0.1745f, 0.01175f, 0.01175f},
     {0.61424f, 0.04136f, 0.04136f},
     {0.727811f, 0.626959f, 0.626959f},
     0.6f * 128.0f,
     1.0f,
     {1.0f, 1.0f, 1.0f},
     1.0f,
     HighlightOn},

    // Turquoise
    {{0.1f, 0.18725f, 0.1745f},
     {0.396f, 0.74151f, 0.69102f},
     {0.297254f, 0.30829f, 0.306678f},
     0.1f * 128.0f,
     1.0f,
     {1.0f, 1.0f, 1.0f},
     1.0f,
     HighlightOn},

    // Yellow Rubber
    {{0.05f, 0.05f, 0.0f},
     {0.5f, 0.5f, 0.4f},
     {0.7f, 0.7f, 0.04f},
     0.078125f * 128.0f,
     1.0f,
     {1.0f, 1.0f, 1.0f},
     1.0f,
     HighlightOn},

};

enum PhongMaterialIDS
{
    PHONG_EMERALD = 0,
    PHONG_JADE,
    PHONG_OBSIDIAN,
    PHONG_PEARL,
    PHONG_RUBY,
    PHONG_TURQUOISE,

    PHONG_YELLOW_RUBBER,

    PHONG_MATERIAL_COUNT,
};

#endif
