#ifndef SKETCH_3D_RENDERER_COMMON_H
#define SKETCH_3D_RENDERER_COMMON_H

#include "math/Plane.h"

namespace Sketch3D {
// Forward struct declaration
class Sphere;

/**
 * @enum BlendingEquation_t
 * Specifies the equation to use in the blending process
 */
enum BlendingEquation_t {
    BLENDING_EQUATION_ADD,
    BLENDING_EQUATION_SUBTRACT,
    BLENDING_EQUATION_REVERSE_SUBTRACT,
    BLENDING_EQUATION_MIN,
    BLENDING_EQUATION_MAX
};

/**
 * @enum BlendingFactors_t
 * Enumeration of all the possible blending factors for the blending equation
 */
enum BlendingFactor_t {
    BLENDING_FACTOR_ZERO,
    BLENDING_FACTOR_ONE,
    BLENDING_FACTOR_SRC_COLOR,
    BLENDING_FACTOR_ONE_MINUS_SRC_COLOR,
    BLENDING_FACTOR_DST_COLOR,
    BLENDING_FACTOR_ONE_MINUS_DST_COLOR,
    BLENDING_FACTOR_SRC_ALPHA,
    BLENDING_FACTOR_ONE_MINUS_SRC_ALPHA,
    BLENDING_FACTOR_DST_ALPHA,
    BLENDING_FACTOR_ONE_MINUS_DST_ALPHA,
    BLENDING_FACTOR_SRC_ALPHA_SATURATE
};

/**
 * @enum ClearBuffer_t
 * Determines which buffer to clear. Those enum should be combined with OR operations
 */
enum ClearBuffer_t {
    CLEAR_BUFFER_COLOR      = 0x01,
    CLEAR_BUFFER_DEPTH      = 0X02
};

/**
 * @enum CullingMethod_t
 * Determines if the culling should be for front face or back face
 */
enum CullingMethod_t {
    CULLING_METHOD_FRONT_FACE,
    CULLING_METHOD_BACK_FACE
};

/**
 * @enum DepthFunc_t
 * Sets the possible depth comparison functions
 */
enum DepthFunc_t {
    DEPTH_FUNC_NEVER,
    DEPTH_FUNC_ALWAYS,
    DEPTH_FUNC_LESS,
    DEPTH_FUNC_LEQUAL,
    DEPTH_FUNC_EQUAL,
    DEPTH_FUNC_GEQUAL,
    DEPTH_FUNC_GREATER,
    DEPTH_FUNC_NOTEQUAL
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
 * @enum RenderSystem_t
 * Enum that give the possible render system that one can use
 */
enum RenderSystem_t {
	RENDER_SYSTEM_OPENGL,
	RENDER_SYSTEM_DIRECT3D9
};

/**
 * @enum RenderMode
 * Enum that give the possible rendering mode
 */
enum RenderMode_t {
	RENDER_MODE_FILL,
	RENDER_MODE_WIREFRAME,
	RENDER_MODE_POINT
};

/**
 * @struct FrustumPlanes_t
 * Struct containing the 6 planes of the view frustum
 */
struct FrustumPlanes_t {
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
struct DisplayMode_t {
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
struct RenderParameters_t {
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
struct ConfigFileAttributes_t {
    RenderSystem_t      renderSystem;
    size_t              width;
    size_t              height;
    bool                windowed;
    size_t              refreshRate;
    DisplayFormat_t     displayFormat;
    DepthStencilBits_t  depthStencilBits;
};

bool ParseConfigFile(const string& filename, ConfigFileAttributes_t& configFileAttributes);

}

#endif