#pragma once

#include <stddef.h>
#include <raylib.h>

typedef Rectangle Room;

typedef struct {
  Rectangle rect;
  size_t left_leaf;
  size_t right_leaf;
  size_t parent;
} Leaf;

#define LEAFS_MAX 64
#define MIN_LEAF_SIZE 10

#define ROOMS_MAX LEAFS_MAX

void generate_rooms(Room rooms[ROOMS_MAX], size_t *rooms_len);
