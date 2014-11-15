Parallax Mapping Sample
=======================

This sample shows an implementation of steep relieve mapping to add an impression of depth to a flat
surface.

It uses a heightmap to displace the texture coordinates of the current pixel according to the viewer
view point. It then uses the new texture coordinates to fetch in the diffuse and normal textures.