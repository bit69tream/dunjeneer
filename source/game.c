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

  #define SCALING_FACTOR 1.5

  /* SetConfigFlags(FLAG_WINDOW_RESIZABLE); */
  InitWindow((int)(LEVEL_WIDTH * GLYPH_WIDTH), (int)(LEVEL_HEIGHT * GLYPH_HEIGHT), "dunjeneer");
  SetExitKey(KEY_NULL);

  SetTargetFPS(60);

  RenderTexture2D level = LoadRenderTexture(LEVEL_WIDTH * GLYPH_WIDTH, LEVEL_HEIGHT * GLYPH_HEIGHT);

  Texture2D font = font_texture();

  Color tile_colors[] = {
    [TILE_NONE] = BLANK,
    [TILE_WALL] = WHITE,
    [TILE_FLOOR] = LIGHTGRAY,
    [TILE_DOOR] = BROWN,
  };

  BeginTextureMode(level); {
    ClearBackground(BLACK);

      for (size_t y = 0; y < LEVEL_HEIGHT; y++) {
        for (size_t x = 0; x < LEVEL_WIDTH; x++) {
          LevelTile tile = map[y][x];

          DrawTextureRec(font,
                         glyphs[tile_to_glyph(tile)],
                         (Vector2) {
                           .x = (float)(x * GLYPH_WIDTH),
                           .y = (float)(y * GLYPH_HEIGHT),
                         },
                         tile_colors[tile]);
        }
      }

  } EndTextureMode();

  while (!WindowShouldClose())
    {
      BeginDrawing();

      ClearBackground(BLACK);

      float height = (float)GetScreenHeight();
      float scaling_factor = height / (float)level.texture.height;

      float width = (float) level.texture.width * scaling_factor;

      float x = ((float)GetScreenWidth() / 2) - (width / 2);

      DrawTexturePro(level.texture,
                     (Rectangle) {
                       .x = 0,
                       .y = 0,
                       .width = (float)level.texture.width,
                       .height = (float)-level.texture.height,
                     },
                     (Rectangle) {
                       .x = x,
                       .y = 0,
                       .width = width,
                       .height = height,
                     },
                     (Vector2){0, 0},
                     0,
                     WHITE);

      /* DrawTexturePro(font, */
      /*                glyphs['A'], */
      /*                (Rectangle) {.x = 0, .y = 0, .width = 500, .height = 700}, */
      /*                (Vector2) {0, 0}, */
      /*                0, */
      /*                BLACK); */

      /* DrawTexture(font, 0, 0, BLACK); */

      EndDrawing();
    }

  CloseWindow();


  return 0;
}
