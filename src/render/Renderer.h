#ifndef SKETCH_3D_RENDERER_H
#define SKETCH_3D_RENDERER_H

#include "SceneGraph.h"

namespace Sketch3D
{
// Forward class declaration
class RenderSystem;

/**
 * @class Renderer
 * This class is a singleton. This class encapsulates the RenderSystem to draw
 * the SceneGraph. It is the class to use to render a scene.
 */
class Renderer
{
    public:
        /**
         * Destructor
         */
                           ~Renderer();
        /**
         * Returns the singleton's instance
         */
        static Renderer*    GetInstance();

        /**
         * Sets and initlizes a new render system
         */
        void                SetRenderSystem(RenderSystem* renderSystem);

        /**
         * Returns the Renderer's scene graph
         */
        SceneGraph&         GetSceneGraph();

    private:
        static Renderer     instance_;      /**< Singleton's instance */
        RenderSystem*       renderSystem_;  /**< The used render system */
        SceneGraph          sceneGraph_;    /**< The scene graph */

        /**
         * Constructor
         */
                            Renderer();

        /**
         * Copy constructor
         */
                            Renderer(Renderer& src);

        /**
         * Assignment operator
         */
        Renderer&           operator= (Renderer& rhs);
};
}

#endif
