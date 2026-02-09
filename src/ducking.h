#pragma once
#include <obs-module.h>
#include <stdbool.h>

typedef struct ducking_state {
    float gain;
} ducking_state_t;

void ducking_init(ducking_state_t *d);

void ducking_process_db(ducking_state_t *d,
                        struct obs_audio_data *audio,
                        bool music_active,
                        float attenuation_db);
