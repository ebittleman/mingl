
#ifndef _SCENES_H
#define _SCENES_H

#include <stdbool.h>

#include "types.h"
#include "entities.h"

void static_object(scene *scene, model **model, material *base_material,
                   material *debug_material, int x, int count,
                   bool display_bounds);
void lamp_scene(scene *scene, light *light, material *material);

#endif
