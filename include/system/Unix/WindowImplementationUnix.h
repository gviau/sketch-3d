#ifndef SKETCH_3D_WINDOW_IMPLEMENTATION_WIN32_H
#define SKETCH_3D_WINDOW_IMPLEMENTATION_WIN32_H

#include "system/WindowImplementation.h"a

#include <X11/Xlib.h>

namespace Sketch3D {

/**
 * @class WindowImplementationUnix
 * This class provides the functionality to create a window on the Unix
 * platform.
 */
class WindowImplementationUnix : public WindowImplementation {
    public:
        WindowImplementationUnix(const string& title, unsigned int width,
                                 unsigned int height, bool windowed);
        virtual ~WindowImplementationUnix();
        virtual void Close();
        virtual void ProcessEvents();
        virtual void SetVisible(bool val);
        virtual void SetPosition(unsigned int x, unsigned y);
        virtual void SetSize(unsigned int width, unsigned int height);
        virtual void SetWindowed(bool val);
        virtual WindowHandle GetHandle() const { return static_cast<WindowHandle>(window_); }

    private:
        ::Window            window_;    /**< X11 structure defining our window */
        ::Display*          display_;   /**< Pointer to the display */
        int                 screen_;    /**< Screen identifier */
        Atom                atomClose_; /**< Atom used to identify the close event */


        /**
         * Process a single XEvent to add it to the event queue
         * @param event The XEvent to process
         */
         void               ProcessEvent(const XEvent& event);
};

}

#endif
