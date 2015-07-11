#ifndef SKETCH_3D_RENDER_STATE_H
#define SKETCH_3D_RENDER_STATE_H

namespace Sketch3D {
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
    FRONT_FACE,
    BACK_FACE
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
 * @enum FillMode_t
 * Enum that give the possible rendering mode
 */
enum FillMode_t {
	FILL,
	WIREFRAME
};

}
#endif