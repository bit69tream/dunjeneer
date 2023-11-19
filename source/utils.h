#pragma once

#define PUSH(arr, len, cap, dest_index)         \
  do {                                          \
    assert((*(len)) < (cap));                   \
                                                \
    *(dest_index) = (*(len))++;                 \
  } while (0)
