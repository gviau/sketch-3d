#ifndef SKETCH_3D_WINDOW_IMPLEMENTATION_WIN32_H
#define SKETCH_3D_WINDOW_IMPLEMENTATION_WIN32_H

#include "system/WindowImplementation.h"

#include <Windows.h>

namespace Sketch3D {

/**
 * @class WindowImplementationWin32
 * This class provides the functionnality to create a window on the Windows
 * platform
 */
class WindowImplementationWin32 : public WindowImplementation {
	public:
		WindowImplementationWin32(const string& title, unsigned int width,
								  unsigned int height, bool windowed);
		virtual ~WindowImplementationWin32();
        virtual void Close();
        virtual void ProcessEvents();
		virtual void SetVisible(bool val);
		virtual void SetPosition(unsigned int x, unsigned int y);
		virtual void SetSize(unsigned int width, unsigned int height);
		virtual void SetWindowed(bool val);
		virtual WindowHandle GetHandle() const { return reinterpret_cast<WindowHandle>(hwnd_); }

	private:
		HWND		    hwnd_;	/**< The window handle */

		/**
		 * Register the window class before creating the actual window
		 */
		void		    RegisterWindow();

		/**
		 * Process the message that the window receives
		 * @param message The message to process
		 * @param wParam The first parameter
		 * @param lParam The second parameter
		 */
		void		    ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

		/**
		 * Function called whenever the window receives a message
		 * @param handle The handle of the window
		 * @param message The received message
		 * @param wParam First parameter of the message
		 * @param lParam Second parameter of the message
		 * @return true to discard the event after it has been processed
		 */
		 static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};

}

#endif
