#include "ducking.h"

#define DUCK_GAIN 0.01f      /*experimenting with audio levels*/
#define ATTACK_RATE 0.15f   /* fast fade down */
#define RELEASE_RATE 0.03f  /* slow fade up */

void ducking_init(ducking_state_t *d)
{
    d->gain = 1.0f;
}

void ducking_process(ducking_state_t *d,
                     struct obs_audio_data *audio,
                     bool music_active)
{
    if (!audio)
        return;

    /* Smooth gain */
    if (music_active) {
        d->gain += (DUCK_GAIN - d->gain) * ATTACK_RATE;
    } else {
        d->gain += (1.0f - d->gain) * RELEASE_RATE;
    }

    /* Apply gain to all planes */
    for (size_t ch = 0; ch < MAX_AV_PLANES; ch++) {

        if (!audio->data[ch])
            continue;

        float *samples = (float *)audio->data[ch];

        for (size_t i = 0; i < audio->frames; i++) {
            samples[i] *= d->gain;
        }
    }
}
