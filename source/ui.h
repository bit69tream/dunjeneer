#pragma once

#include "player.h"
#include "level_generator.h"

void render(LevelMap map,
            LevelObject objects[OBJECTS_MAX], size_t objects_len,
            Player player);
void init_rendering(void);
void cleanup_rendering(void);

Point mouse_in_world(void);
void update_mouse(void);
