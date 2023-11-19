#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <raylib.h>

#include "rand.h"
#include "dungeon_generation.h"
#include "utils.h"
#include "game.h"

int main(void) {
  srand((unsigned int)time(0));

  Room rooms[ROOMS_MAX] = {0};
  size_t rooms_len = 0;

  generate_rooms(rooms, &rooms_len);

  #define CELL_SIZE 10

  /* SetConfigFlags(FLAG_WINDOW_RESIZABLE); */
  InitWindow((LEVEL_WIDTH + 2) * CELL_SIZE, (LEVEL_HEIGHT + 2) * CELL_SIZE, "dunjeneer");
  SetExitKey(KEY_NULL);

  SetTargetFPS(60);

  RenderTexture2D level = LoadRenderTexture(LEVEL_WIDTH * CELL_SIZE, LEVEL_HEIGHT * CELL_SIZE);

  BeginTextureMode(level); {
    ClearBackground(BLACK);

    /* for (size_t i = 0; i < leafs_len; i++) { */
    /*   if (leafs[i].left_leaf != 0 && leafs[i].right_leaf != 0) continue; */

    /*   DrawRectangle((int)(leafs[i].rect.x * CELL_SIZE), */
    /*                 (int)(leafs[i].rect.y * CELL_SIZE), */
    /*                 (int)(leafs[i].rect.width * CELL_SIZE), */
    /*                 (int)(leafs[i].rect.height * CELL_SIZE), */
    /*                 (CLITERAL(Color){ */
    /*                   (unsigned char)rand_range(100, 255), */
    /*                   (unsigned char)rand_range(100, 255), */
    /*                   (unsigned char)rand_range(100, 255), */
    /*                   (unsigned char)rand_range(100, 255), */
    /*                 })); */
    /* } */

    for (size_t i = 0; i < rooms_len; i++) {
      DrawRectangle((int)(rooms[i].x * CELL_SIZE),
                    (int)(rooms[i].y * CELL_SIZE),
                    (int)(rooms[i].width * CELL_SIZE),
                    (int)(rooms[i].height * CELL_SIZE),
                    WHITE);
    }
  } EndTextureMode();

  while (!WindowShouldClose())
    {
      BeginDrawing();

      ClearBackground(BLACK);

      DrawTexture(level.texture, CELL_SIZE, CELL_SIZE, WHITE);

      EndDrawing();
    }

  CloseWindow();


  return 0;
}
