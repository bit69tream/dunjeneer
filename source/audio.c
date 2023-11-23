#include "audio.h"

#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>

#include "utils.h"

void noise_callback(void *buffer, unsigned int frames) {
  short *b = (short *) buffer;

  const short low = (INT16_MIN) / 2;
  const short high = (INT16_MAX) / 2;

  static short x = 0;

  const short c = 440;

  for (unsigned int i = 0; i < frames; i++) {
    /* b[i] = (short)((drand48() * (double)abs(high - low)) - (double)abs(low)); */
    b[i] = (short)CLAMP(low, high, (x + c * (2 * drand48() - 1)) / 6);
    x = b[i];
  }
}

static AudioStream stream;

void init_audio(void) {
  InitAudioDevice();

  stream = LoadAudioStream(44100, 16, 1);
  SetAudioStreamCallback(stream, noise_callback);
}

void play_audio(void) {
  PlayAudioStream(stream);
}

void cleanup_audio(void) {
  UnloadAudioStream(stream);
  CloseAudioDevice();
}
