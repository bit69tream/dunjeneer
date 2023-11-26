#pragma once

#include <raylib.h>
#include <stdint.h>
#include <stddef.h>

#include "level_generator.h"
#include "player.h"

#define GLYPH_WIDTH 5
#define GLYPH_HEIGHT 7
#define GLYPH_GAP 1

typedef enum {
  SPECIAL_GLYPH_NONE,
  SPECIAL_GLYPH_RECTANGLE,
  SPECIAL_GLYPH_CENTERED_DOT,
  SPECIAL_GLYPHS_COUNT,
} SpecialGlyph;

extern Rectangle glyphs[128];

Texture2D font_texture(void);
size_t tile_to_glyph(LevelTile tile);
size_t object_type_to_glyph(LevelObjectType object_type);
size_t action_to_glyph(Action action);
