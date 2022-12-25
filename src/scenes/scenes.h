
#ifndef _SCENES_H
#define _SCENES_H

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "types.h"
#include "entities.h"
#include "geometry/geometry.h"

scene default_scene(slice *models_table, int x, int count, bool display_bounds);

#endif
