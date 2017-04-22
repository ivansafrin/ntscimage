
ntscimage is a commandline tool that applies Blargg's NTSC filter (http://slack.net/~ant/libs/ntsc.html) on an image.
This software uses the stb image includes from (https://github.com/nothings/stb) and supports all image file formats supported by it.

Usage: ntscimage inimage outimage.png
inimage can be most common image formats, outimage is always 24-bit PNG.

Currently no commandline arguments; you can tweak the options directly in the code.
Change the phase from 0 to 1 and animate between the two images for an animated version.
