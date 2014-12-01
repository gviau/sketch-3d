Skeletal Animation Sample
=========================

This sample shows a basic implementation of skeletal animation

Skeletal animation is widely used technique to animated rigged meshes. The technique starts
by creating a skeletal structure where bones are attached to one another. The vertices in
the mesh may be attached to 0, 1 or more bones. The contribution of the bone to that vertex
may vary as well, depending on how many bones contribute to that vertex. Usually, the sum of all
the bones' contribution for a vertex is 1.0. Each parent bone transformation matrix
transforms their children bones.

An animation is played by querying key frames which tells the mesh how it should be transformed
at a particular time. The key frames only prodive information for the bone, not for every vertex.
Between key frame, skeletal animation interpolates the current key frame with the next one.

The mesh is thus animated by interpolating between key frames and advancing through key frame as
time increases.