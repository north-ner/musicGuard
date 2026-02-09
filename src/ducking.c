#include "ducking.h"

#define ATTACK_RATE 0.15f
#define RELEASE_RATE 0.03f

void ducking_init(ducking_state_t *d)
{
    d->gain = 1.0f;
}

void ducking_process(ducking_state_t *d,
                     struct obs_audio_data *audio,
                     bool music_active,
                     float target_gain)
{
    if (!audio)
        return;

    if (music_active) {
        d->gain += (target_gain - d->gain) * ATTACK_RATE;
    } else {
        d->gain += (1.0f - d->gain) * RELEASE_RATE;
    }

    for (size_t ch = 0; ch < MAX_AV_PLANES; ch++) {

        if (!audio->data[ch])
            continue;

        float *samples = (float *)audio->data[ch];

        for (size_t i = 0; i < audio->frames; i++) {
            samples[i] *= d->gain;
        }
    }
}
