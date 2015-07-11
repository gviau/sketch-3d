#ifndef SKETCH_3D_RESOURCE_H
#define SKETCH_3D_RESOURCE_H

#include "system/Platform.h"

namespace Sketch3D {
/**
 * @class Resource
 * The Resource class is the base class for all the resources that can be loaded from the disk as well
 * as hardware resources created by the render context
 */
class SKETCH_3D_API Resource {
    public:
                            Resource();
        unsigned int        GetId() const;

    protected:
        unsigned int        resourceId_;
        static unsigned int nextAvailableId_;
};
}

#endif