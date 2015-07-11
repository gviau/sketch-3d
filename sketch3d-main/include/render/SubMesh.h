#ifndef SKETCH_3D_SUB_MESH_H
#define SKETCH_3D_SUB_MESH_H

#include "system/Platform.h"

namespace Sketch3D {
// Forward class declaration
class Material;
class Vector2;
class Vector3;
class Vector4;

/**
 * @class SubMesh
 * The SubMesh class represents a part of a mesh _(or the entire mesh) geometry paired with a material
 */
class SKETCH_3D_API SubMesh {
    public:
                        SubMesh() : material_(nullptr), positions_(nullptr), normals_(nullptr), texCoords_(nullptr), tangents_(nullptr), bones_(nullptr),
                                      weights_(nullptr), indices_(nullptr), numPositions_(0), numNormals_(0), numTexCoords_(0), numTangents_(0),
                                      numBones_(0), numWeights_(0), numIndices_(0) {}

        void            DrawSubMesh(

        void            SetMaterial(Material* material);
        void            SetPositions(size_t numPositions, Vector3* positions);
        void            SetNormals(size_t numNormals, Vector3* normals);
        void            SetTexCoords(size_t numTexCoords, Vector2* texCoords);
        void            SetTangents(size_t numTangents, Vector3* tangents);
        void            SetBones(size_t numBones, Vector4* bones);
        void            SetWeights(size_t numWeights, Vector4* weights);
        void            SetIndices(size_t numIndices, unsigned short* indices);
        void            FreeSubMeshData();

    private:
        Material*       material_;  /**< The material used by this SubMesh */

        Vector3*        positions_; /**< List of positions */
        Vector3*        normals_;   /**< List of normals */
        Vector2*        texCoords_; /**< List texture coordinates */
        Vector3*        tangents_;  /**< List of tangents */
        Vector4*        bones_;     /**< List of indices of attached bones. Maximum of 4 bones */
        Vector4*        weights_;   /**< List of weights for their corresponding bones */
        unsigned short* indices_;   /**< List of indices */

        size_t          numPositions_;
        size_t          numNormals_;
        size_t          numTexCoords_;
        size_t          numTangents_;
        size_t          numBones_;
        size_t          numWeights_;
        size_t          numIndices_;
};

}

#endif