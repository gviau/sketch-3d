#ifndef SKETCH_3D_BUFFER_OBJECT_DIRECT3D9_H
#define SKETCH_3D_BUFFER_OBJECT_DIRECT3D9_H

#include "render/BufferObject.h"

// Forward class declaration
struct IDirect3DDevice9;
struct IDirect3DIndexBuffer9;
struct IDirect3DVertexBuffer9;
struct IDirect3DVertexDeclaration9;

namespace Sketch3D {

/**
 * @class BufferObjectDirect3D9
 * Direct3D9 implementation of a Direct3D9 vertex paired with an index buffer
 */
class BufferObjectDirect3D9 : public BufferObject {
    public:
                                        BufferObjectDirect3D9(IDirect3DDevice9* device, const VertexAttributesMap_t& vertexAttributes,
                                                              BufferUsage_t usage=BUFFER_USAGE_STATIC);
        virtual                        ~BufferObjectDirect3D9();
        virtual void                    Render();
        virtual bool                    SetVertexData(const vector<float>& vertexData, int presentVertexAttributes);
        virtual bool                    AppendVertexData(const vector<float>& vertexData, int presentVertexAttributes);
        virtual void                    SetIndexData(unsigned short* indexdata, size_t numIndex);
        virtual void                    AppendIndexData(unsigned short* indexData, size_t numIndex);

    private:
        IDirect3DDevice9*               device_;
        IDirect3DVertexBuffer9*         vertexBuffer_;
        IDirect3DIndexBuffer9*          indexBuffer_;
        IDirect3DVertexDeclaration9*    vertexDeclaration_;
        size_t                          stride_;
        size_t                          primitivesCount_;

        void                            GenerateBuffers();
};

}

#endif