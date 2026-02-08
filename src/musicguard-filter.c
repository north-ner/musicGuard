#include "musicguard-filter.h"
#include "music-detector.h"
#include "ducking.h"
#include "overlay.h"

#include <util/platform.h>

#define MUSIC_COOLDOWN_NS (3ULL * 1000000000ULL)

struct musicguard_state {
    obs_source_t *source;
    struct overlay_state overlay;

    bool music_active;
    uint64_t last_music_time;
};

static const char *musicguard_get_name(void *unused)
{
    UNUSED_PARAMETER(unused);
    return "MusicGuard (Music Ducking + Overlay)";
}

static void *musicguard_create(obs_data_t *settings, obs_source_t *source)
{
    UNUSED_PARAMETER(settings);

    struct musicguard_state *st =
        bzalloc(sizeof(struct musicguard_state));

    st->source = source;
    st->music_active = false;
    st->last_music_time = 0;

    overlay_init(&st->overlay);

    blog(LOG_INFO, "[MusicGuard] Filter instance created.");

    return st;
}

static void musicguard_destroy(void *data)
{
    struct musicguard_state *st = data;
    overlay_shutdown(&st->overlay);
    bfree(st);

    blog(LOG_INFO, "[MusicGuard] Filter destroyed.");
}

static struct obs_audio_data *musicguard_filter_audio(
    void *data,
    struct obs_audio_data *audio)
{
    struct musicguard_state *st = data;

    if (!audio)
        return audio;

    bool detected = music_detect(audio);

    uint64_t now = os_gettime_ns();

    if (detected) {
        st->music_active = true;
        st->last_music_time = now;

        ducking_apply(audio);

        overlay_show(&st->overlay);

    } else {
        if (st->music_active &&
            (now - st->last_music_time > MUSIC_COOLDOWN_NS)) {

            st->music_active = false;
            overlay_hide(&st->overlay);
        }
    }

    return audio;
}

static obs_properties_t *musicguard_properties(void *data)
{
    UNUSED_PARAMETER(data);

    obs_properties_t *props = obs_properties_create();

    obs_properties_add_text(
        props,
        "info",
        "MusicGuard Status",
        OBS_TEXT_INFO);

    return props;
}

static struct obs_source_info musicguard_filter_info = {
    .id = "musicguard_filter",
    .type = OBS_SOURCE_TYPE_FILTER,
    .output_flags = OBS_SOURCE_AUDIO,

    .get_name = musicguard_get_name,
    .create = musicguard_create,
    .destroy = musicguard_destroy,
    .filter_audio = musicguard_filter_audio,
    .get_properties = musicguard_properties,
};

void musicguard_register_filter(void)
{
    obs_register_source(&musicguard_filter_info);
}
