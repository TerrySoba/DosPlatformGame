#!/bin/bash
set -e


# Convert wave file to 11kHz 8bit raw using the tool "sox".

# used options:
#   -b 8                    Use 8bit sample size
#   -e unsigned-integer     Use unsigned samples [0-255]
#   -D                      Disable dithering
#   -r 11000                Resample to 11000Hz (11kHz)
#   -c 1                    Use channel 1 of stereo input
#   -t raw                  Create raw file
# sox step_loop.wav -b 8 -e unsigned-integer -D -r 11000 -c 1 -t raw steps.raw
# sox sine.wav -b 8 -e unsigned-integer -D -r 11000 -c 1 -t raw sine.raw
# sox death_sound2.wav -b 8 -e unsigned-integer -D -r 11000 -c 1 -t raw steps.raw
sox death_sound2.wav -b 8 -e unsigned-integer -D -r 11000 -c 1 -t voc steps.voc

# Why disable dithering? If dithering is enabled, then silent parts become very noisy.