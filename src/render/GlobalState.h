#ifndef SKETCH_3D_GLOBAL_STATE_H
#define SKETCH_3D_GLOBAL_STATE_H

#include "Vector4.h"

namespace Sketch3D
{
/**
 * @interface GlobalState
 * Provides an interface for rendering state
 */
class GlobalState
{
    public:
        enum globalState_t {
            ALPHA,
            CULL,
            FOG,
            MATERIAL,
            WIREFRAME,
            ZBUFFER
        };
        /**
         * Returns the type of the state
         */
        virtual globalState_t   GetType() const = 0;

        /**
         * Create a copy of this object.
         */
        virtual GlobalState*    CreateCopy() const = 0;
};

/**
 * @class AlphaState
 * Allows the user to set the rendering parameters for alpha blending. The
 * equation for alpha blending is:
 * \f$a_f\f$ = (1 - \f$a_s\f$)\f$a_s\f$ + \f$a_s\f$\f$a_d\f$
 */
class AlphaState : public GlobalState
{
    public:
        enum alphaBlend_t {
            ZERO,
            ONE,
            SRC_COLOR,
            ONE_MINUS_SRC_COLOR,
            SRC_ALPHA,
            ONE_MINUS_SRC_ALPHA,
            DST_ALPHA,
            ONE_MINUS_DST_ALPHA,
            CONSTANT_COLOR,
            ONE_MINUS_CONSTANT_COLOR,
            CONSTANT_ALPHA,
            ONE_MINUS_CONSTANT_ALPHA,
        };

        enum alphaTest_t {
            NEVER,
            LESS,
            EQUAL,
            LEQUAL,
            GREATER,
            NOTEQUAL,
            GEQUAL,
            ALWAYS
        };

        /**
         * Default constructor. Sets default values
         */
                                AlphaState();

        virtual globalState_t   GetType() const;

        virtual GlobalState*    CreateCopy() const;

        bool                    blendEnabled;   /**< Is alpha blending enabled? Default is false. */
        alphaBlend_t            srcBlend;       /**< Source blending value. Default is SRC_ALPHA. */
        alphaBlend_t            dstBlend;       /**< Destination blending value. Default is ONE_MINUS_SRC_ALPHA. */

        bool                    testEnabled;    /**< Is alpha testing enabled? Default is false. */
        alphaTest_t             test;           /**< The test to use. Default is ALWAYS. */
        float                   reference;      /**< Reference for comparison in [0, 1]. Default is 0. */
};

/**
 * @class CullState
 * Determines if there is any culling to be done by the rendering system
 */
class CullState : public GlobalState
{
    public:
        enum frontFace_t {
            CCW,
            CW
        };

        enum cullFace_t {
            FRONT,
            BACK
        };

        /**
         * Default construtor. Sets default values.
         */
                                CullState();

        virtual globalState_t   GetType() const;

        virtual GlobalState*    CreateCopy() const;

        bool                    enabled;    /**< Default: true */
        frontFace_t             frontFace;  /**< Default: CCW */
        cullFace_t              cullFace;   /**< Default: BACK */
};

/**
 * @class FogState
 * Provides a class to set fog effect
 */
class FogState : public GlobalState
{
    public:
        enum density_t {
            LINEAR,
            EXP,
            EXPSQR
        };

        enum apply_t {
            PER_VERTEX,
            PER_PIXEL
        };

        /**
         * Default constructor. Sets default values.
         */
                                FogState();

        virtual globalState_t   GetType() const;

        virtual GlobalState*    CreateCopy() const;

        bool                    enabled;    /**< Default is false. */
        float                   start;      /**< Default is 0 */
        float                   end;        /**< Default is 1 */
        float                   power;      /**< Default is 1 */
        Vector4                 color;      /**< Default is (0, 0, 0, 1) */
        density_t               density;    /**< Default is LINEAR */
        apply_t                 apply;      /**< Default is PER_VERTEX */
};

/**
 * @class MaterialState
 * Provides a class for a material providing a color to an object.
 * Colors are in RGBA format.
 */
class MaterialState : public GlobalState
{
    public:
        /**
         * Default constructor. Sets default values
         */
                                MaterialState();

        virtual globalState_t   GetType() const;

        virtual GlobalState*    CreateCopy() const;

        Vector4                 ambient;    /**< Default is (0.2, 0.2, 0.2, 1) */
        Vector4                 diffuse;    /**< Default is (0.8, 0.8, 0.8, 1) */
        Vector4                 specular;   /**< Default is (0,   0,   0,   1) */
        Vector4                 emissive;   /**< Default is (0,   0,   0,   1) */
        float                   shininess;  /**< Default is 1 */
};

/**
 * @class WireframeState
 * Enables the user to render in wireframe mode
 */
class WireframeState : public GlobalState
{
    public:
        /**
         * Default constructor. Sets default values.
         */
                                WireframeState();

        virtual globalState_t   GetType() const;

        virtual GlobalState*    CreateCopy() const;

        bool                    enabled;    /**< Default: false */
};

/**
 * @class ZBufferState
 * Provides the user an interface to sets options on how to use the ZBuffer
 */
class ZBufferState : public GlobalState
{
    public:
        enum compare_t {
            NEVER,
            LESS,
            EQUAL,
            LEQUAL,
            GREATER,
            NOTEQUAL,
            GEQUAL,
            ALWAYS
        };

        /**
         * Default constructor. Sets default values
         */
                                ZBufferState();

        virtual globalState_t   GetType() const;

        virtual GlobalState*    CreateCopy() const;

        bool                    enabled;    /**< Default: true */
        bool                    writable;   /**< Default: true */
        compare_t               compare;    /**< Default: LEQUAL */
};
}
#endif
