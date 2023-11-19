#pragma once

#include <raylib.h>
#include <stdint.h>
#include <stddef.h>

#include "level_generator.h"

#define GLYPH_WIDTH 5
#define GLYPH_HEIGHT 7
#define GLYPH_GAP 1

extern Rectangle glyphs[128];

Texture2D font_texture(void);
size_t tile_to_glyph(LevelTile tile);
