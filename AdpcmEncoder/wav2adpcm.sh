#!/bin/bash
set -e

## Encodes the given file into a mono 8 bit creative adpcm 4bit encoded voc file
#
# Parameters:
#   input     : The file to be converted
#   frequency : Frequency that the output file will have
#   output    : The output voc file
#


SCRIPTDIR=`dirname $0`

if (($# < 3))
then
    echo "Usage: $0 <input file> <output frequency in Hz> <output file>"
    exit
fi


# PYTHON_EXECUTABLE=python3
PYTHON_EXECUTABLE=pypy3


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
$SCRIPTDIR/build/adpcm_encoder -i $temp_filename -f $frequency -o $output -c PCM
rm $temp_filename
