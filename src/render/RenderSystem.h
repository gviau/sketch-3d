#ifndef SKETCH_3D_RENDER_SYSTEM_H
#define SKETCH_3D_RENDER_SYSTEM_H

#include "Matrix4x4.h"
#include "RenderWindow.h"

namespace Sketch3D
{

// Forward class declaration
class Geometry;
class Node;
class RenderState;

/**
 * @interface RenderSystem
 * This interface is base class for the implementation of the various API, such
 * as OpenGL and DirectX. It provides methods to access the API's
 * functionalities.
 */
class RenderSystem
{
    public:
        /**
         * Constructor. Initialises the RenderSystem.
         * @param window The RenderWindow on which to render
         */
                            RenderSystem(const RenderWindow& window);
        /**
         * Destructor
         */
        virtual            ~RenderSystem();

        /**
         * Prepares the rendering system to draw.
         * @return true if everything went well, false otherwise.
         */
        virtual bool        BeginScene() = 0;

        /**
         * Swaps the buffer to end the rendering process.
         */
        virtual void        EndScene() = 0;

        /**
         * Draw the scene graph from the specified node.
         * @param node The node from which to draw the scene graph. Usually,
         * this should be the root node.
         */
        virtual void        DrawSceneGraph(Node* node) = 0;

        void                SetTransformMatrix(const Matrix4x4& transform);
        void                SetRenderState(const RenderState* renderState);

    protected:
        Matrix4x4           currentTransformMatrix_; /**< The current transformation matrix to apply */
        RenderWindow        window_;        /**< The window to render in */
        const RenderState*  currentRenderState_;   /**< The current render state to use */

        /**
         * Initializes the RenderSystem
         */
        virtual void        Init();

        /**
         * Shutdowns the RenderSystem
         */
        virtual void        Shutdown();

        /**
         * Draw a Geometry object
         * @param geometry The Geometry object to render
         */
        virtual void        DrawGeometry(Geometry* geometry) = 0;

        /**
         * Clears the back buffer
         */
        virtual void        ClearBackBuffer() = 0;

        /**
         * Displays the back buffer to the screen
         * @return false if an error happened
         */
        virtual bool        DisplayBackBuffer() = 0;
};
}

#endif
