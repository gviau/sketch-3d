#ifndef SKETCH_3D_BUFFER_OBJECT_H
#define SKETCH_3D_BUFFER_OBJECT_H

#include "system/Platform.h"

#include <map>
#include <vector>
using namespace std;

namespace Sketch3D {

// Forward declaration
class Matrix4x4;

/**
 * @enum BufferUsage_t
 * Determines how to internally manage the buffer
 *  - Static means that the buffer should be written once and never modified (you can, but it's slow);
 *  - Dynamic means that the buffer will be updated several times (faster than static)
 */
enum BufferUsage_t {
    BUFFER_USAGE_STATIC,
    BUFFER_USAGE_DYNAMIC
};

/**
 * @enum VertexAttributes_t
 * The different type of vertex attributes that can be loaded from a mesh and
 * sent to the vertex shader. This must be a power of two because we use them in a
 * bitfield to determine what vertex attributes are really present in the mesh data
 */
enum VertexAttributes_t {
    VERTEX_ATTRIBUTES_POSITION = 0,
    VERTEX_ATTRIBUTES_NORMAL = 1,
    VERTEX_ATTRIBUTES_TEX_COORDS = 2,
    VERTEX_ATTRIBUTES_TANGENT = 4,
    VERTEX_ATTRIBUTES_BONES = 8,
    VERTEX_ATTRIBUTES_WEIGHTS = 16
};

// Typdefs
typedef map<VertexAttributes_t, size_t> VertexAttributesMap_t;

/**
 * @class BufferObject
 * This class represents the base class for API dependent buffer object of a vertex buffer
 * coupled with an index buffer
 */
class SKETCH_3D_API BufferObject {
    public:
        /**
         * Constructor
         * @param vertexAttributes The vertex attributes to use with this buffer object
         * @param usage Usage of the buffer
         */
                                BufferObject(const VertexAttributesMap_t& vertexAttributes, BufferUsage_t usage=BUFFER_USAGE_STATIC);

        /**
         * Destructor
         */
        virtual                ~BufferObject();

        /**
         * Actually render the buffer object contents
         */
        virtual void            Render() = 0;

        /**
         * Actually render several instances of the buffer object contents
         * @param modelMatrices A list of model matrices to use to draw all the instances of the buffer object. The
         * size of this list is the number of instances that will be drawn
         */
        virtual void            RenderInstances(const vector<Matrix4x4>& modelMatrices) = 0;

        /**
         * Set the vertices for the vertex buffer
         * @param vertexData An array of float that represent the vertex data
         * @param presentVertexAttributes Bitfield specifying what vertex attributes are actually present
         * @return false if the vertex attributes specified on construction are not all present, true otherwise
         */
        virtual bool            SetVertexData(const vector<float>& vertexData, int presentVertexAttributes) = 0;

        /**
         * Append vertices data at the end of the vertex buffer
         * @param vertexData An array of float that represent the vertex data to append
         * @param presentVertexAttributes Bitfield specifying what vertex attributes are actually present
         * @return false if the vertex attributes specified on construction are not all present, true otherwise
         */
        virtual bool            AppendVertexData(const vector<float>& vertexData, int presentVertexAttributes) = 0;

        /**
         * Set the indices for the index buffer
         * @param indexData An array of unsigned short that represent the index data
         * @param numIndex The number of index in the array
         */
        virtual void            SetIndexData(unsigned short* indexData, size_t numIndex) = 0;

        /**
         * Append indices data at the end of the index buffer
         * @param indexData an array of unsigned short that represent the index data to append
         * @param numIndex The number of index in the array to append
         */
        virtual void            AppendIndexData(unsigned short* indexData, size_t numIndex) = 0;

        /**
         * Prepare buffers for instanced rendering
         */
        virtual void            PrepareInstanceBuffers() = 0;

        size_t                  GetId() const;

    protected:
        VertexAttributesMap_t   vertexAttributes_;  /**< The vertex attributes to use for the vertex buffer */
        BufferUsage_t           usage_;       /**< The buffer usage */
        size_t                  vertexCount_;
        size_t                  indexCount_;
        size_t                  id_;

        static size_t           nextAvailableId_;
};

}

#endif