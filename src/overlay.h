#pragma once
#include <stdbool.h>

/*
 * Overlay system (Safe Mode)
 *
 * OBS UI/text sources are NOT thread-safe.
 * So in v1, overlay is log-only and state-based.
 */

typedef struct overlay_state {
    bool active;
} overlay_state_t;

overlay_state_t *overlay_create(void);
void overlay_destroy(overlay_state_t *ov);

void overlay_set_active(overlay_state_t *ov, bool active);
bool overlay_is_active(overlay_state_t *ov);
