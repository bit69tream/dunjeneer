#pragma once

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
