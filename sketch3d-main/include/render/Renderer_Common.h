#ifndef SKETCH_3D_RENDERER_COMMON_H
#define SKETCH_3D_RENDERER_COMMON_H

#include "math/Plane.h"

#include "system/Platform.h"

namespace Sketch3D {
// Forward struct declaration
class Sphere;

/**
 * @enum RenderSystem_t
 * Enum that give the possible render system that one can use
 */
enum RenderSystem_t {
	RENDER_SYSTEM_OPENGL,
	RENDER_SYSTEM_DIRECT3D9
};

/**
 * @struct FrustumPlanes_t
 * Struct containing the 6 planes of the view frustum
 */
struct SKETCH_3D_API FrustumPlanes_t {
    Plane nearPlane;
    Plane farPlane;
    Plane leftPlane;
    Plane rightPlane;
    Plane bottomPlane;
    Plane topPlane;

    /**
     * Returns true if the specified sphere is completely outside the view frustum, false otherwise
     */
    bool IsSphereOutside(const Sphere& boundingSphere) const;
};

/**
 * @enum DepthStencilBits_t
 * Number of allocated bits for the depth and stencil components per pixel
 */
enum DepthStencilBits_t {
    DEPTH_STENCIL_BITS_D16,
    DEPTH_STENCIL_BITS_D15S1,
    DEPTH_STENCIL_BITS_D24S8,
    DEPTH_STENCIL_BITS_D24X8,
    DEPTH_STENCIL_BITS_D24X4S4,
    DEPTH_STENCIL_BITS_D32
};

/**
 * @enum DisplayFormat_t
 * Specifies the supported format of a display adapter.
 */
enum DisplayFormat_t {
    DISPLAY_FORMAT_A1R5G5B5,
    DISPLAY_FORMAT_A2R10G10B10,
    DISPLAY_FORMAT_A8R8G8B8,
    DISPLAY_FORMAT_R5G6B5,
    DISPLAY_FORMAT_X1R5G5B5,
    DISPLAY_FORMAT_X8R8G8B8
};

/**
 * @struct DisplayMode_t
 * Specifies the supported mode of a display adapter. An adapter can have several modes.
 */
struct SKETCH_3D_API DisplayMode_t {
    size_t          width;          /**< Width of the screen in pixels */
    size_t          height;         /**< Height of the screen in pixels */
    size_t          refreshRate;    /**< Refresh rate of the screen. 0 indicates an adapter default */
    DisplayFormat_t displayFormat;  /**< Display format for this set of resolution and refresh rate */
};

/**
 * @struct RenderParameters_t
 * This struct contains all the information necessary to initialize the rendering context with the appropriates
 * rendering parameters.
 */
struct SKETCH_3D_API RenderParameters_t {
    size_t              width;              /**< Width of the screen in pixels */
    size_t              height;             /**< Height of the screen in pixels */
    size_t              refreshRate;        /**< Refresh rate of the screen. 0 indicates an adapter default */
    DisplayFormat_t     displayFormat;      /**< Display format for this set of resolution and refresh rate */
    DepthStencilBits_t  depthStencilBits;   /**< Number of depth and stencil bits per pixel */
};

/**
 * @struct ConfigFileAttributes_t
 * This struct is filled by a config file and contains all the necessary information to initialize the engine
 */
struct SKETCH_3D_API ConfigFileAttributes_t {
    RenderSystem_t      renderSystem;
    size_t              width;
    size_t              height;
    bool                windowed;
    size_t              refreshRate;
    DisplayFormat_t     displayFormat;
    DepthStencilBits_t  depthStencilBits;
};

bool SKETCH_3D_API ParseConfigFile(const string& filename, ConfigFileAttributes_t& configFileAttributes);

}

#endif