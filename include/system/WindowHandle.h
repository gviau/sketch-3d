#ifndef SKETCH_3D_WINDOW_HANDLE_H
#define SKETCH_3D_WINDOW_HANDLE_H

// Thanks for the SFML source code for the idea
// https://github.com/LaurentGomila/SFML/blob/master/include/SFML/Window/WindowHandle.hpp

#include "Platform.h"

#if PLATFORM == PLATFORM_WIN32
	struct HWND__;
#endif

namespace Sketch3D {
#if PLATFORM == PLATFORM_WIN32
	typedef HWND__* WindowHandle;
#elif PLATFORM == PLATFORM_LINUX
	typedef unsigned long WindowHandle;
#endif
}

#endif