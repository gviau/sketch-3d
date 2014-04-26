#include "system/WindowImplementation.h"

namespace Sketch3D {

WindowEvent WindowImplementation::PollEvent() {
    WindowEvent event = events_.front();
    events_.pop();
    return event;
}

}
