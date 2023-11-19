#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <raylib.h>

#include "rand.h"
#include "level_generator.h"
#include "utils.h"
#include "font.h"

int main(void) {
  srand((unsigned int)time(0));

  static LevelMap map = {0};
  generate_level(&map);

  #define SCALING_FACTOR 2

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  /* TODO: adapt ui to the window size, tell user that window is too small when it is */
  /* TODO: calculate the minimal window size when the ui is ready */
  InitWindow(800, 600, "dunjeneer");
  SetExitKey(KEY_NULL);

  SetTargetFPS(60);

  Texture2D font = font_texture();

  Color tile_colors[] = {
    [TILE_NONE] = BLANK,
    [TILE_WALL] = WHITE,
    [TILE_FLOOR] = GRAY,
    [TILE_DOOR] = BROWN,
  };

  while (!WindowShouldClose()) {

    BeginDrawing();

    ClearBackground(BLACK);

    for (size_t y = 0; y < LEVEL_HEIGHT; y++) {
      for (size_t x = 0; x < LEVEL_WIDTH; x++) {
        LevelTile tile = map[y][x];

        DrawTexturePro(font,
                       glyphs[tile_to_glyph(tile)],
                       (Rectangle) {
                         .x = (float)(x * GLYPH_WIDTH * SCALING_FACTOR) + (float)((GLYPH_GAP * x) + GLYPH_GAP),
                         .y = (float)(y * GLYPH_HEIGHT * SCALING_FACTOR) +  + (float)((GLYPH_GAP * y) + GLYPH_GAP),
                         .width = GLYPH_WIDTH * SCALING_FACTOR,
                         .height = GLYPH_HEIGHT * SCALING_FACTOR,
                       },
                       (Vector2) {0, 0},
                       0,
                       tile_colors[tile]);
      }
    }

    EndDrawing();
  }

  UnloadTexture(font);

  CloseWindow();

  return 0;
}
