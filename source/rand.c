#include "rand.h"

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
