#include "system/Window.h"

#include "system/Platform.h"
#include "system/WindowEvent.h"

#if PLATFORM == PLATFORM_WIN32
#include "system/Win32/WindowImplementationWin32.h"
#elif PLATFORM == PLATFORM_LINUX
#include "system/Unix/WindowImplementationUnix.h"
#endif

namespace Sketch3D {
Window::Window(const string& title, unsigned int width, unsigned int height,
			   bool windowed) : width_(width), height_(height), windowed_(windowed)
{
#if PLATFORM == PLATFORM_WIN32
	windowImpl_ = new WindowImplementationWin32(title, width_, height_, windowed_);
#elif PLATFORM == PLATFORM_LINUX
    windowImpl_ = new WindowImplementationUnix(title, width, height, windowed_);
#endif

	windowImpl_->SetVisible(true);
}

Window::~Window() {
	delete windowImpl_;
	windowImpl_ = NULL;
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

bool Window::PollEvents(WindowEvent& event) {
    windowImpl_->ProcessEvents();

    if (windowImpl_->HasEvents()) {
        event = windowImpl_->PollEvent();
        return true;
    }

    return false;
}

bool Window::IsOpen() const {
	return windowImpl_->IsOpen();
}

WindowHandle Window::GetHandle() const {
	return windowImpl_->GetHandle();
}

}
