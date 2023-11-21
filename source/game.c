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

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  /* TODO: adapt ui to the window size, tell user that window is too small when it is */
  /* TODO: calculate the minimal window size when the ui is ready */
  InitWindow(800, 600, "dunjeneer");
  SetExitKey(KEY_NULL);

  HideCursor();

  SetTargetFPS(60);

  Texture2D font = font_texture();

  Color tile_colors[] = {
    [TILE_NONE] = BLANK,
    [TILE_WALL] = WHITE,
    [TILE_FLOOR] = DARKGRAY,
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

#define X_TO_SCREEN(x, type) ((type)((x) * GLYPH_WIDTH) + (type)((GLYPH_GAP * (x)) + GLYPH_GAP))
#define Y_TO_SCREEN(x, type) ((type)((x) * GLYPH_HEIGHT) + (type)((GLYPH_GAP * (x)) + GLYPH_GAP))

  Camera2D camera = {0};

  camera.rotation = 0;
  camera.zoom = 4;

  /* TODO: blocky or fast movement + smooth camera */

  camera.target.x = X_TO_SCREEN(player_location.x, float) + (GLYPH_WIDTH / 2.0f);
  camera.target.y = Y_TO_SCREEN(player_location.y, float) + (GLYPH_HEIGHT / 2.0f);

  /* TODO: https://www.squidi.net/three/entry.php?id=83 */

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

    player_location.x = CLAMP(1, LEVEL_WIDTH, player_location.x);
    player_location.y = CLAMP(1, LEVEL_HEIGHT, player_location.y);

    float player_screen_x = X_TO_SCREEN(player_location.x, float) + (GLYPH_WIDTH / 2.0f);
    float player_screen_y = Y_TO_SCREEN(player_location.y, float) + (GLYPH_HEIGHT / 2.0f);

    camera.target.x = LERP(camera.target.x, player_screen_x, 0.1f);
    camera.target.y = LERP(camera.target.y, player_screen_y, 0.1f);

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
                         .x = X_TO_SCREEN(x, float),
                         .y = Y_TO_SCREEN(y, float),
                         .width = GLYPH_WIDTH,
                         .height = GLYPH_HEIGHT,
                       },
                       (Vector2) {0, 0},
                       0,
                       tile_colors[tile]);
      }
    }

    for (size_t i = 0; i < objects_len; i++) {
      DrawRectangle(X_TO_SCREEN(objects[i].location.x, int),
                    Y_TO_SCREEN(objects[i].location.y, int),
                    GLYPH_WIDTH,
                    GLYPH_HEIGHT,
                    BLACK);

      DrawTexturePro(font,
                     glyphs[object_type_to_glyph(objects[i].type)],
                     (Rectangle) {
                         .x = X_TO_SCREEN(objects[i].location.x, float),
                         .y = Y_TO_SCREEN(objects[i].location.y, float),
                         .width = GLYPH_WIDTH,
                         .height = GLYPH_HEIGHT,
                     },
                     (Vector2) {0, 0},
                     0,
                     object_colors[objects[i].type]);
    }

    DrawRectanglePro((Rectangle) {
                     .x = X_TO_SCREEN(player_location.x, float) - GLYPH_GAP,
                     .y = Y_TO_SCREEN(player_location.y, float) - GLYPH_GAP,
                     .width = (GLYPH_WIDTH + (GLYPH_GAP * 2)),
                     .height = (GLYPH_HEIGHT + (GLYPH_GAP * 2)),
                   },
                   (Vector2) {0, 0},
                   0,
                   GREEN);

    DrawTexturePro(font,
                   glyphs['@'],
                   (Rectangle) {
                     .x = X_TO_SCREEN(player_location.x, float),
                     .y = Y_TO_SCREEN(player_location.y, float),
                     .width = GLYPH_WIDTH,
                     .height = GLYPH_HEIGHT,
                   },
                   (Vector2) {0, 0},
                   0,
                   BLACK);

    Vector2 mouse_on_screen = GetScreenToWorld2D(GetMousePosition(), camera);

    Point mouse_in_world = {
      .x = (size_t)((mouse_on_screen.x) / (GLYPH_WIDTH + 1)),
      .y = (size_t)((mouse_on_screen.y) / (GLYPH_HEIGHT + 1)),
    };

    /* TODO: нужно зарендерить всё в текстуру, а потом рисовать курсор полностью в шейдере */

    DrawRectanglePro((Rectangle) {
        .x = X_TO_SCREEN(mouse_in_world.x, float) - GLYPH_GAP,
        .y = Y_TO_SCREEN(mouse_in_world.y, float) - GLYPH_GAP,
        .width = GLYPH_WIDTH + (GLYPH_GAP * 2),
        .height = GLYPH_HEIGHT + (GLYPH_GAP * 2),
      },
      (Vector2) {0, 0},
      0,
      WHITE);

    EndMode2D();

    EndDrawing();
  }

  UnloadTexture(font);

  CloseWindow();

  return 0;
}
