Shadow Sample
=============

This sample implements shadow mapping with multiple lights.

It shows shadows applied by up to 4 lights at once. Because aliasing artifacts are strong, the sample
make use of percentage close filtering (PCF) to smooth the edges of the shadows.

Finally, the sample uses additive blending to add the contribution of all the lights into the final
image.