#include "overlay.h"
#include <obs-module.h>
#include <stdlib.h>

overlay_state_t *overlay_create(void)
{
    overlay_state_t *ov = calloc(1, sizeof(overlay_state_t));

    blog(LOG_INFO,
         "[MusicGuard] Overlay initialized (SAFE log-only mode).");

    return ov;
}

void overlay_destroy(overlay_state_t *ov)
{
    if (!ov)
        return;

    blog(LOG_INFO,
         "[MusicGuard] Overlay destroyed.");

    free(ov);
}

void overlay_set_active(overlay_state_t *ov, bool active)
{
    if (!ov)
        return;

    /* Log only when switching ON */
    if (active && !ov->active) {
        blog(LOG_WARNING,
             "[MusicGuard] MUSIC DETECTED — Ducking audio!");
    }

    ov->active = active;
}

bool overlay_is_active(overlay_state_t *ov)
{
    return ov ? ov->active : false;
}
