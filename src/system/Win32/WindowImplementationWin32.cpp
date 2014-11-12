#include "system/Win32/WindowImplementationWin32.h"

#include "system/Logger.h"

namespace {
	LPCSTR className = "Sketch3D_Window";
}

namespace Sketch3D {

WindowImplementationWin32::WindowImplementationWin32(const string& title,
													 unsigned int width,
													 unsigned int height,
													 bool windowed)
{
	Logger::GetInstance()->Info("Win32 window creation");
	
	RegisterWindow();
	
	hwnd_ = CreateWindow(className, title.c_str(),
						 WS_OVERLAPPEDWINDOW,
						 CW_USEDEFAULT,
						 CW_USEDEFAULT,
						 width, height, NULL, NULL, GetModuleHandle(NULL),
						 this);

	// Force the size of the window
	SetSize(width, height);

	isOpen_ = true;
}

WindowImplementationWin32::~WindowImplementationWin32() {
    Close();
}

void WindowImplementationWin32::Close() {
	DestroyWindow(hwnd_);
	UnregisterClass(className, GetModuleHandle(NULL));
}

void WindowImplementationWin32::ProcessEvents() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void WindowImplementationWin32::SetVisible(bool val) {
	ShowWindow(hwnd_, val ? SW_SHOW : SW_HIDE);
}

void WindowImplementationWin32::SetPosition(unsigned int x, unsigned int y) {
	SetWindowPos(hwnd_, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void WindowImplementationWin32::SetSize(unsigned int width, unsigned int height) {
	RECT rectangle = {0, 0, (long)width, (long)height};
	AdjustWindowRect(&rectangle, GetWindowLong(hwnd_, GWL_STYLE), false);
	int w = rectangle.right - rectangle.left;
	int h = rectangle.bottom - rectangle.top;

	SetWindowPos(hwnd_, NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);
}

void WindowImplementationWin32::SetWindowed(bool val) {
}

void WindowImplementationWin32::RegisterWindow() {
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = &WindowImplementationWin32::WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = NULL;
	wndClass.hCursor = 0;
	wndClass.hbrBackground = 0;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = className;
	
	if (!RegisterClass(&wndClass)) {
		Logger::GetInstance()->Error("Couldn't register window class");
	}
}

void WindowImplementationWin32::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_DESTROY:
			break;

		case WM_CLOSE:
			isOpen_ = false;
			break;
	}
}

LRESULT CALLBACK WindowImplementationWin32::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {

	if (message == WM_CREATE) {
		LONG_PTR window = (LONG_PTR)reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams;
		SetWindowLongPtr(handle, GWLP_USERDATA, window);
	}

	// Get the WindowImplementation instance
	WindowImplementationWin32* impl = handle ? reinterpret_cast<WindowImplementationWin32*>(GetWindowLongPtr(handle, GWLP_USERDATA)) : NULL;

	if (impl) {
		impl->ProcessMessage(message, wParam, lParam);
	}

	if (message == WM_CLOSE) {
		return 0;
	}

	if ((message == WM_SYSCOMMAND) && (wParam == SC_KEYMENU)) {
		return 0;
	}

	return DefWindowProc(handle, message, wParam, lParam);
}

}
