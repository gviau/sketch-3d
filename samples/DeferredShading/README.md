Deferred Shading Sample
=======================

This sample shows a basic implementation of deferred shading.

Deferred shading works by rendering all the geometrys' attributes, such as positions, normals and
diffuse texture colors, to a render texture called the G-Buffer. Then, it renders a full screen quad
and uses the G-Buffer to fetch the attributes of each pixel to compute the lighting contribution.