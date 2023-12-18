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

#if DISABLE_NOISE != 1
static AudioStream noise_stream;
#endif

static Music drill;

/* TODO: better drill sounds */
extern const unsigned char drill_wav[];
extern const unsigned long drill_wav_size;

void init_audio(void) {
  InitAudioDevice();

  drill = LoadMusicStreamFromMemory(".wav", drill_wav, (int)drill_wav_size);

  SetMusicVolume(drill, 0.5f);

#if DISABLE_NOISE != 1
  noise_stream = LoadAudioStream(44100, 16, 1);
  SetAudioStreamCallback(noise_stream, noise_callback);
#endif
}

void play_audio(void) {
#if DISABLE_NOISE != 1
  PlayAudioStream(noise_stream);
#endif
}

void start_drilling(void) {
  PlayMusicStream(drill);
}

void update_drill(void) {
  UpdateMusicStream(drill);
}

void stop_drilling(void) {
  StopMusicStream(drill);
}

void cleanup_audio(void) {
#if DISABLE_NOISE != 1
  UnloadAudioStream(noise_stream);
#endif

  UnloadMusicStream(drill);

  CloseAudioDevice();
}
