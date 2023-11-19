#include "level_generator.h"
#include "rand.h"
#include "utils.h"

#include <math.h>
#include <stdio.h>

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

  /* TODO: remove the huge leaf generation duplication */

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

    if (rooms_counter < 10) {
      leafs_len = 2;
      continue;
    }

    break;
  }

  *rooms_len = 0;

  for (size_t i = 0; i < leafs_len; i++) {
    if (leafs[i].left_leaf != 0 && leafs[i].right_leaf != 0) continue;

    size_t new_room = 0;
    PUSH(rooms, rooms_len, ROOMS_MAX, &new_room);

    rooms[new_room].width = (float)((int)((frand_range(0.6f, 0.9f) * (leafs[i].rect.width))));
    rooms[new_room].height = (float)((int)((frand_range(0.6f, 0.9f) * (leafs[i].rect.height))));
    rooms[new_room].x = leafs[i].rect.x +
      (float)(rand_range(0,
                         (int)(leafs[i].rect.width - rooms[new_room].width - 1)));
    rooms[new_room].y = leafs[i].rect.y +
      (float)(rand_range(0,
                         (int)(leafs[i].rect.height - rooms[new_room].height - 1)));
  }
}

void generate_paths_between_rooms(Room rooms[ROOMS_MAX], size_t rooms_len,
                                  Path paths[PATHS_MAX], size_t *paths_len) {
  size_t room_ids[ROOMS_MAX] = {0};
  size_t room_ids_len = rooms_len;

  for (size_t i = 0; i < rooms_len; i++) {
    room_ids[i] = i;
  }

  /* TODO: improve path generator */
  /* it would be better if paths were generated from continuously */
  /* like, from room 0 to room 1, then from room 1 to room 2, etc */
  /* it should also connect last rooms with first ones, or generate a few random paths */

  while (room_ids_len > 1) {
    size_t ai = (size_t)rand_range(0, (int)(room_ids_len - 1));
    size_t id_a = room_ids[ai];
    room_ids[ai] = room_ids[--room_ids_len];

    size_t bi = (size_t)rand_range(0, (int)(room_ids_len - 1));
    size_t id_b = room_ids[bi];
    room_ids[bi] = room_ids[--room_ids_len];

    Point center_a = {
      .x = floorf(rooms[id_a].x + (rooms[id_a].width / 2)),
      .y = floorf(rooms[id_a].y + (rooms[id_a].height / 2)),
    };

    Point center_b = {
      .x = floorf(rooms[id_b].x + (rooms[id_b].width / 2)),
      .y = floorf(rooms[id_b].y + (rooms[id_b].height / 2)),
    };

    size_t new_path = 0;
    PUSH(paths, paths_len, PATHS_MAX, &new_path);
    if (roll_dice(2)) {
      /* vertical - horizontal */
      paths[new_path] = (Path) {
        .start = center_a,
        .middle = (Point) {
          .x = center_a.x,
          .y = center_b.y
        },
        .end = center_b,
        .vertical = true,
      };
    } else {
      /* horizontal - vertical */
      paths[new_path] = (Path) {
        .start = center_a,
        .middle = (Point) {
          .x = center_b.x,
          .y = center_a.y
        },
        .end = center_b,
        .vertical = false,
      };
    }
  }
}

void put_horizontal_line(LevelMap *output_map, size_t x_start, size_t x_end, size_t y) {
  if (x_end < x_start) {
    size_t tmp = x_end;
    x_end = x_start;
    x_start = tmp;
  }

  if (x_start > x_end) {
    printf("bruuuh\n");
  }

  for (size_t xi = x_start; xi < x_end; xi++) {
    if ((*output_map)[y][xi] == TILE_WALL) {
      (*output_map)[y][xi] = TILE_DOOR;
    } else {
      (*output_map)[y][xi] = TILE_FLOOR;
    }
  }
}

void put_vertical_line(LevelMap *output_map, size_t y_start, size_t y_end, size_t x) {
  if (y_end < y_start) {
    size_t tmp = y_end;
    y_end = y_start;
    y_start = tmp;
  }

  if (y_start > y_end) {
    printf("aaah\n");
  }

  for (size_t yi = y_start; yi < y_end; yi++) {
    if ((*output_map)[yi][x] == TILE_WALL) {
      (*output_map)[yi][x] = TILE_DOOR;
    } else {
      (*output_map)[yi][x] = TILE_FLOOR;
    }
  }
}

void put_everything_on_a_map(LevelMap *output_map,
                             Room rooms[ROOMS_MAX], size_t rooms_len,
                             Path paths[PATHS_MAX], size_t paths_len) {
  (void) paths;
  (void) paths_len;

  for (size_t i = 0; i < LEVEL_HEIGHT; i++) {
    for (size_t j = 0; j < LEVEL_WIDTH; j++) {
      (*output_map)[i][j] = TILE_NONE;
    }
  }

  for (size_t i = 0; i < rooms_len; i++) {
    size_t y = (size_t)rooms[i].y;
    size_t x = (size_t)rooms[i].x;

    /* top */
    for (size_t xi = 0; xi < (size_t)rooms[i].width; xi++) {
      (*output_map)[y][x + xi] = TILE_WALL;
    }

    /* bottom */
    y += (size_t)rooms[i].height;
    for (size_t xi = 0; xi < (size_t)rooms[i].width; xi++) {
      (*output_map)[y][x + xi] = TILE_WALL;
    }

    /* left */
    y = (size_t)rooms[i].y;
    for (size_t yi = 0; yi <= (size_t)rooms[i].height; yi++) {
      (*output_map)[y + yi][x] = TILE_WALL;
    }

    /* right */
    x += (size_t)rooms[i].width;
    for (size_t yi = 0; yi <= (size_t)rooms[i].height; yi++) {
      (*output_map)[y + yi][x] = TILE_WALL;
    }

    /* floor */
    x = (size_t)rooms[i].x;
    for (size_t yi = 1; yi < (size_t)rooms[i].height; yi++) {
      for (size_t xi = 1; xi < (size_t)rooms[i].width; xi++) {
        (*output_map)[y + yi][x + xi] = TILE_FLOOR;
      }
    }
  }

  for (size_t i = 0; i < paths_len; i++) {
    if (paths[i].vertical) {
      /* vertical horizontal */
      put_vertical_line(output_map,
                        (size_t)paths[i].start.y,
                        (size_t)paths[i].middle.y,
                        (size_t)paths[i].start.x);

      put_horizontal_line(output_map,
                          (size_t)paths[i].middle.x,
                          (size_t)paths[i].end.x,
                          (size_t)paths[i].middle.y);
    } else {
      /* horizontal vertical */
      put_horizontal_line(output_map,
                          (size_t)paths[i].start.x,
                          (size_t)paths[i].middle.x,
                          (size_t)paths[i].start.y);

      put_vertical_line(output_map,
                        (size_t)paths[i].middle.y,
                        (size_t)paths[i].end.y,
                        (size_t)paths[i].middle.x);
    }
  }
}

void generate_level(LevelMap *output_map) {
  static Room rooms[ROOMS_MAX] = {0};
  size_t rooms_len = 0;

  static Path paths[PATHS_MAX] = {0};
  size_t paths_len = 0;

  generate_rooms(rooms, &rooms_len);
  generate_paths_between_rooms(rooms, rooms_len,
                               paths, &paths_len);

  put_everything_on_a_map(output_map,
                          rooms, rooms_len,
                          paths, paths_len);
}
