#include "render/Direct3D9/RenderStateCacheDirect3D9.h"

#include <d3d9.h>

namespace Sketch3D {
RenderStateCacheDirect3D9::RenderStateCacheDirect3D9(IDirect3DDevice9* device) {
    device_ = device;

    EnableDepthTestImpl();
    EnableDepthWriteImpl();
    EnableColorWriteImpl();
    EnableBlendingImpl();
    SetDepthComparisonFuncImpl();
    SetCullingMethodImpl();
    SetBlendingEquationImpl();
    SetBlendingFactorImpl();
    SetRenderFillModeImpl();
}

RenderStateCacheDirect3D9::~RenderStateCacheDirect3D9() {

}

void RenderStateCacheDirect3D9::EnableDepthTestImpl() {
    device_->SetRenderState(D3DRS_ZENABLE, (isDepthTestEnabled_) ? D3DZB_TRUE : D3DZB_FALSE);
}

void RenderStateCacheDirect3D9::EnableDepthWriteImpl() {
    device_->SetRenderState(D3DRS_ZWRITEENABLE, (isDepthWriteEnabled_) ? TRUE : FALSE);
}

void RenderStateCacheDirect3D9::EnableColorWriteImpl() {
    device_->SetRenderState(D3DRS_COLORWRITEENABLE, (isColorWriteEnabled_) ? TRUE : FALSE);
}

void RenderStateCacheDirect3D9::EnableBlendingImpl() {
    device_->SetRenderState(D3DRS_ALPHABLENDENABLE, (isBlendingEnabled_) ? TRUE : FALSE);
}

void RenderStateCacheDirect3D9::SetDepthComparisonFuncImpl() {
    DWORD func;
    switch (depthComparisonFunction_) {
        case DEPTH_FUNC_NEVER:
            func = D3DCMP_NEVER;
            break;

        case DEPTH_FUNC_ALWAYS:
            func = D3DCMP_ALWAYS;
            break;

        case DEPTH_FUNC_LESS:
            func = D3DCMP_LESS;
            break;

        case DEPTH_FUNC_LEQUAL:
            func = D3DCMP_LESSEQUAL;
            break;

        case DEPTH_FUNC_EQUAL:
            func = D3DCMP_EQUAL;
            break;

        case DEPTH_FUNC_GEQUAL:
            func = D3DCMP_GREATEREQUAL;
            break;

        case DEPTH_FUNC_GREATER:
            func = D3DCMP_GREATER;
            break;

        case DEPTH_FUNC_NOTEQUAL:
            func = D3DCMP_NOTEQUAL;
            break;
    }

    device_->SetRenderState(D3DRS_ZFUNC, func);
}

void RenderStateCacheDirect3D9::SetCullingMethodImpl() {
    DWORD cullMode;
    if (cullingMethod_ == CULLING_METHOD_BACK_FACE) {
        cullMode = D3DCULL_CW;
    } else if (cullingMethod_ == CULLING_METHOD_FRONT_FACE) {
        cullMode = D3DCULL_CCW;
    }

    device_->SetRenderState(D3DRS_CULLMODE, cullMode);
}

void RenderStateCacheDirect3D9::SetBlendingEquationImpl() {
    DWORD blendingEquation;
    switch (blendingEquation_) {
        case BLENDING_EQUATION_ADD:
            blendingEquation = D3DBLENDOP_ADD;
            break;

        case BLENDING_EQUATION_SUBTRACT:
            blendingEquation = D3DBLENDOP_SUBTRACT;
            break;

        case BLENDING_EQUATION_REVERSE_SUBTRACT:
            blendingEquation = D3DBLENDOP_REVSUBTRACT;
            break;

        case BLENDING_EQUATION_MIN:
            blendingEquation = D3DBLENDOP_MIN;
            break;

        case BLENDING_EQUATION_MAX:
            blendingEquation = D3DBLENDOP_MAX;
            break;
    }

    device_->SetRenderState(D3DRS_BLENDFACTOR, blendingEquation);
}

void RenderStateCacheDirect3D9::SetBlendingFactorImpl() {
    DWORD srcBlendFactor = GetBlendingFactor(sourceBlendingFactor_);
    DWORD dstBlendFactor = GetBlendingFactor(destinationBlendingFactor_);

    device_->SetRenderState(D3DRS_SRCBLEND, srcBlendFactor);
    device_->SetRenderState(D3DRS_DESTBLEND, dstBlendFactor);
}

void RenderStateCacheDirect3D9::SetRenderFillModeImpl() {
    DWORD renderMode;
    switch (renderMode_) {
        case RENDER_MODE_FILL:
            renderMode = D3DFILL_SOLID;
            break;

        case RENDER_MODE_POINT:
            renderMode = D3DFILL_POINT;
            break;

        case RENDER_MODE_WIREFRAME:
            renderMode = D3DFILL_WIREFRAME;
            break;
    }

    device_->SetRenderState(D3DRS_FILLMODE, renderMode);
}

unsigned int RenderStateCacheDirect3D9::GetBlendingFactor(BlendingFactor_t factor) const {
    unsigned int blendingFactor;
    switch (factor) {
        case BLENDING_FACTOR_ZERO:
            blendingFactor = D3DBLEND_ZERO;
            break;

        case BLENDING_FACTOR_ONE:
            blendingFactor = D3DBLEND_ONE;
            break;

        case BLENDING_FACTOR_SRC_COLOR:
            blendingFactor = D3DBLEND_SRCCOLOR;
            break;

        case BLENDING_FACTOR_ONE_MINUS_SRC_COLOR:
            blendingFactor = D3DBLEND_INVSRCCOLOR;
            break;

        case BLENDING_FACTOR_DST_COLOR:
            blendingFactor = D3DBLEND_DESTCOLOR;
            break;

        case BLENDING_FACTOR_ONE_MINUS_DST_COLOR:
            blendingFactor = D3DBLEND_INVDESTCOLOR;
            break;

        case BLENDING_FACTOR_SRC_ALPHA:
            blendingFactor = D3DBLEND_SRCALPHA;
            break;

        case BLENDING_FACTOR_ONE_MINUS_SRC_ALPHA:
            blendingFactor = D3DBLEND_INVSRCALPHA;
            break;

        case BLENDING_FACTOR_DST_ALPHA:
            blendingFactor = D3DBLEND_DESTALPHA;
            break;

        case BLENDING_FACTOR_ONE_MINUS_DST_ALPHA:
            blendingFactor = D3DBLEND_INVDESTALPHA;
            break;

        case BLENDING_FACTOR_SRC_ALPHA_SATURATE:
            blendingFactor = D3DBLEND_SRCALPHASAT;
            break;
    }

    return blendingFactor;
}

}