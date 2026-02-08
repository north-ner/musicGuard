#include "overlay.h"
#include <obs-frontend-api.h>

static obs_source_t *global_overlay = NULL;

void overlay_init(struct overlay_state *ov)
{
    if (global_overlay) {
        ov->text_source = global_overlay;
        return;
    }

    obs_data_t *settings = obs_data_create();
    obs_data_set_string(settings, "text",
        "🎵 Music detected — Discord must be isolated!");
    obs_data_set_int(settings, "font_size", 48);

    global_overlay = obs_source_create(
        "text_gdiplus",
        OVERLAY_NAME,
        settings,
        NULL);

    obs_data_release(settings);

    ov->text_source = global_overlay;

    blog(LOG_INFO, "[MusicGuard] Global overlay created.");

    /* Inject into active scene */
    obs_source_t *scene_source = obs_frontend_get_current_scene();
    if (scene_source) {

        obs_scene_t *scene = obs_scene_from_source(scene_source);

        if (scene && !obs_scene_find_source(scene, OVERLAY_NAME)) {
            obs_scene_add(scene, global_overlay);
            blog(LOG_INFO,
                "[MusicGuard] Overlay injected into active scene.");
        }

        obs_source_release(scene_source);
    }

    overlay_hide(ov);
}

void overlay_show(struct overlay_state *ov)
{
    if (!ov || !ov->text_source)
        return;

    obs_source_set_enabled(ov->text_source, true);
}

void overlay_hide(struct overlay_state *ov)
{
    if (!ov || !ov->text_source)
        return;

    obs_source_set_enabled(ov->text_source, false);
}

void overlay_shutdown(struct overlay_state *ov)
{
    UNUSED_PARAMETER(ov);
    /* Global overlay persists until OBS exit */
}
