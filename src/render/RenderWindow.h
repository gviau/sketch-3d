#ifndef SKETCH_3D_RENDER_WINDOW_H
#define SKETCH_3D_RENDER_WINDOW_H

#include <string>
using namespace std;

namespace Sketch3D
{

/**
 * @class Contains the information required to create a window. It is simply a
 * class containing the data. The actual creation is done by the RenderSystem
 * implementations.
 */
class RenderWindow
{
    public:
        /**
         * Default constructor. Creates a 800x600 window in windowed mode
         */
                RenderWindow();

        /**
         * Constructor
         * @param width The width of the window
         * @param height The height of the window
         * @param caption The caption of the window
         * @param fullscreen Is the window in fullscreen? Default is false
         */
                RenderWindow(size_t width, size_t height, const string& caption,
                             bool fullscreen=false);

        void    SetWidth(size_t width);
        void    SetHeight(size_t height);
        void    SetCaption(const string& caption);
        void    SetFullscreen(bool val);

        size_t  GetWidth() const;
        size_t  GetHeight() const;
        string  GetCaption() const;
        bool    IsFullscreen() const;

    private:
        size_t  width_;         /**< Width of the window */
        size_t  height_;        /**< Height of the window */
        string  caption_;       /**< Window's caption */
        bool    fullscreen_;    /**< Is the window in fullscreen mode? */
};
}

#endif
