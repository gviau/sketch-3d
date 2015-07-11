#ifndef SKETCH_3D_HARDWARE_RESOURCE_H
#define SKETCH_3D_HARDWARE_RESOURCE_H

#include "render/Resource.h"

namespace Sketch3D {
/**
 * @class HardwareResource
 * A hardware resource is a resource directly related to the rendering API, such as a vertex buffer,
 * a shader or a texture
 */
class SKETCH_3D_API HardwareResource : public Resource {
};
}

#endif