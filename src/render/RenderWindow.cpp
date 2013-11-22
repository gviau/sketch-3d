#include "RenderWindow.h"

namespace Sketch3D
{
RenderWindow::RenderWindow() : width_(800), height_(800), caption_("Default"),
                               fullscreen_(false)
{
}

RenderWindow::RenderWindow(size_t width, size_t height, const string& caption,
                           bool fullscreen)
{
    width_ = width;
    height_ = height;
    caption_ = caption;
    fullscreen_ = fullscreen;
}

void RenderWindow::SetWidth(size_t width)
{
    width_ = width;
}

void RenderWindow::SetHeight(size_t height)
{
    height_ = height;
}

void RenderWindow::SetCaption(const string& caption)
{
    caption_ = caption;
}

void RenderWindow::SetFullscreen(bool val)
{
    fullscreen_ = val;
}

size_t RenderWindow::GetWidth() const
{
    return width_;
}

size_t RenderWindow::GetHeight() const
{
    return height_;
}

string RenderWindow::GetCaption() const
{
    return caption_;
}

bool RenderWindow::IsFullscreen() const
{
    return fullscreen_;
}
}
