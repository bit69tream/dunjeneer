#include "level_generator.h"
#include "rand.h"
#include "utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
        rooms_counter++;
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
    if (leafs[i].left_leaf != 0 && leafs[i].right_leaf != 0) {
      continue;
    }

    size_t new_room = 0;
    PUSH(rooms, rooms_len, ROOMS_MAX, &new_room);

    rooms[new_room].width = (float)((int)((frand_range(0.6f, 0.9f) * (leafs[i].rect.width))));
    rooms[new_room].height = (float)((int)((frand_range(0.6f, 0.9f) * (leafs[i].rect.height))));
    rooms[new_room].x = leafs[i].rect.x + (float)(rand_range(0, (int)(leafs[i].rect.width - rooms[new_room].width - 1)));
    rooms[new_room].y = leafs[i].rect.y + (float)(rand_range(0, (int)(leafs[i].rect.height - rooms[new_room].height - 1)));
  }
}

bool do_vertical_lines_intersect(Point start_a, Point end_a,
                                 Point start_b, Point end_b) {
  assert(start_a.x == end_a.x);
  assert(start_b.x == end_b.x);

  if (start_a.x == 35 && start_a.y == 51) {
    printf("!!!!\n");
  }

  if (start_a.x != start_b.x) {
    return false;
  }

  if (start_a.y > end_a.y) {
    SWAP(size_t, &start_a.y, &end_a.y);
  }

  if (start_b.y > end_b.y) {
    SWAP(size_t, &start_a.y, &end_a.y);
  }

  if (start_a.y < start_b.y) {
    return end_a.y >= start_b.y;
  } else {
    return end_b.y >= start_a.y;
  }
}

bool do_horizontal_lines_intersect(Point start_a, Point end_a,
                                   Point start_b, Point end_b) {
  assert(start_a.y == end_a.y);
  assert(start_b.y == end_b.y);

  if (start_a.y != start_b.y) {
    return false;
  }

  if (start_a.x > end_a.x) {
    SWAP(size_t, &start_a.x, &end_a.x);
  }

  if (start_b.x > end_b.x) {
    SWAP(size_t, &start_a.x, &end_a.x);
  }

  if (start_a.x < start_b.x) {
    return end_a.x >= start_b.x;
  } else {
    return end_b.x >= start_a.x;
  }
}

bool is_line_inside_vertical_walls(Room rooms[ROOMS_MAX], size_t room_index,
                                   Point line_start, Point line_end) {
  return
    do_vertical_lines_intersect(line_start, line_end,
                                (Point) {
                                  .x = (size_t)rooms[room_index].x,
                                  .y = (size_t)rooms[room_index].y,
                                },
                                (Point) {
                                  .x = (size_t)rooms[room_index].x,
                                  .y = (size_t)rooms[room_index].y + (size_t)rooms[room_index].height,
                                }) ||
    do_vertical_lines_intersect(line_start, line_end,
                                     (Point) {
                                       .x = (size_t)rooms[room_index].x + (size_t)rooms[room_index].width,
                                       .y = (size_t)rooms[room_index].y,
                                     },
                                     (Point) {
                                       .x = (size_t)rooms[room_index].x + (size_t)rooms[room_index].width,
                                       .y = (size_t)rooms[room_index].y + (size_t)rooms[room_index].height,
                                     });
}

bool is_line_inside_horizontal_walls(Room rooms[ROOMS_MAX], size_t room_index,
                                     Point line_start, Point line_end) {
  return
    do_horizontal_lines_intersect(line_start, line_end,
                                  (Point) {
                                    .x = (size_t)rooms[room_index].x,
                                    .y = (size_t)rooms[room_index].y,
                                  },
                                  (Point) {
                                    .x = (size_t)rooms[room_index].x + (size_t)rooms[room_index].width,
                                    .y = (size_t)rooms[room_index].y,
                                  }) ||
    do_horizontal_lines_intersect(line_start, line_end,
                                  (Point) {
                                    .x = (size_t)rooms[room_index].x,
                                    .y = (size_t)rooms[room_index].y + (size_t)rooms[room_index].height,
                                  },
                                  (Point) {
                                    .x = (size_t)rooms[room_index].x + (size_t)rooms[room_index].width,
                                    .y = (size_t)rooms[room_index].y + (size_t)rooms[room_index].height,
                                  });
}

