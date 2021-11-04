#!/bin/bash
set -e


## Encodes the given file into a mono 8 bit creative adpcm 4bit encoded voc file
#
# Parameters:
#   input     : The file to be converted
#   frequency : Frequency that the output file will have
#   output    : The output voc file
#
encode_to_adpcm () {
    input=$1
    frequency=$2
    output=$3

    temp_filename=$(tempfile)

    # used sox options:
    #   -b 8                    Use 8bit sample size
    #   -e unsigned-integer     Use unsigned samples [0-255]
    #   -D                      Disable dithering
    #   -r 11000                Resample to 11000Hz (11kHz)
    #   -c 1                    Use channel 1 of stereo input
    #   -t raw                  Create raw file
    # Why disable dithering? If dithering is enabled, then silent parts become very noisy.
    sox $input -b 8 -e unsigned-integer -D -r $frequency -c 1 -t raw $temp_filename
    python3 encode_creative_adpcm.py $temp_filename $frequency $output
    rm $temp_filename
}

encode_to_adpcm death_sound2.wav 11000 death.voc
encode_to_adpcm step_loop.wav 8000 steps.voc
