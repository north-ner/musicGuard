#include "musicguard-filter.h"
#include "music-detector.h"
#include "ducking.h"
#include "overlay.h"

#include <util/platform.h>
#include <stdlib.h>

#define HOLD_NS (2ULL * 1000000000ULL)

typedef struct musicguard_state {
    obs_source_t *source;

    overlay_state_t *overlay;
    ducking_state_t duck;

    bool music_active;
    uint64_t last_music_time;

    /* User configurable attenuation in dB */
    float attenuation_db;

} musicguard_state_t;

/* ---------------- OBS UI ---------------- */

static obs_properties_t *musicguard_properties(void *data)
{
    UNUSED_PARAMETER(data);

    obs_properties_t *props = obs_properties_create();

    /*
     * Attenuation slider:
     * -5 dB  = mild duck
     * -20 dB = strong duck
     * -35 dB = almost mute
     */
    obs_properties_add_float_slider(
        props,
        "attenuation_db",
        "Music Attenuation (dB)  (more negative = quieter)",
        -40.0, -5.0,
        1.0);

    return props;
}

static void musicguard_defaults(obs_data_t *settings)
{
    /* Default: -25 dB (music becomes barely audible) */
    obs_data_set_default_double(settings, "attenuation_db", -25.0);
}

/* ---------------- Filter Core ---------------- */

static const char *musicguard_get_name(void *unused)
{
    UNUSED_PARAMETER(unused);
    return "MusicGuard (Stable Ducking Filter)";
}

static void *musicguard_create(obs_data_t *settings, obs_source_t *source)
{
    musicguard_state_t *st =
        calloc(1, sizeof(musicguard_state_t));

    st->source = source;
    st->overlay = overlay_create();

    ducking_init(&st->duck);

    st->music_active = false;
    st->last_music_time = 0;

    /* Load attenuation from settings */
    st->attenuation_db =
        (float)obs_data_get_double(settings, "attenuation_db");

    blog(LOG_INFO,
         "[MusicGuard] Filter created (attenuation=%.1f dB)",
         st->attenuation_db);

    return st;
}

static void musicguard_update(void *data, obs_data_t *settings)
{
    musicguard_state_t *st = data;

    st->attenuation_db =
        (float)obs_data_get_double(settings, "attenuation_db");

    blog(LOG_INFO,
         "[MusicGuard] Updated attenuation=%.1f dB",
         st->attenuation_db);
}

static void musicguard_destroy(void *data)
{
    musicguard_state_t *st = data;
    if (!st)
        return;

    overlay_destroy(st->overlay);

    blog(LOG_INFO, "[MusicGuard] Filter destroyed.");

    free(st);
}

static struct obs_audio_data *musicguard_filter_audio(
    void *data,
    struct obs_audio_data *audio)
{
    musicguard_state_t *st = data;
    if (!st || !audio)
        return audio;

    bool detected = music_detect(audio);
    uint64_t now = os_gettime_ns();

    /* If music detected, reset hold timer */
    if (detected) {
        st->last_music_time = now;
        overlay_set_active(st->overlay, true);
    }

    /* HOLD logic: keep ducking active for 2 seconds */
    st->music_active =
        (now - st->last_music_time < HOLD_NS);

    /* Smooth ducking in decibels */
    ducking_process_db(&st->duck,
                       audio,
                       st->music_active,
                       st->attenuation_db);

    return audio;
}

/* ---------------- Registration ---------------- */

static struct obs_source_info musicguard_filter_info = {
    .id = "musicguard_filter",
    .type = OBS_SOURCE_TYPE_FILTER,
    .output_flags = OBS_SOURCE_AUDIO,

    .get_name = musicguard_get_name,

    .create = musicguard_create,
    .destroy = musicguard_destroy,
    .update = musicguard_update,

    .get_properties = musicguard_properties,
    .get_defaults = musicguard_defaults,

    .filter_audio = musicguard_filter_audio,
};

void musicguard_register_filter(void)
{
    obs_register_source(&musicguard_filter_info);

    blog(LOG_INFO,
         "[MusicGuard] Filter registered successfully.");
}
