#pragma once

#define DISABLE_NOISE 1

void init_audio(void);
void cleanup_audio(void);

void play_audio(void);

void start_drilling(void);
void update_drill(void);
void stop_drilling(void);
