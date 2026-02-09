#include "ducking.h"
#include <string.h>

#define ATTACK_RATE 0.20f
#define RELEASE_RATE 0.05f

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

    /* HARD MUTE if music detected */
    if (music_active) {
        for (size_t ch = 0; ch < MAX_AV_PLANES; ch++) {
            if (!audio->data[ch])
                continue;

            memset(audio->data[ch], 0,
                   audio->frames * sizeof(float));
        }
        return;
    }

    /* Otherwise normal audio passes untouched */
    return;
}
