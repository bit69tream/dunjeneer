#pragma once

#include "player.h"
#include "level_generator.h"

void render(LevelMap map,
            LevelObject objects[OBJECTS_MAX], size_t objects_len,
            Player player);
void initialize_rendering(void);
void cleanup_rendering(void);
