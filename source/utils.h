#pragma once

#include <stdlib.h>

#include <raylib.h>

#define PUSH(arr, len, cap, dest_index)         \
  do {                                          \
    assert((*(len)) < (cap));                   \
                                                \
    *(dest_index) = (*(len))++;                 \
  } while (0)

#define SIZE_OF(arr) ((sizeof(arr) / sizeof(arr[0])))

#define SWAP(type, a, b)                        \
  do {                                          \
    type _tmp = *(a);                           \
    *(a) = *(b);                                \
    *(b) = _tmp;                                \
  } while (0)

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

#define CLAMP(start, end, value) (MIN(MAX(value, start), end))
#define CLAMPF(start, end, value) (fminf(fmaxf(value, start), end))

#define LERP(a, b, t) (a + (t * (b - a)))

Color color_interpolate(Color a, Color b, float t);
size_t text_width(size_t text_length);
