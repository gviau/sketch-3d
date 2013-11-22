#include "RenderState.h"

#include "RenderSystem.h"

namespace Sketch3D
{
RenderState::RenderState(const string& name) : Node(name)
{
}

RenderState::~RenderState()
{
    RemoveAllGlobalStates();
}

void RenderState::Draw(RenderSystem* renderSystem) const
{
    renderSystem->SetRenderState(this);
}

void RenderState::UpdateRenderState(const RenderState& newState)
{
    map<GlobalState::globalState_t, GlobalState*>::const_iterator it;
    RemoveAllGlobalStates();

    for (it = newState.states_.begin(); it != newState.states_.end(); ++it) {
        AddGlobalState(it->second->CreateCopy());
    }
}

void RenderState::AddGlobalState(GlobalState* state)
{
    states_[state->GetType()] = state;
}

void RenderState::RemoveGlobalState(GlobalState::globalState_t type)
{
    map<GlobalState::globalState_t,
        GlobalState*>::iterator it = states_.find(type);
    if (it != states_.end()) {
        states_.erase(it);
    }
}

void RenderState::RemoveAllGlobalStates()
{
    map<GlobalState::globalState_t, GlobalState*>::iterator it;
    for (it = states_.begin(); it != states_.end(); ++it) {
        delete it->second;
        it->second = NULL;
    }

    states_.clear();
}

GlobalState* RenderState::GetGlobalState(GlobalState::globalState_t state) const
{
    map<GlobalState::globalState_t, GlobalState*>::const_iterator it;
    for (it = states_.begin(); it != states_.end(); ++it) {
        if (it->second->GetType() == state) {
            return it->second;
        }
    }

    return 0;
}

RenderState::nodeType_t RenderState::GetType() const
{
    return RENDERSTATE;
}
}
