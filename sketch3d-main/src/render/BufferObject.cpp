#include "render/BufferObject.h"

#include "render/Mesh.h"

namespace Sketch3D {

size_t BufferObject::nextAvailableId_ = 0;

BufferObject::BufferObject(const VertexAttributesMap_t& vertexAttributes, BufferUsage_t usage) : vertexAttributes_(vertexAttributes), usage_(usage),
        vertexCount_(0), stride_(0), indexCount_(0)
{
    id_ = nextAvailableId_++;
}

BufferObject::~BufferObject() {
}

size_t BufferObject::GetVertexAttributesBitField() const {
    size_t vertexAttributes = 0;
    VertexAttributesMap_t::const_iterator it = vertexAttributes_.begin();
    for (; it != vertexAttributes_.end(); ++it) {
        vertexAttributes |= it->first;
    }

    return vertexAttributes;
}

size_t BufferObject::GetId() const {
    return id_;
}

bool BufferObject::AreVertexAttributesValid(int presentVertexAttributes) const {
    // We implicitely count position
    size_t count = 1;
    VertexAttributesMap_t::const_iterator it = vertexAttributes_.begin();

    for (; it != vertexAttributes_.end(); ++it) {
        if ((presentVertexAttributes & it->first) > 0) {
            count += 1;
        }
    }

    return count == vertexAttributes_.size();
}

void PackSurfaceTriangleVertices(const SurfaceTriangles_t* surface, const map<size_t, VertexAttributes_t>& attributesFromIndex,
                                     vector<float>& vertexData, int& presentVertexAttributes, size_t& stride)
{
	// Interleave the data
    vertexData.reserve(surface->numVertices * 3 +
                       surface->numNormals * 3 +
                       surface->numTexCoords * 2 +
                       surface->numTangents * 3);

    bool hasNormals = surface->numNormals > 0;
    bool hasTexCoords = surface->numTexCoords > 0;
    bool hasTangents = surface->numTangents > 0;
    bool hasBones = surface->numBones > 0;
    bool hasWeights = surface->numWeights > 0;

    Vector3 vertex;
    for (size_t j = 0; j < surface->numVertices; j++) {
        map<size_t, VertexAttributes_t>::const_iterator v_it = attributesFromIndex.begin();

        for (; v_it != attributesFromIndex.end(); ++v_it) {
            switch (v_it->second) {
                case VERTEX_ATTRIBUTES_POSITION:
                    vertex = surface->vertices[j];
                    vertexData.push_back(vertex.x); vertexData.push_back(vertex.y); vertexData.push_back(vertex.z);
                    break;

                case VERTEX_ATTRIBUTES_NORMAL:
                    if (hasNormals) {
                        Vector3 normal = surface->normals[j];
                        vertexData.push_back(normal.x); vertexData.push_back(normal.y); vertexData.push_back(normal.z);
                    }
                    break;

                case VERTEX_ATTRIBUTES_TEX_COORDS:
                    if (hasTexCoords) {
                        Vector2 texCoords = surface->texCoords[j];
                        vertexData.push_back(texCoords.x); vertexData.push_back(texCoords.y);
                    }
                    break;

                case VERTEX_ATTRIBUTES_TANGENT:
                    if (hasTangents) {
                        Vector3 tangents = surface->tangents[j];
                        vertexData.push_back(tangents.x); vertexData.push_back(tangents.y); vertexData.push_back(tangents.z);
                    }
                    break;
            }
        }
	}

    stride = sizeof(float) * 3;
    presentVertexAttributes = 0;
    if (hasNormals) {
        presentVertexAttributes |= VERTEX_ATTRIBUTES_NORMAL;
        stride += sizeof(float) * 3;
    }

    if (hasTexCoords) {
        presentVertexAttributes |= VERTEX_ATTRIBUTES_TEX_COORDS;
        stride += sizeof(float) * 2;
    }

    if (hasTangents) {
        presentVertexAttributes |= VERTEX_ATTRIBUTES_TANGENT;
        stride += sizeof(float) * 3;
    }
}

}