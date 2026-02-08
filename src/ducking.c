#include "ducking.h"

void ducking_apply(struct obs_audio_data *audio)
{
    if (!audio || !audio->data[0])
        return;

    float *samples = (float *)audio->data[0];
    size_t frames = audio->frames;

    float duck_factor = 0.15f;

    for (size_t i = 0; i < frames; i++) {
        samples[i] *= duck_factor;
    }
}
