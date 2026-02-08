#pragma once
#include <obs-module.h>

#define OVERLAY_NAME "MusicGuard Warning Overlay"

struct overlay_state {
    obs_source_t *text_source;
};

void overlay_init(struct overlay_state *ov);
void overlay_show(struct overlay_state *ov);
void overlay_hide(struct overlay_state *ov);
void overlay_shutdown(struct overlay_state *ov);
