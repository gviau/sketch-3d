#ifndef SKETCH_3D_WINDOW_H
#define SKETCH_3D_WINDOW_H

#include "system/WindowHandle.h"

#include <string>
using namespace std;

namespace Sketch3D {

// Forward declaration
class WindowImplementation;

/**
 * @class Window
 * This class provides an interface to create a window independently of the
 * platform on which we are
 */
class Window {
	public:
		/**
		 * Constructor
		 * @param title The title of the window
		 * @param width The width of the window
		 * @param height the height of the window
		 * @param windowed Is the window in windowed mode?
		 */
								Window(const string& title, unsigned int width,
									   unsigned int height, bool windowed);

		/**
		 * Destructor
		 */
							   ~Window();

		/**
		 * Show the window if the value is true, hide the window otherwise
		 */
		void					SetVisible(bool val);

		/**
		 * Set the window's position
		 * @param x The x position of the window
		 * @param y The y position of the window
		 */
		void					SetPosition(unsigned int x, unsigned int y);

		/**
		 * Set the window's size
		 * @param width The new width of the window
		 * @param height The new height of the window
		 */
		void					SetSize(unsigned int width, unsigned int height);

		/**
		 * Set the window in windowed mode if the value is set to true,
		 * fullscreen otherwise.
		 */
		void					SetWindowed(bool val);

		unsigned int			GetWidth() const { return width_; }
		unsigned int			GetHeight() const { return height_; }
		bool					IsWindowed() const { return windowed_; }
		bool					IsOpen() const;
		WindowHandle			GetHandle() const;

	private:
		WindowImplementation*	windowImpl_;	/**< The window implementation based on the platform we are using */
		unsigned int			width_;			/**< The width of the window */
		unsigned int			height_;		/**< The height of the window */
		bool					windowed_;		/**< Are we in windowed mode? */
};

}

#endif