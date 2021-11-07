# Creative ADPCM

This directory contains a Creative ADPCM 4bit encoder.
The encoder can be used to encode raw unsigned 8bit audio files to Creative ADPCM 4bit VOC files.

## Usage of encoder

Example usage:
~~~bash
python3 encode_creative_adpcm.py input.raw 11000 output.voc
~~~
This will encode the file **input.raw** into a Creative ADPCM encoded VOC file with 11kHz.

## About Creative ADPCM

Creative ADPCM compresses an 8bit per sample sound file into a 4bit per sample sound file.
This halves the required space, but also decreases the sound quality.
This works by only storing the difference to the previous sample.
The difference is stored as a 4bit value.


## About this encoder

I wrote this encoder because I needed a Creative ADPCM encoder for my MSDOS platform game.
At first I looked for already existing encoders, but there actually were no free encoders available.
The only (non free) encoder I could find was the tool VOCEDIT 2 (see http://www.vogons.org/viewtopic.php?t=8634).
Unfortunately that tool only runs on DOS and it is also a GUI tool that does not support scripting.
I actually got VOCEDIT 2 to run using Dosbox but because of the fact that it is not scriptable and also not free, I decided to build an encoder myself.


