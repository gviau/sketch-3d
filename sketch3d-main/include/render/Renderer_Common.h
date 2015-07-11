#ifndef SKETCH_3D_RENDERER_COMMON_H
#define SKETCH_3D_RENDERER_COMMON_H

#include "math/Plane.h"

#include "system/Platform.h"

namespace Sketch3D {
// Forward struct declaration
class Sphere;

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
 * @enum RenderSystem_t
 * Enum that give the possible render system that one can use
 */
enum RenderSystem_t {
	OPENGL,
	DIRECT3D9,
    DIRECT3D10,
    DIRECT3D11
};

/**
 * @enum DepthStencilBits_t
 * Number of allocated bits for the depth and stencil components per pixel
 */
enum DepthStencilBits_t {
    D16,
    D24S8,
    D32
};

/**
 * @enum DisplayFormat_t
 * Specifies the supported format of a display adapter.
 */
enum DisplayFormat_t {
    A1R5G5B5,
    A2R10G10B10,
    A8R8G8B8,
    R5G6B5,
    X8R8G8B8
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