bool is_inside_a_wall(Room rooms[ROOMS_MAX], size_t rooms_len,
                      Path paths[PATHS_MAX], size_t path_index) {
  for (size_t i = 0; i < rooms_len; i++) {
    if (paths[path_index].vertical_first) {
      if (is_line_inside_vertical_walls(rooms, i,
                                        paths[path_index].start,
                                        paths[path_index].middle) ||
          is_line_inside_horizontal_walls(rooms, i,
                                          paths[path_index].middle,
                                          paths[path_index].end)) {
        return true;
      }
    } else {
      if (is_line_inside_horizontal_walls(rooms, i,
                                          paths[path_index].start,
                                          paths[path_index].middle) ||
          is_line_inside_vertical_walls(rooms, i,
                                        paths[path_index].middle,
                                        paths[path_index].end)) {
        return true;
      }
    }
  }

  return false;
}

void generate_paths_between_rooms(Room rooms[ROOMS_MAX], size_t rooms_len,
                                  Path paths[PATHS_MAX], size_t *paths_len) {
  for (size_t i = 1; i < rooms_len; i++) {
  try_again:;

    Point point_a = {
        .x = (size_t)((rooms[i - 1].x + 1) + (drand48() * (rooms[i - 1].width - 2))),
        .y = (size_t)((rooms[i - 1].y + 1) + (drand48() * (rooms[i - 1].height - 2))),
    };

    Point point_b = {
        .x = (size_t)((rooms[i].x + 1) + (drand48() * (rooms[i].width - 2))),
        .y = (size_t)((rooms[i].y + 1) + (drand48() * (rooms[i].height - 2))),
    };

    size_t new_path = 0;
    PUSH(paths, paths_len, PATHS_MAX, &new_path);
    if (roll_dice(2)) {
      /* vertical - horizontal */
      paths[new_path] = (Path){
          .start = point_a,
          .middle = (Point){.x = point_a.x, .y = point_b.y},
          .end = point_b,
          .vertical_first = true,
      };
    } else {
      /* horizontal - vertical */
      paths[new_path] = (Path){
          .start = point_a,
          .middle = (Point){.x = point_b.x, .y = point_a.y},
          .end = point_b,
          .vertical_first = false,
      };
    }

    if (is_inside_a_wall(rooms, rooms_len, paths, new_path)) {
      *paths_len -= 1;
      goto try_again;
    }
  }
}

void put_horizontal_path(LevelMap *output_map,
                         size_t x_start,
                         size_t x_end,
                         size_t y) {
  if (x_end < x_start) {
    size_t tmp = x_end;
    x_end = x_start;
    x_start = tmp;
  }

  if (x_start > x_end) {
    printf("bruuuh\n");
  }

  for (size_t xi = x_start; xi <= x_end; xi++) {
    if ((*output_map)[y][xi] == TILE_WALL && roll_dice(2)) {
      (*output_map)[y][xi] = TILE_DOOR;
    } else {
      (*output_map)[y][xi] = TILE_FLOOR;
    }
  }
}

void put_vertical_path(LevelMap *output_map,
                       size_t y_start,
                       size_t y_end,
                       size_t x) {
  if (y_end < y_start) {
    size_t tmp = y_end;
    y_end = y_start;
    y_start = tmp;
  }

  if (y_start > y_end) {
    printf("aaah\n");
  }

  for (size_t yi = y_start; yi <= y_end; yi++) {
    if ((*output_map)[yi][x] == TILE_WALL && roll_dice(2)) {
      (*output_map)[yi][x] = TILE_DOOR;
    } else {
      (*output_map)[yi][x] = TILE_FLOOR;
    }
  }
}

void put_everything_on_a_map(LevelMap *output_map,
                             Room rooms[ROOMS_MAX], size_t rooms_len,
                             Path paths[PATHS_MAX], size_t paths_len) {
  (void)paths;
  (void)paths_len;

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
    if (paths[i].vertical_first) {
      /* vertical horizontal */
      put_vertical_path(output_map,
                        (size_t)paths[i].start.y,
                        (size_t)paths[i].middle.y,
                        (size_t)paths[i].start.x);

      put_horizontal_path(output_map,
                          (size_t)paths[i].middle.x,
                          (size_t)paths[i].end.x,
                          (size_t)paths[i].middle.y);
    } else {
      /* horizontal vertical */
      put_horizontal_path(output_map,
                          (size_t)paths[i].start.x,
                          (size_t)paths[i].middle.x,
                          (size_t)paths[i].start.y);

      put_vertical_path(output_map,
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
  generate_paths_between_rooms(rooms, rooms_len, paths, &paths_len);

  put_everything_on_a_map(output_map, rooms, rooms_len, paths, paths_len);
}
