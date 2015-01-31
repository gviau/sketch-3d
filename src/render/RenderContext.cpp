#include "render/RenderContext.h"

namespace Sketch3D {
bool RenderContext::AreRenderParametersSupported(const RenderParameters_t& renderParameters) const {
    for (size_t i = 0; i < supportedDisplayModes_.size(); i++) {
        const DisplayMode_t& displayMode = supportedDisplayModes_[i];

        if (displayMode.width == renderParameters.width && displayMode.height == renderParameters.height &&
            (displayMode.refreshRate == renderParameters.refreshRate || renderParameters.refreshRate == 0) &&
            displayMode.displayFormat == renderParameters.displayFormat)
        {
            return true;
        }
    }

    return false;
}
}