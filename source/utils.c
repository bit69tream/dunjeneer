#include "utils.h"

Color color_interpolate(Color a, Color b, float t) {
  return
    (CLITERAL(Color) {
      .r = (unsigned char)LERP((float)a.r, (float)b.r, t),
      .g = (unsigned char)LERP((float)a.g, (float)b.g, t),
      .b = (unsigned char)LERP((float)a.b, (float)b.b, t),
      .a = 255,
    });
}
