Cel Shading Sample
==================

This sample shows a basic implementation of cel shading.

Cel shading is a technique to draw non photorealistic geometry. It tries to mimic
the way that pencil drawing are made, where contour are usually thicker than the
rest of the edges that compose an object.

To draw thick contour, this sample draws the mesh in black with its polygon winding
order reversed, like that, there won't be any polygon occluding the view of the real
object being draw.

The object is then drawn normally using a shader that make big jumps between the
different light intensities.