#pragma once
#include <obs-module.h>
#include <stdbool.h>

/*
 * Voice-Preservation Detector
 *
 * Returns true if signal is "music/non-voice dominant"
 * Returns false if signal is mostly speech-like
 */

bool music_detect(struct obs_audio_data *audio);
