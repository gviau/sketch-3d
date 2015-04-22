Frustum culling static batching Sample
======================================

This sample showcases frustum culling to eliminate unseen geometry and static batching to make rendering of
objects that share the exact same properties, such as geometry, material, etc, drawn faster.

The initialization is very long (about a minute) because the batching process for this particular example
involves 10 000 objects that each have almso 2 000 vertices. In a real-case example, one would prefer to batch
smaller objects.

However, even without frustum culling, this sample is as fast as the instanced version in viewing angles where
the user can see the same amount of objects.