#include "dungeon_generation.h"
#include "rand.h"
#include "utils.h"
#include "game.h"

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


void generate_rooms(Room rooms[ROOMS_MAX], size_t *rooms_len) {
  Leaf leafs[LEAFS_MAX] = {0};
  size_t leafs_len = 0;

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

  for (size_t i = 0; i < leafs_len; i++) {
    if (leafs[i].left_leaf != 0 && leafs[i].right_leaf != 0) continue;

    size_t new_room = 0;
    PUSH(rooms, rooms_len, ROOMS_MAX, &new_room);

    rooms[new_room].width = (float)((int)((frand_range(0.6f, 0.9f) * (leafs[i].rect.width))));
    rooms[new_room].height = (float)((int)((frand_range(0.6f, 0.9f) * (leafs[i].rect.height))));
    rooms[new_room].x = leafs[i].rect.x + (float)(rand_range(0, (int)(leafs[i].rect.width - rooms[new_room].width)));
    rooms[new_room].y = leafs[i].rect.y + (float)(rand_range(0, (int)(leafs[i].rect.height - rooms[new_room].height)));
  }
}
