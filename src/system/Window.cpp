#include "system/Window.h"

#include "system/Platform.h"

#if PLATFORM == PLATFORM_WIN32
#include "system/Win32/WindowImplementationWin32.h"
#elif PLATFORM == PLATFORM_LINUX
#endif

namespace Sketch3D {
Window::Window(const string& title, unsigned int width, unsigned int height,
			   bool windowed) : width_(width), height_(height), windowed_(windowed)
{
#if PLATFORM == PLATFORM_WIN32
	windowImpl_ = new WindowImplementationWin32(title, width_, height_, windowed_);
#elif PLATFORM == PLATFORM_LINUX
#endif

	windowImpl_->SetVisible(true);
}

Window::~Window() {
	delete windowImpl_;
}

void Window::SetVisible(bool val) {
}

void Window::SetPosition(unsigned int x, unsigned int y) {
}

void Window::SetSize(unsigned int width, unsigned int height) {
}

void Window::SetWindowed(bool val) {
	if (windowed_ != val) {
		windowed_ = val;

		if (windowed_) {
		} else {
		}
	}
}

bool Window::IsOpen() const {
	return windowImpl_->IsOpen();
}

WindowHandle Window::GetHandle() const {
	return windowImpl_->GetHandle();
}

}