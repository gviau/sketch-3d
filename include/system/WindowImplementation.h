#ifndef SKETCH_3D_WINDOW_IMPLEMENTATION_H
#define SKETCH_3D_WINDOW_IMPLEMENTATION_H

#include "system/Platform.h"
#include "system/WindowEvent.h"
#include "system/WindowHandle.h"

#include <queue>
#include <string>
using namespace std;

namespace Sketch3D {

/**
 * @class WindowImplementation
 * This class is an interface for the platform dependend implementation of a
 * window.
 */
class SKETCH_3D_API WindowImplementation {
	public:
        /**
         * Close the window
         */
        virtual void            Close() = 0;

        /**
         * Get the events from the window implementation. This function will
         * fill the events queue.
         */
        virtual void            ProcessEvents() = 0;

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
         * Return an event from the queue
         */
        WindowEvent             PollEvent();

		/**
		 * Return the OS dependent window handle
		 */
		virtual WindowHandle	GetHandle() const = 0;

		bool					IsOpen() const { return isOpen_; }
        bool                    HasEvents() const { return !events_.empty(); }

	protected:
		bool					isOpen_;	/**< Is the window open? */
        queue<WindowEvent>      events_;    /**< Events to be processed for the window */
};

}

#endif
