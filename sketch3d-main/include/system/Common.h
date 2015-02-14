#ifndef SKETCH_3D_COMMON_H
#define SKETCH_3D_COMMON_H

#include "math/Constants.h"
#include "system/Platform.h"
#include <cmath>

namespace Sketch3D
{
// DEFINES
#if PLATFORM == PLATFORM_WIN32
    #define INLINE inline
#elif PLATFORM == PLATFORM_LINUX
    #define INLINE inline
#endif
}

#endif
