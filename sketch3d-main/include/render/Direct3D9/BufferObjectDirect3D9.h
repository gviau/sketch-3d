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
        virtual void                    RenderInstances(const vector<Matrix4x4>& modelMatrices);
        virtual BufferObjectError_t     SetVertexData(const vector<float>& vertexData, int presentVertexAttributes);
        virtual BufferObjectError_t     AppendVertexData(const vector<float>& vertexData, int presentVertexAttributes);
        virtual BufferObjectError_t     SetIndexData(unsigned short* indexdata, size_t numIndex);
        virtual BufferObjectError_t     AppendIndexData(unsigned short* indexData, size_t numIndex);
        virtual void                    PrepareInstanceBuffers();

    private:
        IDirect3DDevice9*               device_;
        IDirect3DVertexBuffer9*         vertexBuffer_;
        IDirect3DIndexBuffer9*          indexBuffer_;
        IDirect3DVertexDeclaration9*    vertexDeclaration_;
        size_t                          primitivesCount_;
        IDirect3DVertexBuffer9*         instanceBuffer_;
        bool                            instanceDataPrepared_;

        void                            GenerateBuffers();
        void                            CreateVertexDeclaration(bool hasNormals, bool hasTexCoords, bool hasTangents,
                                                                bool hasBones, bool hasWeights);
};

}

#endif