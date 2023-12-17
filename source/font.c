#include "font.h"
#include "level_generator.h"
#include "player.h"
#include "raylib.h"
#include "utils.h"

#include <assert.h>

/* https://opengameart.org/content/ascii-bitmap-font-oldschool */
extern const unsigned char charmap_oldschool_white_png[];
extern const unsigned long charmap_oldschool_white_png_size;

Rectangle glyphs[128];

#define GLYPHS_IN_A_LINE 18
#define FIRST_GLYPH_IN_A_FONT ' '
void fill_glyphs_array(void) {
  int current_y = GLYPH_GAP;
  int current_x = GLYPH_GAP;
  size_t glyphs_scanned = 0;

  for (size_t i = FIRST_GLYPH_IN_A_FONT; i < SIZE_OF(glyphs); i++) {
    if ((glyphs_scanned != 0) &&
        (glyphs_scanned % GLYPHS_IN_A_LINE == 0)) {
      current_y += GLYPH_HEIGHT + (2 * GLYPH_GAP);
      current_x = GLYPH_GAP;
    }

    glyphs[i].x = (float)current_x;
    glyphs[i].y = (float)current_y;
    glyphs[i].width = GLYPH_WIDTH;
    glyphs[i].height = GLYPH_HEIGHT;

    current_x += GLYPH_WIDTH + (2 * GLYPH_GAP);
    glyphs_scanned ++;
  }

  static_assert(SPECIAL_GLYPHS_COUNT < FIRST_GLYPH_IN_A_FONT, "too much special glyphs");

  glyphs[SPECIAL_GLYPH_RECTANGLE] = (Rectangle) {
    .x = 1,
    .y = 55,
    .width = GLYPH_WIDTH,
    .height = GLYPH_HEIGHT,
  };

  glyphs[SPECIAL_GLYPH_CENTERED_DOT] = (Rectangle) {
    .x = 8,
    .y = 55,
    .width = GLYPH_WIDTH,
    .height = GLYPH_HEIGHT,
  };
}

Texture2D font_texture(void) {
  Image font_image = LoadImageFromMemory(".png", charmap_oldschool_white_png, (int)charmap_oldschool_white_png_size);
  ImageFormat(&font_image, PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA);
  ImageColorReplace(&font_image, BLACK, BLANK);

  Texture font_texture = LoadTextureFromImage(font_image);

  UnloadImage(font_image);

  fill_glyphs_array();

  return font_texture;
}

size_t tile_to_glyph(LevelTileType tile) {
  switch (tile) {
  case TILE_ELEVATOR_UP: return '<';
  case TILE_ELEVATOR_DOWN: return '>';

  case TILE_NONE: return ' ';

  case TILE_WALL: return '#';

  case TILE_VERTICAL_LOCKED_DOOR:
  case TILE_VERTICAL_CLOSED_DOOR: return '|';

  case TILE_HORIZONTAL_LOCKED_DOOR:
  case TILE_HORIZONTAL_CLOSED_DOOR: return '-';

  case TILE_FLOOR: return '.';

  case TILE_VERTICAL_OPENED_DOOR: return '_';
  case TILE_HORIZONTAL_OPENED_DOOR: return '|';

  case TILE_GROUND: return '*';
  case TILE_HILL: return ';';

  case TILE_ROCK: return '%';
  case TILE_GREY_ROCK: return '%';
  case TILE_HARD_ROCK: return '%';

  case TILE_GREY_ROCK_GRAVEL:
  case TILE_ROCK_GRAVEL: return ';';

  case LEVEL_TILE_COUNT: assert(false && "bruh");
  }

  assert(false && "unknown tile");
}

size_t object_type_to_glyph(LevelObjectType object_type) {
  switch (object_type) {
  case OBJECT_NONE: assert(false && "skill issue");
  case LEVEL_OBJECT_COUNT: assert(false && "nuh uh");
  }

  assert(false && "unknown tile");
}

size_t action_to_glyph(Action action) {
  switch (action) {
  case ACTION_NONE: assert(false && "no glyph for you");
  case ACTION_OPEN: return 'O';
  case ACTION_CLOSE: return 'C';
  case ACTION_PICK_UP: return 'P';
  case ACTION_CLIMB: return 'c';
  case ACTION_COUNT: assert(false && "illegal");
  }

  assert(false && "unknown action");
}
