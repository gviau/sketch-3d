#include "system/Unix/WindowImplementationUnix.h"

#include "system/Logger.h"

#include <X11/X.h>
#include <X11/Xutil.h>
typedef long int int64_t;
typedef int int32_t;
#include "render/OpenGL/gl/glx.h"

namespace {
    Bool checkEvent(::Display*, XEvent* event, XPointer userData) {
        return event->xany.window == reinterpret_cast< ::Window >(userData);
    }
}

namespace Sketch3D {

WindowImplementationUnix::WindowImplementationUnix(const string& title, unsigned int width,
                                                   unsigned int height, bool windowed) : window_(0)
{
    Logger::GetInstance()->Info("Unix X window creation");

    // Open a connection with the X server
    display_ = XOpenDisplay(NULL);
    screen_ = DefaultScreen(display_);
    ::Window root = RootWindow(display_, screen_);

    int att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    XVisualInfo* visualInfo = glXChooseVisual(display_, 0, att);

    Colormap colormap = XCreateColormap(display_, root, visualInfo->visual, AllocNone);

    XSetWindowAttributes attributes;
    attributes.colormap = colormap;
    attributes.background_pixel = XWhitePixel(display_, 0);
    attributes.event_mask = ExposureMask | KeyPressMask;

    window_ = XCreateWindow(display_, root, 0, 0, width, height, 0,
                            visualInfo->depth, InputOutput, visualInfo->visual,
                            CWColormap | CWEventMask, &attributes);

    XClassHint* classHint = XAllocClassHint();
    classHint->res_name = "sketh-3d";
    classHint->res_class = "sketch-3d";
    XSetClassHint(display_, window_, classHint);
    XFree(classHint);

    XStoreName(display_, window_, title.c_str());

    // Get the close notification message atom
    atomClose_ = XInternAtom(display_, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(display_, window_, &atomClose_, 1);

    isOpen_ = true;
}

WindowImplementationUnix::~WindowImplementationUnix() {
    Close();
}

void WindowImplementationUnix::Close() {
    if (window_) {
        Logger::GetInstance()->Debug("closing");
        XDestroyWindow(display_, window_);
        XFlush(display_);
    }

    XCloseDisplay(display_);
}

void WindowImplementationUnix::SetVisible(bool val) {
    if (val) {
        XMapWindow(display_, window_);
    } else {
        XUnmapWindow(display_, window_);
    }

    XFlush(display_);
}

void WindowImplementationUnix::SetPosition(unsigned int x, unsigned int y) {
}

void WindowImplementationUnix::SetSize(unsigned int width, unsigned int height) {
}

void WindowImplementationUnix::SetWindowed(bool val) {
}

void WindowImplementationUnix::ProcessEvents() {
    XEvent event;
    while (XCheckIfEvent(display_, &event, &checkEvent, reinterpret_cast<XPointer>(window_))) {
        ProcessEvent(event);
    }
}

void WindowImplementationUnix::ProcessEvent(const XEvent& event) {
    switch (event.type) {
        case DestroyNotify:
            break;

        case ReparentNotify:
            XSync(display_, True);
            break;

        case ClientMessage:
            if ((event.xclient.format == 32) && (event.xclient.data.l[0]) == static_cast<long>(atomClose_)) {
                isOpen_ = false;
            }

            break;
    }
}

}
