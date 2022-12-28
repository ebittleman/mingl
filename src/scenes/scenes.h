
#ifndef _SCENES_H
#define _SCENES_H

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "types.h"
#include "entities.h"
#include "geometry/geometry.h"

void default_scene(scene *scene, size_t model_id, int x, int count, bool display_bounds);

#endif
