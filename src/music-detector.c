#include "music-detector.h"
#include <math.h>

/*
 * Goertzel algorithm for energy at a target frequency.
 * Much cheaper than FFT and perfect for speech-band analysis.
 */

static double goertzel_energy(const float *samples,
                              size_t frames,
                              double freq,
                              double sample_rate)
{
    double s_prev = 0.0;
    double s_prev2 = 0.0;

    double normalized = freq / sample_rate;
    double coeff = 2.0 * cos(2.0 * M_PI * normalized);

    for (size_t i = 0; i < frames; i++) {
        double s = samples[i] + coeff * s_prev - s_prev2;
        s_prev2 = s_prev;
        s_prev = s;
    }

    return s_prev2 * s_prev2 + s_prev * s_prev -
           coeff * s_prev * s_prev2;
}

/*
 * Voice Band Frequencies (dominant harmonics in speech)
 */
static const double voice_bins[] = {
    200.0, 400.0, 800.0, 1600.0, 2500.0
};

/*
 * Music/Non-Voice Frequencies (highs + lows)
 */
static const double music_bins[] = {
    60.0, 5000.0, 8000.0, 12000.0
};

bool music_detect(struct obs_audio_data *audio)
{
    if (!audio || !audio->data[0])
        return false;

    float *samples = (float *)audio->data[0];
    size_t frames = audio->frames;

    /* OBS sample rate is almost always 48k */
    double sr = 48000.0;

    double voice_energy = 0.0;
    double music_energy = 0.0;

    /* Compute speech-band energy */
    for (size_t i = 0; i < sizeof(voice_bins) / sizeof(double); i++) {
        voice_energy += goertzel_energy(samples, frames,
                                        voice_bins[i], sr);
    }

    /* Compute non-voice energy */
    for (size_t i = 0; i < sizeof(music_bins) / sizeof(double); i++) {
        music_energy += goertzel_energy(samples, frames,
                                        music_bins[i], sr);
    }

    /* Normalize ratio */
    double ratio = music_energy / (voice_energy + 1e-9);

    /*
     * Decision rule:
     * - ratio > 2.0 → music/non-voice dominates
     * - ratio < 2.0 → speech dominates
     */
    return (ratio > 6.0);
}
