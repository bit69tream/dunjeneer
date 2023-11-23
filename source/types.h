#pragma once

#include <raylib.h>
#include <stddef.h>
#include <stdlib.h>

typedef Rectangle Room;

typedef struct {
  size_t x;
  size_t y;
} Vector2U;

typedef Vector2U Point;

typedef struct {
  ssize_t x;
  ssize_t y;
} Vector2I;

typedef enum {
  DIRECTION_NONE = 0,
  DIRECTION_UP = 1 << 0,
  DIRECTION_LEFT = 1 << 1,
  DIRECTION_DOWN = 1 << 2,
  DIRECTION_RIGHT = 1 << 3,
} Direction;
