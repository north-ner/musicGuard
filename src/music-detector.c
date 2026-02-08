#include "music-detector.h"
#include <math.h>

bool music_detect(struct obs_audio_data *audio)
{
    if (!audio || !audio->data[0])
        return false;

    float *samples = (float *)audio->data[0];
    size_t frames = audio->frames;

    double energy = 0.0;

    for (size_t i = 0; i < frames; i++) {
        float s = samples[i];
        energy += (s * s);
    }

    energy /= frames;

    /* Enterprise threshold:
       speech ~ low variance
       music ~ sustained energy */
    return (energy > 0.0025);
}
