#ifndef SKETCH_3D_WINDOW_IMPLEMENTATION_H
#define SKETCH_3D_WINDOW_IMPLEMENTATION_H

#include "system/WindowHandle.h"

#include <string>
using namespace std;

namespace Sketch3D {

/**
 * @class WindowImplementation
 * This class is an interface for the platform dependend implementation of a
 * window.
 */
class WindowImplementation {
	public:
		/**
		 * Constructor
		 * @param title The title of the window
		 * @param width The width of the window
		 * @param height the height of the window
		 * @param windowed Is the window in windowed mode?
		 */
								WindowImplementation(const string& title, unsigned int width,
													 unsigned int height, bool windowed) {}

		/**
		 * Destructor
		 */
		virtual				   ~WindowImplementation() {}

		/**
		 * Show the window if the value is true, hide the window otherwise
		 */
		virtual void			SetVisible(bool val) = 0;

		/**
		 * Set the window's position
		 * @param x The x position of the window
		 * @param y The y position of the window
		 */
		virtual void			SetPosition(unsigned int x, unsigned int y) = 0;

		/**
		 * Set the window's size
		 * @param width The new width of the window
		 * @param height The new height of the window
		 */
		virtual void			SetSize(unsigned int width, unsigned int height) = 0;

		/**
		 * Set the window in windowed mode if the value is set to true,
		 * fullscreen otherwise.
		 */
		virtual void			SetWindowed(bool val) = 0;

		/**
		 * Return the OS dependent window handle
		 */
		virtual WindowHandle	GetHandle() const = 0;

		bool					IsOpen() const { return isOpen_; }

	protected:
		bool					isOpen_;	/**< Is the window open? */
};

}

#endif