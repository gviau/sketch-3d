#ifndef SKETCH_3D_RENDER_STATE_H
#define SKETCH_3D_RENDER_STATE_H

#include <map>
using namespace std;

#include "GlobalState.h"
#include "Node.h"
#include "Vector4.h"

namespace Sketch3D
{
/**
 * @class RenderState
 * Defines a render state that the RenderSystem will use to render the current
 * geometry with.
 */
class RenderState : public Node
{
    public:
        /**
         * Constructor
         * @param name A reference to the name of the node
         */
                            RenderState(const string& name);

        /**
         * Destructor
         */
        virtual            ~RenderState();

        virtual void        Draw(RenderSystem* renderSystem) const;

        virtual void        UpdateRenderState(const RenderState& newState);

        /**
         * Adds a global state to this render state. If another state of the
         * same type already exists, it will be overiden.
         * @param state The state to add
         */
        void                AddGlobalState(GlobalState* state);

        /**
         * Remove a global state from this render state
         * @param type The type of the global state
         */
        void                RemoveGlobalState(GlobalState::globalState_t type);

        /**
         * Remove all global states from this render state
         */
        void                RemoveAllGlobalStates();

        /**
         * Get a pointer to a GlobalState. Returns NULL is not found
         */
        GlobalState*        GetGlobalState(GlobalState::globalState_t type) const;

        virtual nodeType_t  GetType() const;
    private:
        map<GlobalState::globalState_t,
            GlobalState*>   states_;    /**< The states */
};
}

#endif
