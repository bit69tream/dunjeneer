#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include <raylib.h>

#include "rand.h"
#include "level_generator.h"
#include "utils.h"
#include "font.h"

int main(void) {
  srand((unsigned int)time(0));
  /* srand(1); */

  static LevelMap map = {0};

  static LevelObject objects[OBJECTS_MAX] = {0};
  size_t objects_len = 0;

  Point player_location = {0};

  generate_level(&map, objects, &objects_len, &player_location);

  #define SCALING_FACTOR 1

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
    [TILE_VERTICAL_CLOSED_DOOR] = BROWN,
    [TILE_HORIZONTAL_CLOSED_DOOR] = BROWN,
  };

  static_assert(SIZE_OF(tile_colors) == LEVEL_TILE_COUNT);

  Color object_colors[] = {
    [OBJECT_NONE] = BLANK,
    [OBJECT_ELEVATOR_DOWN] = WHITE,
    [OBJECT_ELEVATOR_UP] = WHITE,
  };

  static_assert(SIZE_OF(object_colors) == LEVEL_OBJECT_COUNT);

  /* RenderTexture2D t = LoadRenderTexture(LEVEL_WIDTH * GLYPH_WIDTH * SCALING_FACTOR + (GLYPH_GAP * (LEVEL_WIDTH + 1)), */
  /*                                       LEVEL_HEIGHT * GLYPH_HEIGHT * SCALING_FACTOR + (GLYPH_GAP * (LEVEL_HEIGHT + 1))); */
  /* BeginTextureMode(t); */

  /* ClearBackground(BLACK); */

  /* for (size_t y = 0; y < LEVEL_HEIGHT; y++) { */
  /*   for (size_t x = 0; x < LEVEL_WIDTH; x++) { */
  /*     LevelTile tile = map[y][x]; */

  /*     DrawTexturePro(font, */
  /*                    glyphs[tile_to_glyph(tile)], */
  /*                    (Rectangle) { */
  /*                      .x = (float)(x * GLYPH_WIDTH * SCALING_FACTOR) + (float)((GLYPH_GAP * x) + GLYPH_GAP), */
  /*                      .y = (float)(y * GLYPH_HEIGHT * SCALING_FACTOR) + (float)((GLYPH_GAP * y) + GLYPH_GAP), */
  /*                      .width = GLYPH_WIDTH * SCALING_FACTOR, */
  /*                      .height = GLYPH_HEIGHT * SCALING_FACTOR, */
  /*                    }, */
  /*                    (Vector2) {0, 0}, */
  /*                    0, */
  /*                    tile_colors[tile]); */
  /*   } */
  /* } */

  /* EndTextureMode(); */

  /* ExportImage(LoadImageFromTexture(t.texture), "level.png"); */

#define TO_SCREEN(x, type) ((type)((x) * GLYPH_WIDTH * SCALING_FACTOR) + (type)((GLYPH_GAP * (x)) + GLYPH_GAP))

  Camera2D camera = {0};

  camera.rotation = 0;
  camera.zoom = 5;

  /* TODO: make player movements a little slower, but still crisp and responsive */

  while (!WindowShouldClose()) {

    if (IsKeyDown(KEY_E)) {
      player_location.y -= 1;
    }

    if (IsKeyDown(KEY_S)) {
      player_location.x -= 1;
    }

    if (IsKeyDown(KEY_D)) {
      player_location.y += 1;
    }

    if (IsKeyDown(KEY_F)) {
      player_location.x += 1;
    }

    player_location.x = CLAMP(0, LEVEL_WIDTH, player_location.x);
    player_location.y = CLAMP(0, LEVEL_HEIGHT, player_location.y);

    camera.target.x = TO_SCREEN(player_location.x, float) + (GLYPH_WIDTH / 2.0f);
    camera.target.y = TO_SCREEN(player_location.y, float) + (GLYPH_HEIGHT / 2.0f);

    camera.offset.x = (float)GetScreenWidth() / 2.0f;
    camera.offset.y = (float)GetScreenHeight() / 2.0f;

    BeginDrawing();

    ClearBackground(BLACK);

    BeginMode2D(camera);

    for (size_t y = 0; y < LEVEL_HEIGHT; y++) {
      for (size_t x = 0; x < LEVEL_WIDTH; x++) {
        LevelTile tile = map[y][x];

        DrawTexturePro(font,
                       glyphs[tile_to_glyph(tile)],
                       (Rectangle) {
                         .x = TO_SCREEN(x, float),
                         .y = TO_SCREEN(y, float),
                         .width = GLYPH_WIDTH * SCALING_FACTOR,
                         .height = GLYPH_HEIGHT * SCALING_FACTOR,
                       },
                       (Vector2) {0, 0},
                       0,
                       tile_colors[tile]);
      }
    }

    for (size_t i = 0; i < objects_len; i++) {
      DrawRectangle(TO_SCREEN(objects[i].location.x, int),
                    TO_SCREEN(objects[i].location.y, int),
                    GLYPH_WIDTH,
                    GLYPH_HEIGHT,
                    BLACK);

      DrawTexturePro(font,
                     glyphs[object_type_to_glyph(objects[i].type)],
                     (Rectangle) {
                         .x = TO_SCREEN(objects[i].location.x, float),
                         .y = TO_SCREEN(objects[i].location.y, float),
                         .width = GLYPH_WIDTH * SCALING_FACTOR,
                         .height = GLYPH_HEIGHT * SCALING_FACTOR,
                     },
                     (Vector2) {0, 0},
                     0,
                     object_colors[objects[i].type]);
    }

    DrawTexturePro(font,
                   glyphs['@'],
                   (Rectangle) {
                     .x = TO_SCREEN(player_location.x, float),
                     .y = TO_SCREEN(player_location.y, float),
                     .width = GLYPH_WIDTH * SCALING_FACTOR,
                     .height = GLYPH_HEIGHT * SCALING_FACTOR,
                   },
                   (Vector2) {0, 0},
                   0,
                   GREEN);

    EndMode2D();

    EndDrawing();
  }

  UnloadTexture(font);

  CloseWindow();

  return 0;
}
