#include "font.h"
#include "level_generator.h"
#include "raylib.h"
#include "utils.h"

#include <assert.h>

/* https://opengameart.org/content/ascii-bitmap-font-oldschool */
static const unsigned char charmap_oldschool_white_png[] = {
  0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x80,
  0x00, 0x00, 0x00, 0x40, 0x08, 0x02, 0x00, 0x00, 0x00, 0x5D, 0xB4, 0xE8, 0x28, 0x00, 0x00, 0x00, 0x01, 0x73, 0x52, 0x47,
  0x42, 0x00, 0xAE, 0xCE, 0x1C, 0xE9, 0x00, 0x00, 0x03, 0xDB, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9C, 0xED, 0x5C, 0xD9, 0x72,
  0xE4, 0x20, 0x0C, 0xC4, 0xA9, 0xF9, 0xFF, 0x5F, 0x76, 0x1E, 0x66, 0x42, 0x18, 0x1D, 0x4D, 0x8B, 0xC3, 0x4E, 0x65, 0xD4,
  0x0F, 0x5B, 0x5E, 0x59, 0x80, 0x04, 0x3A, 0x71, 0x92, 0x52, 0x12, 0x7F, 0x19, 0xE7, 0x79, 0x9A, 0x14, 0x41, 0x37, 0x89,
  0x26, 0x9B, 0xE6, 0xA9, 0xF4, 0xEE, 0x70, 0x30, 0xAD, 0x27, 0xFF, 0x5D, 0xE0, 0x35, 0xFA, 0xDA, 0x2A, 0x44, 0x81, 0x9B,
  0x5E, 0x1F, 0x8E, 0xE3, 0x38, 0x8E, 0x83, 0x9F, 0x56, 0x30, 0x1F, 0xC7, 0x01, 0x0C, 0xE2, 0xB2, 0xD3, 0xD2, 0x1A, 0x09,
  0xC1, 0x46, 0x96, 0x66, 0xC6, 0x9C, 0xEF, 0x68, 0x07, 0x32, 0x26, 0x8C, 0x39, 0x79, 0x79, 0x4C, 0x6F, 0xE3, 0xBD, 0x6A,
  0x12, 0xE6, 0x46, 0x33, 0x1B, 0xB2, 0xC0, 0x03, 0x9E, 0xF6, 0xD8, 0xFE, 0x5B, 0x1A, 0xAB, 0x34, 0xB7, 0xA0, 0x35, 0xE1,
  0x27, 0x27, 0xEF, 0x01, 0x0C, 0xF0, 0x76, 0x78, 0x4E, 0xD9, 0x0E, 0xD1, 0x26, 0x85, 0xF9, 0xB5, 0xFC, 0x5A, 0x4D, 0x5E,
  0xFE, 0xB7, 0x59, 0x18, 0x1E, 0x4F, 0x5C, 0x7D, 0x06, 0x38, 0xA9, 0x74, 0x97, 0xDB, 0xE4, 0x01, 0x33, 0x6E, 0x61, 0x2E,
  0x21, 0x0C, 0x7F, 0xC9, 0x42, 0x7D, 0x21, 0xC4, 0x46, 0xEB, 0x2D, 0x00, 0xBE, 0xC9, 0x0B, 0xC7, 0xEC, 0x20, 0x38, 0xD4,
  0xB5, 0x07, 0xD0, 0x9D, 0x6D, 0xF6, 0x00, 0x42, 0x9B, 0xC2, 0x07, 0x5C, 0x7D, 0x30, 0x33, 0x99, 0x60, 0xD2, 0xAC, 0x42,
  0xC3, 0x79, 0x09, 0x4D, 0x1D, 0xF9, 0xB0, 0x96, 0x48, 0x7C, 0x06, 0x5E, 0xA9, 0xB9, 0x3A, 0x45, 0x9B, 0xAC, 0x4D, 0x62,
  0xA5, 0x57, 0xA2, 0x70, 0x28, 0x4D, 0x1F, 0xE0, 0x04, 0xAB, 0x5F, 0x03, 0x5E, 0x24, 0xAC, 0x26, 0xA5, 0x91, 0x97, 0x27,
  0x01, 0x03, 0x2E, 0x6C, 0x3C, 0xA2, 0xC9, 0x10, 0x5A, 0x3D, 0xBA, 0x1C, 0x13, 0x6D, 0x71, 0xEA, 0x1A, 0x90, 0x33, 0xA4,
  0xD1, 0xA3, 0x38, 0x26, 0x66, 0x12, 0xCF, 0x45, 0x05, 0xFB, 0xCC, 0x3C, 0xC3, 0xD2, 0x7A, 0xFE, 0x17, 0x6A, 0x44, 0x96,
  0xE4, 0xCF, 0x76, 0xAD, 0x87, 0x98, 0x1D, 0x47, 0x9B, 0xF2, 0x7E, 0xA4, 0x63, 0x31, 0xE4, 0xB4, 0x1A, 0x31, 0xC1, 0xA9,
  0x6F, 0x17, 0xA2, 0xD0, 0xBA, 0x80, 0x2D, 0x16, 0x67, 0xD0, 0xEA, 0xA5, 0x47, 0x31, 0x17, 0x0C, 0x23, 0xF1, 0x13, 0x28,
  0x1C, 0xAA, 0x2F, 0x35, 0x03, 0x16, 0x77, 0x3E, 0x9A, 0x91, 0x03, 0x41, 0x21, 0x48, 0x0E, 0x6F, 0x27, 0x01, 0xC2, 0x77,
  0x89, 0x2D, 0x1E, 0xF5, 0x35, 0xB6, 0xD3, 0xBF, 0x03, 0x5E, 0x30, 0xCF, 0xAE, 0xC9, 0x09, 0x79, 0x62, 0x14, 0x6F, 0x52,
  0x95, 0x60, 0x71, 0xA2, 0xA7, 0x08, 0x25, 0x7D, 0x53, 0xDB, 0xC9, 0x2A, 0x68, 0x93, 0xAD, 0x78, 0x01, 0xB9, 0x38, 0x31,
  0xAA, 0x3E, 0x87, 0xAB, 0xA0, 0x44, 0xE2, 0x73, 0xE1, 0xBA, 0x8C, 0x19, 0x97, 0x42, 0x3D, 0x17, 0xF6, 0x4D, 0x3C, 0xA7,
  0x37, 0xBC, 0x1B, 0x0C, 0xC1, 0x70, 0x4F, 0x4E, 0xAD, 0xCE, 0x18, 0xE7, 0x80, 0x46, 0xC6, 0x14, 0x38, 0x6B, 0x77, 0x89,
  0xC3, 0x9D, 0x14, 0x33, 0xDC, 0x5B, 0xBD, 0xAD, 0x4F, 0xA2, 0x22, 0x2D, 0x24, 0x7A, 0xBA, 0x80, 0xE1, 0x65, 0xF9, 0x27,
  0xC9, 0xE5, 0xE9, 0x85, 0x49, 0xB0, 0x66, 0x27, 0x35, 0xD9, 0x46, 0x5C, 0x86, 0x07, 0x78, 0x37, 0x5F, 0x9C, 0xCC, 0x48,
  0x76, 0xAA, 0x1E, 0x10, 0xC0, 0x6B, 0x97, 0x98, 0xE1, 0x03, 0x1A, 0x99, 0x0D, 0xE3, 0x98, 0xF1, 0xA1, 0x03, 0x88, 0x16,
  0xC2, 0x62, 0xCB, 0xBC, 0x74, 0x42, 0x02, 0xC4, 0x7D, 0x7E, 0x06, 0x66, 0x69, 0xBE, 0x3F, 0x30, 0x83, 0x38, 0x6E, 0x9B,
  0xBB, 0x58, 0x19, 0x82, 0xC6, 0x76, 0x0A, 0xEF, 0xD1, 0xE4, 0x9D, 0xC4, 0x4C, 0x48, 0xDC, 0xF1, 0xB1, 0x5A, 0xE3, 0xB7,
  0x13, 0xAE, 0xAB, 0xD6, 0x77, 0x5E, 0xDE, 0x17, 0xE0, 0xDD, 0x50, 0xE4, 0xAE, 0xE3, 0xE7, 0x3B, 0xFE, 0x58, 0x58, 0x8B,
  0x6E, 0x0D, 0xBF, 0x50, 0xE5, 0xEC, 0x9E, 0x01, 0x2F, 0xCF, 0x7C, 0xB0, 0x4A, 0x24, 0xFE, 0x2B, 0x36, 0x15, 0xC8, 0x2D,
  0xC4, 0x72, 0x4C, 0x58, 0xF7, 0xD8, 0xC8, 0x94, 0xB0, 0x96, 0x6D, 0x60, 0x14, 0xA9, 0xE6, 0xAE, 0x1F, 0x4D, 0x3C, 0x1A,
  0x98, 0x6F, 0xEB, 0xB3, 0x27, 0xA5, 0x39, 0xF0, 0x19, 0x91, 0xC9, 0xDA, 0xE6, 0xC6, 0x56, 0xE0, 0xA4, 0x7F, 0xDE, 0x12,
  0x95, 0xA1, 0x1A, 0x35, 0x29, 0x89, 0x3A, 0x4C, 0x13, 0x85, 0x28, 0xCC, 0x9C, 0xA4, 0x00, 0x85, 0x4E, 0x62, 0x33, 0xD7,
  0xAE, 0xDD, 0xAA, 0x04, 0x70, 0xD6, 0xCE, 0xDC, 0xA4, 0x1B, 0xB2, 0x2D, 0xB9, 0x4B, 0xB8, 0x8C, 0xB8, 0x04, 0x5B, 0xEF,
  0x27, 0x8A, 0x25, 0xB9, 0xF7, 0xB6, 0xE3, 0x01, 0x66, 0x8B, 0x4F, 0xDA, 0x6C, 0xA8, 0x88, 0xBE, 0xA6, 0xE8, 0x1E, 0x38,
  0xD1, 0x55, 0x0D, 0x7F, 0x27, 0x24, 0x0A, 0xAF, 0x29, 0x91, 0x4B, 0x2E, 0xE6, 0xBF, 0x63, 0xAF, 0xD6, 0x7A, 0xC0, 0xA6,
  0x5A, 0xA3, 0xBB, 0x16, 0x7E, 0xFB, 0xF2, 0x00, 0xD3, 0x00, 0x75, 0xA3, 0x14, 0x02, 0x68, 0x52, 0x3C, 0x63, 0x67, 0x92,
  0x33, 0x58, 0x45, 0x2F, 0x24, 0x5E, 0x91, 0x41, 0xDC, 0x6C, 0x9A, 0xCC, 0xE1, 0x82, 0x48, 0xF6, 0xA1, 0xD9, 0x88, 0x25,
  0x12, 0x2D, 0x26, 0xE3, 0xF5, 0xC5, 0x05, 0xCC, 0x72, 0xDC, 0x2B, 0x6A, 0xA7, 0x11, 0xC3, 0x21, 0x75, 0xB5, 0x30, 0x9F,
  0x88, 0xD7, 0x01, 0xE8, 0x3B, 0x03, 0xF3, 0x16, 0x01, 0xBF, 0xEA, 0x9A, 0x52, 0x5B, 0x59, 0xE9, 0xD9, 0x74, 0x09, 0xE4,
  0x89, 0xE4, 0x71, 0x0A, 0xA7, 0x1C, 0xE6, 0xDC, 0x5D, 0x2F, 0xC9, 0x8D, 0x02, 0xEF, 0xC8, 0x10, 0x64, 0xAA, 0xEA, 0x0D,
  0x11, 0x67, 0xB0, 0x49, 0x5B, 0xCD, 0xE0, 0xE9, 0x82, 0x2D, 0x69, 0x6C, 0x5B, 0x4D, 0x3B, 0x33, 0x9F, 0x63, 0x57, 0x11,
  0x1E, 0xE6, 0xBF, 0x5E, 0x91, 0xA8, 0xA2, 0xE3, 0xEA, 0x50, 0x30, 0x68, 0xCE, 0xDD, 0x72, 0xEA, 0xB2, 0xDE, 0x93, 0x33,
  0x76, 0x19, 0x87, 0x83, 0xCC, 0xE4, 0x27, 0x40, 0x06, 0xE6, 0x05, 0x1F, 0xB8, 0xF5, 0xF3, 0x86, 0x03, 0x07, 0x05, 0x63,
  0xAB, 0x5D, 0xEB, 0xB6, 0x46, 0x4F, 0xA5, 0x57, 0x31, 0xE5, 0x74, 0x3D, 0x40, 0x78, 0x59, 0xF4, 0xEB, 0xD5, 0xA4, 0xB9,
  0xED, 0xB0, 0xD6, 0x76, 0x3B, 0x4C, 0xDB, 0x64, 0xBC, 0xCA, 0xB4, 0x6B, 0x41, 0xAF, 0xCF, 0x9A, 0xBE, 0x56, 0xA3, 0x44,
  0xE2, 0x13, 0x80, 0x8B, 0x04, 0x8F, 0x92, 0x58, 0x03, 0x3E, 0x59, 0xE5, 0x19, 0x8C, 0xE1, 0x51, 0x22, 0x17, 0x8A, 0x21,
  0xD4, 0xFC, 0x73, 0x41, 0x79, 0xFA, 0x4F, 0x40, 0x76, 0x61, 0x0C, 0xDD, 0xEB, 0x45, 0x13, 0x2E, 0x26, 0xA3, 0x4D, 0x6E,
  0xF4, 0x18, 0x7E, 0xEF, 0x82, 0x32, 0x4A, 0xDC, 0x86, 0xF3, 0x1D, 0xFA, 0xED, 0x00, 0x25, 0x91, 0x48, 0x24, 0x08, 0xA0,
  0xDF, 0xFC, 0x4A, 0x5C, 0x80, 0x8D, 0x7F, 0x35, 0x31, 0xC1, 0x20, 0x0F, 0xE0, 0x66, 0x74, 0x0E, 0x20, 0xCB, 0x9B, 0xDD,
  0x18, 0xFF, 0x28, 0x9F, 0x58, 0x82, 0x0C, 0x41, 0x37, 0x23, 0x0F, 0x20, 0x91, 0x48, 0xDC, 0x88, 0x6F, 0x06, 0x9D, 0xEF,
  0xCC, 0x7A, 0xE7, 0x4F, 0x38, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
};

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
  Image font_image = LoadImageFromMemory(".png", charmap_oldschool_white_png, sizeof(charmap_oldschool_white_png));
  ImageFormat(&font_image, PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA);
  ImageColorReplace(&font_image, BLACK, BLANK);

  Texture font_texture = LoadTextureFromImage(font_image);

  UnloadImage(font_image);

  fill_glyphs_array();

  return font_texture;
}

size_t tile_to_glyph(LevelTile tile) {
  switch (tile) {
  case TILE_NONE: return ' ';
  case TILE_WALL: return '#';
  case TILE_VERTICAL_CLOSED_DOOR: return '|';
  case TILE_HORIZONTAL_CLOSED_DOOR: return '-';
  case TILE_FLOOR: return SPECIAL_GLYPH_CENTERED_DOT;
  case TILE_VERTICAL_OPENED_DOOR: return '_';
  case TILE_HORIZONTAL_OPENED_DOOR: return '|';
  case LEVEL_TILE_COUNT: assert(false && "bruh");
  }

  assert(false && "unknown tile");
}

size_t object_type_to_glyph(LevelObjectType object_type) {
  switch (object_type) {
  case OBJECT_NONE: assert(false && "skill issue");
  case OBJECT_ELEVATOR_DOWN: return '>';
  case OBJECT_ELEVATOR_UP: return '<';
  case LEVEL_OBJECT_COUNT: assert(false && "nuh uh");
  }

  assert(false && "unknown tile");
}
