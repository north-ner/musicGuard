#include "ducking.h"
#include <math.h>

#define ATTACK_RATE 0.20f
#define RELEASE_RATE 0.05f

static float db_to_gain(float db)
{
    return powf(10.0f, db / 20.0f);
}

void ducking_init(ducking_state_t *d)
{
    d->gain = 1.0f;
}

void ducking_process_db(ducking_state_t *d,
                        struct obs_audio_data *audio,
                        bool music_active,
                        float attenuation_db)
{
    if (!audio)
        return;

    /* Convert attenuation (negative dB) into linear gain */
    float target_gain =
        music_active ? db_to_gain(attenuation_db) : 1.0f;

    /* Smooth transition */
    if (music_active)
        d->gain += (target_gain - d->gain) * ATTACK_RATE;
    else
        d->gain += (target_gain - d->gain) * RELEASE_RATE;

    /* Apply gain */
    for (size_t ch = 0; ch < MAX_AV_PLANES; ch++) {

        if (!audio->data[ch])
            continue;

        float *samples = (float *)audio->data[ch];

        for (size_t i = 0; i < audio->frames; i++)
            samples[i] *= d->gain;
    }
}
