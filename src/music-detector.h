#pragma once
#include <obs-module.h>
#include <stdbool.h>

/* Simple music detector (energy-based baseline) */

bool music_detect(struct obs_audio_data *audio);
