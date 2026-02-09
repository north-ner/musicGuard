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

} musicguard_state_t;

static const char *musicguard_get_name(void *unused)
{
    UNUSED_PARAMETER(unused);
    return "MusicGuard (Stable Ducking Filter)";
}

static void *musicguard_create(obs_data_t *settings, obs_source_t *source)
{
    UNUSED_PARAMETER(settings);

    musicguard_state_t *st =
        calloc(1, sizeof(musicguard_state_t));

    st->source = source;
    st->overlay = overlay_create();

    ducking_init(&st->duck);

    st->music_active = false;
    st->last_music_time = 0;

    blog(LOG_INFO, "[MusicGuard] Filter instance created.");

    return st;
}

static void musicguard_destroy(void *data)
{
    musicguard_state_t *st = data;
    if (!st)
        return;

    overlay_destroy(st->overlay);

    blog(LOG_INFO, "[MusicGuard] Filter instance destroyed.");
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

    if (detected) {
        st->last_music_time = now;
        overlay_set_active(st->overlay, true);
    }

    /* HOLD logic: stay active for 2s after detection */
    st->music_active =
        (now - st->last_music_time < HOLD_NS);

    /* Smooth ducking */
    ducking_process(&st->duck, audio, st->music_active);

    return audio;
}

static struct obs_source_info musicguard_filter_info = {
    .id = "musicguard_filter",
    .type = OBS_SOURCE_TYPE_FILTER,
    .output_flags = OBS_SOURCE_AUDIO,

    .get_name = musicguard_get_name,
    .create = musicguard_create,
    .destroy = musicguard_destroy,
    .filter_audio = musicguard_filter_audio,
};

void musicguard_register_filter(void)
{
    obs_register_source(&musicguard_filter_info);

    blog(LOG_INFO,
         "[MusicGuard] Filter registered successfully.");
}
