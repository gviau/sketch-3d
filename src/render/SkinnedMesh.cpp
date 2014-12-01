#include "render/SkinnedMesh.h"

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include "render/BufferObjectManager.h"
#include "render/ModelManager.h"
#include "render/Renderer.h"
#include "render/Texture2D.h"
#include "render/TextureManager.h"

#include "system/Logger.h"
#include "system/Utils.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <map>
#include <queue>
using namespace std;

namespace Sketch3D {

SkinnedMesh::SkinnedMesh(MeshType_t meshType) : Mesh(meshType), skeleton_(nullptr), time_(0.0),
                                                currentAnimationState_(nullptr), isLooping_(false)
{
}

SkinnedMesh::SkinnedMesh(const string& filename, const VertexAttributesMap_t& vertexAttributes,
                         MeshType_t meshType) : Mesh(filename, vertexAttributes, meshType), skeleton_(nullptr), time_(0.0),
                                                currentAnimationState_(nullptr), isLooping_(false)
{
    Load(filename, vertexAttributes);
}

SkinnedMesh::~SkinnedMesh() {
    FreeMeshMemory();
}

void SkinnedMesh::Load(const string& filename, const VertexAttributesMap_t& vertexAttributes) {
    // Delete last model's skeleton if present
    if (skeleton_ != nullptr) {
        ModelManager::GetInstance()->RemoveSkeletonReferenceFromCache(filename_);
    }

    // Check cache first
    if (ModelManager::GetInstance()->CheckIfSkeletonLoaded(filename)) {
        skeleton_ = ModelManager::GetInstance()->LoadSkeletonFromCache(filename);
        return;
    }

    // Reload the scene - we need it to query information about the bones
    const aiScene* scene = nullptr;
    if (importer_ == nullptr) {
        importer_ = new Assimp::Importer;

        scene = importer_->ReadFile(filename, aiProcess_LimitBoneWeights);

        if (scene == nullptr) {
            Logger::GetInstance()->Error("Couldn't load mesh " + filename);
            delete importer_;
            importer_ = nullptr;
            return;
        }
    } else {
        scene = importer_->GetScene();
    }

    if (!scene->HasAnimations()) {
        Logger::GetInstance()->Warning("Skinned mesh " + filename + " has no animations");
        return;
    }

    // Because there are several sub meshes, we want to remember the bone is from which mesh
    set<string> necessaryBones;
    map<string, vector<pair<const aiBone*, size_t>>> bones;
    size_t cumulativeNumVertices = 0; 
    queue<const aiNode*> nodes;
    nodes.push(scene->mRootNode);

    // Load the bones
    while (!nodes.empty()) {
        const aiNode* node = nodes.front();
        nodes.pop();

        for (size_t i = 0; i < node->mNumMeshes; i++) {
            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            size_t numVertices = mesh->mNumVertices;

            // Retrieve for each bones the offset matrix and the vertex with their weight that are attached to it
            if (mesh->HasBones()) {
                for (size_t j = 0; j < mesh->mNumBones; j++) {
                    const aiBone* bone = mesh->mBones[j];
                    if (bones.find(bone->mName.data) == bones.end()) {
                        bones[bone->mName.data] = vector<pair<const aiBone*, size_t>>();
                    }

                    bones[bone->mName.data].push_back( pair<const aiBone*, size_t>(bone, cumulativeNumVertices) );
                    necessaryBones.insert(bone->mName.data);

                    // Also, there might be some bones in the hierarchy that have no vertices attached to them
                    // They might still be useful because they connect other bones, so we have to add them to the
                    // skeleton.
                    queue<const aiNode*> nodeHierarchy;
                    nodeHierarchy.push(scene->mRootNode);

                    while (!nodeHierarchy.empty()) {
                        const aiNode* currentNode = nodeHierarchy.front();
                        nodeHierarchy.pop();

                        if (currentNode->mName == bone->mName) {
                            const aiNode* parentNode = currentNode->mParent;

                            while (parentNode != nullptr || (parentNode != nullptr && parentNode->mName == node->mName) ||
                                   (node->mParent != nullptr && parentNode != nullptr && parentNode->mName == node->mParent->mName))
                            {
                                necessaryBones.insert(parentNode->mName.data);
                                parentNode = parentNode->mParent;
                            }

                            break;
                        } else {
                            for (size_t k = 0; k < currentNode->mNumChildren; k++) {
                                nodeHierarchy.push(currentNode->mChildren[k]);
                            }
                        }
                    }
                }
            }

            cumulativeNumVertices += numVertices;
        }

        for (size_t i = 0; i < node->mNumChildren; i++) {
            nodes.push(node->mChildren[i]);
        }
    }

    // Build the skeleton
    skeleton_ = new Skeleton;
    aiMatrix4x4 globalTransform = scene->mRootNode->mTransformation;
    Matrix4x4 globalInverseTransform(globalTransform.a1, globalTransform.a2, globalTransform.a3, globalTransform.a4,
                                     globalTransform.b1, globalTransform.b2, globalTransform.b3, globalTransform.b4,
                                     globalTransform.c1, globalTransform.c2, globalTransform.c3, globalTransform.c4,
                                     globalTransform.d1, globalTransform.d2, globalTransform.d3, globalTransform.d4);
    globalInverseTransform.Inverse();
    skeleton_->SetGlobalInverseTransform(globalInverseTransform);

    nodes.push(scene->mRootNode);

    while (!nodes.empty()) {
        const aiNode* node = nodes.front();
        nodes.pop();

        set<string>::iterator s_it = necessaryBones.find(node->mName.data);
        if (s_it != necessaryBones.end()) {
            const aiNode* parent = node->mParent;

            if (parent == nullptr) {
                skeleton_->CreateBone(*s_it, Matrix4x4::IDENTITY);
            } else {
                // Build the bone hierarchy
                Bone_t* parentBone = skeleton_->FindBoneByName(parent->mName.data);
                if (parentBone == nullptr) {
                    parentBone = skeleton_->CreateBone(parent->mName.data, Matrix4x4::IDENTITY);
                }

                map<string, vector<pair<const aiBone*, size_t>>>::iterator it = bones.find(node->mName.data);
                if (it == bones.end()) {
                    aiMatrix4x4 offset = node->mTransformation;
                    Matrix4x4 offsetMatrix(offset.a1, offset.a2, offset.a3, offset.a4,
                                           offset.b1, offset.b2, offset.b3, offset.b4,
                                           offset.c1, offset.c2, offset.c3, offset.c4,
                                           offset.d1, offset.d2, offset.d3, offset.d4);

                    Bone_t* bone = skeleton_->CreateBone(*s_it, offsetMatrix);
                    parentBone->linkedBones.push_back(bone);
                } else {
                    aiMatrix4x4 offset = it->second[0].first->mOffsetMatrix;
                    Matrix4x4 offsetMatrix(offset.a1, offset.a2, offset.a3, offset.a4,
                                           offset.b1, offset.b2, offset.b3, offset.b4,
                                           offset.c1, offset.c2, offset.c3, offset.c4,
                                           offset.d1, offset.d2, offset.d3, offset.d4);

                    Bone_t* bone = skeleton_->CreateBone(it->first, offsetMatrix);
                    parentBone->linkedBones.push_back(bone);

                    for (size_t i = 0; i < it->second.size(); i++) {
                        const aiVertexWeight* weights = it->second[i].first->mWeights;
                        size_t baseIndex = it->second[i].second;

                        for (size_t j = 0; j < it->second[i].first->mNumWeights; j++) {
                            bone->vertexWeight[baseIndex + weights[j].mVertexId] = weights[j].mWeight;
                        }
                    }
                }
            }
        }

        for (size_t i = 0; i < node->mNumChildren; i++) {
            nodes.push(node->mChildren[i]);
        }
    }

    // Load the animations
    for (size_t i = 0; i < scene->mNumAnimations; i++) {
        aiAnimation* animation = scene->mAnimations[i];
        AnimationState animationState(animation->mDuration, animation->mTicksPerSecond);
        
        // Get the keys for all bones
        for (size_t j = 0; j < animation->mNumChannels; j++) {
            const aiNodeAnim* nodeAnim = animation->mChannels[j];

            vector<pair<double, Vector3>> positionKeys, scaleKeys;
            vector<pair<double, Quaternion>> rotationKeys;

            // Get all the key frames
            for (size_t k = 0; k < nodeAnim->mNumPositionKeys; k++) {
                const aiVectorKey& positionKey = nodeAnim->mPositionKeys[k];
                const aiVector3D& position = positionKey.mValue;
                positionKeys.push_back(pair<double, Vector3>( positionKey.mTime, Vector3(position.x, position.y, position.z) ));
            }

            for (size_t k = 0; k < nodeAnim->mNumRotationKeys; k++) {
                const aiQuatKey& rotationKey = nodeAnim->mRotationKeys[k];
                const aiQuaternion& rotation = rotationKey.mValue;
                rotationKeys.push_back(pair<double, Quaternion>( rotationKey.mTime, Quaternion(rotation.w, rotation.x, rotation.y, rotation.z) ));
            }

            for (size_t k = 0; k < nodeAnim->mNumScalingKeys; k++) {
                const aiVectorKey& scalingKey = nodeAnim->mScalingKeys[k];
                const aiVector3D& scale = scalingKey.mValue;
                scaleKeys.push_back(pair<double, Vector3>( scalingKey.mTime, Vector3(scale.x, scale.y, scale.z) ));
            }

            animationState.SetPositionKeysForBone(nodeAnim->mNodeName.data, positionKeys);
            animationState.SetRotationKeysForBone(nodeAnim->mNodeName.data, rotationKeys);
            animationState.SetScaleKeysForBone(nodeAnim->mNodeName.data, scaleKeys);
        }

        skeleton_->AddAnimationState(animation->mName.data, animationState);
    }

    // Cache the skeleton for future loads
    ModelManager::GetInstance()->CacheSkeleton(filename, skeleton_);

    Logger::GetInstance()->Info("Successfully loaded animations from file " + filename);
}

void SkinnedMesh::Animate(double deltaTime) {
    if (currentAnimationState_ == nullptr) {
        return;
    }

    time_ += deltaTime;
    map<const Bone_t*, Matrix4x4> transformationMatrices;
    if (!skeleton_->GetTransformationMatrices(time_, currentAnimationState_, isLooping_,
                                              transformationMatrices))
    {
        return;
    }

    if (meshType_ == MESH_TYPE_DYNAMIC) {
        // Pass over each vertex and transform them
        bool useNormals = vertexAttributes_.find(VERTEX_ATTRIBUTES_NORMAL) != vertexAttributes_.end();
        map<const Bone_t*, Matrix4x4>::iterator it, end_it = transformationMatrices.end();
        size_t baseIndex = 0;

        vector<vector<Vector3>> originalVertices, originalNormals;

        for (size_t i = 0; i < surfaces_.size(); i++) {
            ModelSurface_t& surface = surfaces_[i];
            bool hasNormals = (surface.geometry->numNormals > 0);

            Vector3* vertices = surface.geometry->vertices;
            Vector3* normals = surface.geometry->normals;
            originalVertices.push_back(vector<Vector3>());
            if (useNormals && hasNormals) {
                originalNormals.push_back(vector<Vector3>());
            }

            for (size_t j = 0; j < surface.geometry->numVertices; j++) {
                Matrix4x4 boneTransform;
                it = transformationMatrices.begin();
                size_t numberOfBones = 0;

                for (; it != end_it; ++it) {
                    const Bone_t* bone = it->first;

                    // The key is the vertex index, taking into account at which mesh we're at and the
                    // value if the weight of that vertex for that bone
                    hash_map<size_t, float>::const_iterator w_it = bone->vertexWeight.find(baseIndex + j);
                    if (w_it != bone->vertexWeight.end()) {
                        if (numberOfBones == 0) {
                            boneTransform = it->second * w_it->second;
                        } else {
                            boneTransform += it->second * w_it->second;
                        }

                        numberOfBones += 1;
                    }

                    // A maximum of 4 bones can contribute to a vertex
                    if (numberOfBones == 4) {
                        break;
                    }
                }

                Vector3 vertex = vertices[j];
                Vector4 transformedVertex(vertex.x, vertex.y, vertex.z);
                transformedVertex = boneTransform * transformedVertex;
                vertices[j] = Vector3(transformedVertex.x, transformedVertex.y, transformedVertex.z);
                originalVertices[i].push_back(vertex);

                if (useNormals && hasNormals) {
                    Vector3 normal = normals[j];
                    Vector4 transformedNormal(normal.x, normal.y, normal.z, 0.0f);
                    transformedNormal = boneTransform * transformedNormal;
                    normals[j] = Vector3(normal.x, normal.y, normal.z);
                    originalNormals[i].push_back(normal);
                }
            }

            baseIndex += surface.geometry->numVertices;
        }

        Mesh::UpdateMeshData();

        for (size_t i = 0; i < surfaces_.size(); i++) {
            ModelSurface_t& surface = surfaces_[i];
            bool hasNormals = (surface.geometry->numNormals > 0);

            Vector3* vertices = surface.geometry->vertices;
            Vector3* normals = surface.geometry->normals;

            for (size_t j = 0; j < surface.geometry->numVertices; j++) {
                vertices[j] = originalVertices[i][j];

                if (useNormals && hasNormals) {
                    normals[j] = originalNormals[i][j];
                }
            }
        }
    } else {
        // TODO
    }
}

void SkinnedMesh::SetAnimationState(const string& name) {
    currentAnimationState_ = skeleton_->GetAnimationState(name);
    time_ = 0.0;
}

void SkinnedMesh::SetAnimationLoop(bool looping) {
    isLooping_ = looping;
}

void SkinnedMesh::StopAnimation() {
    currentAnimationState_ = nullptr;
}

bool SkinnedMesh::IsAnimationRunning() const {
    return (currentAnimationState_ != nullptr);
}

void SkinnedMesh::FreeMeshMemory() {
    if (skeleton_ != nullptr) {
        ModelManager::GetInstance()->RemoveSkeletonReferenceFromCache(filename_);
        skeleton_ = nullptr;
    }
}

}