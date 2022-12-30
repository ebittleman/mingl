
#ifndef _SCENES_H
#define _SCENES_H

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "types.h"
#include "entities.h"
#include "shaders/shaders.h"
#include "geometry/geometry.h"

void default_scene(scene *scene, model **model, shader *scene_shader, int x, int count, bool display_bounds);
void lamp_scene(scene *scene, light *light, material material);

#endif
