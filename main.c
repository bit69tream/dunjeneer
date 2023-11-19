#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <raylib.h>

#define LEVEL_WIDTH 90
#define LEVEL_HEIGHT 60

#define LEAFS_MAX 64
#define MIN_LEAF_SIZE 8

#define ROOMS_MAX LEAFS_MAX

typedef Rectangle Room;

typedef struct {
  Rectangle rect;
  size_t left_leaf;
  size_t right_leaf;
  size_t parent;
} Leaf;

bool roll_dice(int n) {
  return (rand() % n) == 0;
}

int rand_range(int a, int b) {
  assert(a < b && "a must be less than b");

  int n = b - a;
  return (rand() % (n + 1)) - abs(a);
}

float frand_range(float a, float b) {
  assert(a < b && "a must be less than b");
  assert((a < 1.0 && b <= 1.0) &&
         (a >= 0.0 && b > 0.0) &&
         "a must be in range [0, 1), b must be in range (0, 1]");

  int bb = (int)(b * 100.0f);
  int aa = (int)(a * 100.0f);
  int n = (int)(bb - aa);
  return (float)((rand() % (n + 1)) + aa) / 100.0f;
}

#define PUSH(arr, len, cap, dest_index)         \
  do {                                          \
    assert((*(len)) < (cap));                   \
                                                \
    *(dest_index) = (*(len))++;                 \
  } while (0)

bool generate_leafs(Leaf *leafs, size_t *leafs_len, size_t leaf_index) {
  float where_to_split = frand_range(0.4f, 0.6f);

  if (((int)(leafs[leaf_index].rect.height * where_to_split) <= MIN_LEAF_SIZE) ||
      ((int)(leafs[leaf_index].rect.width * where_to_split) <= MIN_LEAF_SIZE)) {
    return false;
  }

  size_t new_left_leaf = 0;
  size_t new_right_leaf = 0;

  if (roll_dice(2)) {
    // horizontal split
    int split_y = (int)(leafs[leaf_index].rect.height * where_to_split);

    /* first leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = leafs[leaf_index].rect.x;
      leafs[new_leaf].rect.y = leafs[leaf_index].rect.y;
      leafs[new_leaf].rect.width = leafs[leaf_index].rect.width;
      leafs[new_leaf].rect.height = (float)split_y;
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = leaf_index;

      new_left_leaf = new_leaf;
    }

    /* second leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = leafs[leaf_index].rect.x;
      leafs[new_leaf].rect.y = leafs[leaf_index].rect.y + (float)split_y;
      leafs[new_leaf].rect.width = leafs[leaf_index].rect.width;
      leafs[new_leaf].rect.height = (float)((int)leafs[leaf_index].rect.height - split_y);
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = leaf_index;

      new_right_leaf = new_leaf;
    }
  } else {
    // vertical split
    int split_x = (int)(leafs[leaf_index].rect.width * where_to_split);

    /* first leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = leafs[leaf_index].rect.x;
      leafs[new_leaf].rect.y = leafs[leaf_index].rect.y;
      leafs[new_leaf].rect.width = (float)split_x;
      leafs[new_leaf].rect.height = leafs[leaf_index].rect.height;
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = leaf_index;

      new_left_leaf = new_leaf;
    }

    /* second leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = leafs[leaf_index].rect.x + (float)split_x;
      leafs[new_leaf].rect.y = leafs[leaf_index].rect.y;
      leafs[new_leaf].rect.width = (float)((int)leafs[leaf_index].rect.width - split_x);
      leafs[new_leaf].rect.height = leafs[leaf_index].rect.height;
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = leaf_index;

      new_right_leaf = new_leaf;
    }
  }

  leafs[leaf_index].left_leaf = new_left_leaf;
  leafs[leaf_index].right_leaf = new_right_leaf;

  return true;
}

bool generate_leafs_until_unable_to(Leaf *leafs, size_t *leafs_len, size_t leaf_index) {
  if (!generate_leafs(leafs, leafs_len, leaf_index)) {
    return false;
  }

  if (!generate_leafs_until_unable_to(leafs, leafs_len, leafs[leaf_index].left_leaf) &&
      !generate_leafs_until_unable_to(leafs, leafs_len, leafs[leaf_index].right_leaf)) {
    return false;
  }

  return true;
}

int main(void) {
  Leaf leafs[LEAFS_MAX] = {0};
  size_t leafs_len = 0;

  srand((unsigned int)time(0));

  float where_to_split = frand_range(0.4f, 0.6f);

  if (roll_dice(2)) {
    // horizontal split
    int split_y = (int)(LEVEL_HEIGHT * where_to_split);

    /* first leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, &leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = 0;
      leafs[new_leaf].rect.y = 0;
      leafs[new_leaf].rect.width = LEVEL_WIDTH;
      leafs[new_leaf].rect.height = (float)split_y;
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = 0;
    }

    /* second leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, &leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = 0;
      leafs[new_leaf].rect.y = (float)split_y;
      leafs[new_leaf].rect.width = LEVEL_WIDTH;
      leafs[new_leaf].rect.height = (float)(LEVEL_HEIGHT - split_y);
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = 0;
    }
  } else {
    // vertical split
    int split_x = (int)(LEVEL_WIDTH * where_to_split);

    /* first leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, &leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = 0;
      leafs[new_leaf].rect.y = 0;
      leafs[new_leaf].rect.width = (float)split_x;
      leafs[new_leaf].rect.height = LEVEL_HEIGHT;
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = 0;
    }

    /* second leaf */
    {
      size_t new_leaf = 0;
      PUSH(leafs, &leafs_len, LEAFS_MAX, &new_leaf);

      leafs[new_leaf].rect.x = (float)split_x;
      leafs[new_leaf].rect.y = 0;
      leafs[new_leaf].rect.width = (float)(LEVEL_WIDTH - split_x);
      leafs[new_leaf].rect.height = LEVEL_HEIGHT;
      leafs[new_leaf].left_leaf = 0;
      leafs[new_leaf].right_leaf = 0;
      leafs[new_leaf].parent = 0;
    }
  }

  assert(leafs_len == 2);
  while (true) {
    generate_leafs_until_unable_to(leafs, &leafs_len, 0);
    generate_leafs_until_unable_to(leafs, &leafs_len, 1);

    size_t rooms_counter = 0;
    for (size_t i = 0; i < leafs_len; i++) {
      if (leafs[i].left_leaf == 0 || leafs[i].right_leaf == 0) {
        rooms_counter ++;
      }
    }

    if (rooms_counter < 6) {
      leafs_len = 2;
      continue;
    }

    break;
  }

  Room rooms[ROOMS_MAX] = {0};
  size_t rooms_len = 0;

  for (size_t i = 0; i < leafs_len; i++) {
    if (leafs[i].left_leaf != 0 && leafs[i].right_leaf != 0) continue;

    size_t new_room = 0;
    PUSH(rooms, &rooms_len, ROOMS_MAX, &new_room);

    rooms[new_room].width = (float)((int)((frand_range(0.6f, 0.9f) * (leafs[i].rect.width))));
    rooms[new_room].height = (float)((int)((frand_range(0.6f, 0.9f) * (leafs[i].rect.height))));
    rooms[new_room].x = leafs[i].rect.x + (float)(rand_range(0, (int)(leafs[i].rect.width - rooms[new_room].width)));
    rooms[new_room].y = leafs[i].rect.y + (float)(rand_range(0, (int)(leafs[i].rect.height - rooms[new_room].height)));
  }

  printf("%lu\n", leafs_len);

  #define CELL_SIZE 16

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
