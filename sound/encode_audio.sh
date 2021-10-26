#!/bin/bash
set -e
sox ding_bass.wav -b 8 -e unsigned-integer -r 11000 -c 1 -t raw ding.raw
