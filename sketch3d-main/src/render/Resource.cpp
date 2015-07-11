#include "render/Resource.h"

namespace Sketch3D {
unsigned int Resource::nextAvailableId_ = 0;

Resource::Resource() {
    resourceId_ = nextAvailableId_++;
}

unsigned int Resource::GetId() const {
    return resourceId_;
}
}