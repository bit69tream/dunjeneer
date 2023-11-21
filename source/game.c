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

  Shader cursor_shader = LoadShaderFromMemory(NULL,
                                              "#version 330\n\n"

                                              "out vec4 finalColor;\n\n"

                                              "uniform sampler2D texture0;\n"
                                              "uniform vec4 colDiffuse;\n\n"

                                              "in vec4 fragColor;\n"
                                              "in vec2 fragTexCoord;\n\n"

                                              "void main() {\n"
                                              "    vec4 texelColor = texture(texture0, fragTexCoord)*colDiffuse*fragColor;\n"
                                              "    finalColor = vec4(1.0 - texelColor.r, 1.0 - texelColor.g, 1.0 - texelColor.b, 1.0);"
                                              "}\n");

  assert(IsShaderReady(cursor_shader));

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

#define X_TO_SCREEN(x, type) ((type)((x) * GLYPH_WIDTH * SCALING_FACTOR) + (type)((GLYPH_GAP * (x)) + GLYPH_GAP))
#define Y_TO_SCREEN(x, type) ((type)((x) * GLYPH_HEIGHT * SCALING_FACTOR) + (type)((GLYPH_GAP * (x)) + GLYPH_GAP))

  player_location.x = X_TO_SCREEN(player_location.x, size_t);
  player_location.y = X_TO_SCREEN(player_location.y, size_t);

  Camera2D camera = {0};

  camera.rotation = 0;
  camera.zoom = 4;

  /* TODO: blocky or fast movement + smooth camera */

  #define MOVEMENT_DELTA_MAX 30
  #define MOVEMENT_DELTA_STEP 30

  ssize_t x_delta = 0;
  ssize_t y_delta = 0;

  camera.target.x = X_TO_SCREEN(player_location.x, float) + (GLYPH_WIDTH / 2.0f);
  camera.target.y = Y_TO_SCREEN(player_location.y, float) + (GLYPH_HEIGHT / 2.0f);

  /* TODO: https://www.squidi.net/three/entry.php?id=83 */

  while (!WindowShouldClose()) {

    if (IsKeyDown(KEY_E)) {
      if (y_delta > 0) {
        y_delta = 0;
      }
      /* player_location.y -= 1; */
      y_delta -= MOVEMENT_DELTA_STEP;
    }

    if (IsKeyDown(KEY_S)) {
      if (x_delta > 0) {
        x_delta = 0;
      }
      /* player_location.x -= 1; */
      x_delta -= MOVEMENT_DELTA_STEP;
    }

    if (IsKeyDown(KEY_D)) {
      if (y_delta < 0) {
        y_delta = 0;
      }
      /* player_location.y += 1; */
      y_delta += MOVEMENT_DELTA_STEP;
    }

    if (IsKeyDown(KEY_F)) {
      if (x_delta < 0) {
        x_delta = 0;
      }
      /* player_location.x += 1; */
      x_delta += MOVEMENT_DELTA_STEP;
    }

    if (x_delta != 0 && x_delta % MOVEMENT_DELTA_MAX == 0) {
      player_location.x = (size_t)((ssize_t)player_location.x + (x_delta / MOVEMENT_DELTA_MAX));
      x_delta = 0;
    }

    if (y_delta != 0 && y_delta % MOVEMENT_DELTA_MAX == 0) {
      player_location.y = (size_t)((ssize_t)player_location.y + (y_delta / MOVEMENT_DELTA_MAX));
      y_delta = 0;
    }

    /* player_location.x = CLAMP(1, LEVEL_WIDTH, player_location.x); */
    /* player_location.y = CLAMP(1, LEVEL_HEIGHT, player_location.y); */

    player_location.x = CLAMP(1, X_TO_SCREEN(LEVEL_WIDTH, size_t), player_location.x);
    player_location.y = CLAMP(1, Y_TO_SCREEN(LEVEL_HEIGHT, size_t), player_location.y);

    /* float player_screen_x = X_TO_SCREEN(player_location.x, float) + (GLYPH_WIDTH / 2.0f); */
    /* float player_screen_y = Y_TO_SCREEN(player_location.y, float) + (GLYPH_HEIGHT / 2.0f); */

    float player_screen_x = (float)player_location.x + (GLYPH_WIDTH / 2.0f);
    float player_screen_y = (float)player_location.y + (GLYPH_HEIGHT / 2.0f);

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
                         .width = GLYPH_WIDTH * SCALING_FACTOR,
                         .height = GLYPH_HEIGHT * SCALING_FACTOR,
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
                     /* .x = X_TO_SCREEN(player_location.x, float), */
                     /* .y = Y_TO_SCREEN(player_location.y, float), */
                     .x = (float)player_location.x,
                     .y = (float)player_location.y,
                     .width = GLYPH_WIDTH * SCALING_FACTOR,
                     .height = GLYPH_HEIGHT * SCALING_FACTOR,
                   },
                   (Vector2) {0, 0},
                   0,
                   GREEN);

    Vector2 mouse_in_world = GetScreenToWorld2D(GetMousePosition(), camera);

    /* TODO: нужно зарендерить всё в текстуру, а потом рисовать курсор полностью в шейдере */
    BeginShaderMode(cursor_shader);

    DrawRectanglePro((Rectangle) {
        .x = mouse_in_world.x,
        .y = mouse_in_world.y,
        .width = GLYPH_WIDTH,
        .height = GLYPH_HEIGHT,
      },
      (Vector2) {GLYPH_WIDTH / 2.0f, GLYPH_HEIGHT / 2.0f},
      0,
      BLANK);

    EndShaderMode();

    EndMode2D();

    EndDrawing();
  }

  UnloadTexture(font);

  CloseWindow();

  return 0;
}
