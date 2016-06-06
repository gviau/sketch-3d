#include "framework/Skeleton.h"

#include "system/Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <map>
#include <queue>
#include <set>
using namespace std;

namespace Sketch3D
{

bool LoadSkeletonFromFileInternal(const string& filename, shared_ptr<Skeleton>& skeleton, map<string, size_t>& bonesNameToIndex);

// Create a single instance of an Assimp importer to load meshes from files
extern unique_ptr<Assimp::Importer> assimpImporter;

Skeleton::Skeleton()
    : m_Root(nullptr)
{
}

Skeleton::~Skeleton()
{
}

bool Skeleton::GetTransformationMatrices(double time, const AnimationState* animationState, bool isLooping,
                                         map<const Bone_t*, Matrix4x4>& transformationMatrices) const
{
    if (animationState == nullptr)
    {
        Logger::GetInstance()->Warning("Trying to play an animation but none is set");
        return false;
    }

    double ticksPerSeconds = animationState->GetTicksPerSeconds();
    if (ticksPerSeconds == 0.0)
    {
        ticksPerSeconds = 25.0;
    }

    double animationTime = ticksPerSeconds * time;
    if (isLooping)
    {
        animationTime = fmod(animationTime, animationState->GetDurationInTicks());
    }
    else if (animationTime > animationState->GetDurationInTicks())
    {
        return false;
    }

    Matrix4x4 identity;
    CalculateBoneTransformation(animationTime, animationState, m_Root, identity, transformationMatrices);

    return true;
}

void Skeleton::AddAnimationState(const string& name, const AnimationState& animationState)
{
    string animationName = name;
    if (animationName == "")
    {
        animationName = "Default";
    }

    m_AnimationStates[animationName] = animationState;
}

const AnimationState* Skeleton::GetAnimationState(const string& name) const
{
    map<string, AnimationState>::const_iterator it = m_AnimationStates.find(name);
    if (it != m_AnimationStates.end())
    {
        return &(it->second);
    }

    return nullptr;
}

Bone_t* Skeleton::CreateBone(const string& name, const Matrix4x4& offsetMatrix)
{
    m_Bones[name] = Bone_t(offsetMatrix);
    m_Bones[name].name = name;

    if (m_Root == nullptr)
    {
        m_Root = &m_Bones[name];
    }

    return &m_Bones[name];
}

Bone_t* Skeleton::FindBoneByName(const string& name)
{
    BoneCacheMap_t::iterator it = m_Bones.find(name);
    if (it != m_Bones.end())
    {
        return &(it->second);
    }

    return nullptr;
}

void Skeleton::SetGlobalInverseTransform(const Matrix4x4& globalInverseTransform)
{
    m_GlobalInverseTransform = globalInverseTransform;
}

void Skeleton::CalculateBoneTransformation(double time, const AnimationState* animationState, const Bone_t* bone,
                                           const Matrix4x4& parentTransformation,
                                           map<const Bone_t*, Matrix4x4>& transformationMatrices) const
{
    Matrix4x4 boneTransformation;

    // First verify that this bone has any animation keys at all, maybe it is just
    // a bone in the hierarchy without any vertices attached to it
    if (animationState->HasAnimationKeys(bone->name))
    {
        Vector3 scaling = CalculateInterpolatedScaling(time, animationState, bone);
        Quaternion rotation = CalculateInterpolatedRotation(time, animationState, bone);
        Vector3 translation = CalculateInterpolatedTranslation(time, animationState, bone);

        Matrix4x4 scalingMatrix, rotationMatrix, translationMatrix;
        scalingMatrix.Scale(scaling);
        rotation.ToRotationMatrix(rotationMatrix);
        translationMatrix.Translate(translation);

        boneTransformation = translationMatrix * rotationMatrix * scalingMatrix;
    }

    Matrix4x4 globalTransformation = parentTransformation * boneTransformation;
    transformationMatrices[bone] = m_GlobalInverseTransform * globalTransformation * bone->offsetMatrix;

    for (size_t i = 0; i < bone->linkedBones.size(); i++)
    {
        CalculateBoneTransformation(time, animationState, bone->linkedBones[i], globalTransformation,
                                    transformationMatrices);
    }
}

Vector3 Skeleton::CalculateInterpolatedScaling(double time, const AnimationState* animationState, const Bone_t* bone) const
{
    size_t scalingIndex = animationState->FindScalingKeyFrameIndex(bone->name, time);
    size_t nextScalingIndex = scalingIndex + 1;

    pair<double, Vector3> scaling = animationState->GetScalingValue(bone->name, scalingIndex);
    pair<double, Vector3> nextScaling = animationState->GetScalingValue(bone->name, nextScalingIndex);

    double deltaTime = nextScaling.first - scaling.first;
    double factor = 0.0;

    if (deltaTime < EPSILON)
    {
        factor = 0.0;
    }
    else
    {
        factor = (time - scaling.first) / deltaTime;
    }

    Vector3 deltaValue = nextScaling.second - scaling.second;
    return scaling.second + (float)factor * deltaValue;
}

Quaternion Skeleton::CalculateInterpolatedRotation(double time, const AnimationState* animationState, const Bone_t* bone) const
{
    size_t rotationIndex = animationState->FindRotationKeyFrameIndex(bone->name, time);
    size_t nextRotationIndex = rotationIndex + 1;

    pair<double, Quaternion> rotation = animationState->GetRotationValue(bone->name, rotationIndex);
    pair<double, Quaternion> nextRotation = animationState->GetRotationValue(bone->name, nextRotationIndex);

    double deltaTime = nextRotation.first - rotation.first;
    double factor = 0.0;

    if (deltaTime < EPSILON)
    {
        factor = 0.0;
    }
    else
    {
        factor = (time - rotation.first) / deltaTime;
    }

    Quaternion interpolatedRotation = rotation.second.Slerp(nextRotation.second, (float)factor, true);
    interpolatedRotation.Normalize();
    return interpolatedRotation;
}

Vector3 Skeleton::CalculateInterpolatedTranslation(double time, const AnimationState* animationState, const Bone_t* bone) const
{
    size_t translationIndex = animationState->FindTranslationKeyFrameIndex(bone->name, time);
    size_t nextTranslationIndex = translationIndex + 1;

    pair<double, Vector3> translation = animationState->GetTranslationValue(bone->name, translationIndex);
    pair<double, Vector3> nextTranslation = animationState->GetTranslationValue(bone->name, nextTranslationIndex);

    double deltaTime = nextTranslation.first - translation.first;
    double factor = 0.0;

    if (deltaTime < EPSILON)
    {
        factor = 0.0;
    }
    else
    {
        factor = (time - translation.first) / deltaTime;
    }

    Vector3 deltaValue = nextTranslation.second - translation.second;
    return translation.second + (float)factor * deltaValue;
}

bool LoadSkeletonFromFile(const string& filename, shared_ptr<Skeleton>& skeleton)
{
	map<string, size_t> bonesNameToIndex;

	return LoadSkeletonFromFileInternal(filename, skeleton, bonesNameToIndex);
}

bool LoadSkeletonFromFileInternal(const string& filename, shared_ptr<Skeleton>& skeleton, map<string, size_t>& bonesNameToIndex)
{
    const aiScene* scene = assimpImporter->ReadFile(filename, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_LimitBoneWeights);

    if (scene == nullptr)
	{
        Logger::GetInstance()->Error("Couldn't load skeleton in skinned mesh " + filename);
        return false;
    }

    if (!scene->HasAnimations())
	{
        Logger::GetInstance()->Warning("Skeleton in skinned mesh " + filename + " has no animations");
        return false;
    }

    map<string, const aiBone*> bones;
	set<string> necessaryBones;

    queue<const aiNode*> nodes;
    nodes.push(scene->mRootNode);

    ///////////////////////////////////////////////////////////////////////////////////
    // Load the bones from the mesh
    while (!nodes.empty())
	{
        const aiNode* node = nodes.front();
        nodes.pop();

        for (size_t i = 0; i < node->mNumMeshes; i++)
		{
            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            
            // Retrieve for each bone the offset matrix
            if (mesh->HasBones())
			{
                for (size_t j = 0; j < mesh->mNumBones; j++)
				{
                    const aiBone* bone = mesh->mBones[j];

					auto it = bonesNameToIndex.find(bone->mName.data);
					if (it == bonesNameToIndex.end())
					{
						bonesNameToIndex[bone->mName.data] = bonesNameToIndex.size();
					}

					bones[bone->mName.data] = bone;
                    necessaryBones.insert(bone->mName.data);

                    // Also, there might be some bones in the hierarchy that have no vertices attached to them
                    // They might still be useful because they connect other bones, so we have to add them to the
                    // skeleton.
                    queue<const aiNode*> nodeHierarchy;
                    nodeHierarchy.push(scene->mRootNode);

                    while (!nodeHierarchy.empty())
					{
                        const aiNode* currentNode = nodeHierarchy.front();
                        nodeHierarchy.pop();

                        if (currentNode->mName == bone->mName)
						{
                            const aiNode* parentNode = currentNode->mParent;

                            while (parentNode != nullptr || (parentNode != nullptr && parentNode->mName == node->mName) ||
                                   (node->mParent != nullptr && parentNode != nullptr && parentNode->mName == node->mParent->mName))
                            {
                                necessaryBones.insert(parentNode->mName.data);
                                parentNode = parentNode->mParent;
                            }

                            break;
                        }
						else
						{
                            for (size_t k = 0; k < currentNode->mNumChildren; k++)
							{
                                nodeHierarchy.push(currentNode->mChildren[k]);
                            }
                        }
                    }
                }
            }
        }

        for (size_t i = 0; i < node->mNumChildren; i++)
		{
            nodes.push(node->mChildren[i]);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////
    // Build the skeleton
    skeleton = shared_ptr<Skeleton>(new Skeleton);
    aiMatrix4x4 globalTransform = scene->mRootNode->mTransformation;
    Matrix4x4 globalInverseTransform(globalTransform.a1, globalTransform.a2, globalTransform.a3, globalTransform.a4,
                                     globalTransform.b1, globalTransform.b2, globalTransform.b3, globalTransform.b4,
                                     globalTransform.c1, globalTransform.c2, globalTransform.c3, globalTransform.c4,
                                     globalTransform.d1, globalTransform.d2, globalTransform.d3, globalTransform.d4);
    globalInverseTransform.Inverse();
    skeleton->SetGlobalInverseTransform(globalInverseTransform);

    nodes.push(scene->mRootNode);

    while (!nodes.empty())
	{
        const aiNode* node = nodes.front();
        nodes.pop();

        set<string>::iterator s_it = necessaryBones.find(node->mName.data);
        if (s_it != necessaryBones.end())
		{
            const aiNode* parent = node->mParent;

            if (parent == nullptr)
			{
                skeleton->CreateBone(*s_it, Matrix4x4::IDENTITY);
            }
			else
			{
                // Build the bone hierarchy
                Bone_t* parentBone = skeleton->FindBoneByName(parent->mName.data);
                if (parentBone == nullptr)
				{
                    parentBone = skeleton->CreateBone(parent->mName.data, Matrix4x4::IDENTITY);
                }

                auto it = bones.find(node->mName.data);
                if (it == bones.end())
				{
                    aiMatrix4x4 offset = node->mTransformation;
                    Matrix4x4 offsetMatrix(offset.a1, offset.a2, offset.a3, offset.a4,
                                           offset.b1, offset.b2, offset.b3, offset.b4,
                                           offset.c1, offset.c2, offset.c3, offset.c4,
                                           offset.d1, offset.d2, offset.d3, offset.d4);

                    Bone_t* bone = skeleton->CreateBone(*s_it, offsetMatrix);
                    parentBone->linkedBones.push_back(bone);
                }
				else
				{
                    aiMatrix4x4 offset = it->second->mOffsetMatrix;
                    Matrix4x4 offsetMatrix(offset.a1, offset.a2, offset.a3, offset.a4,
                                           offset.b1, offset.b2, offset.b3, offset.b4,
                                           offset.c1, offset.c2, offset.c3, offset.c4,
                                           offset.d1, offset.d2, offset.d3, offset.d4);

                    Bone_t* bone = skeleton->CreateBone(it->first, offsetMatrix);
                    parentBone->linkedBones.push_back(bone);
                }
            }
        }

        for (size_t i = 0; i < node->mNumChildren; i++)
		{
            nodes.push(node->mChildren[i]);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////
    // Load the animations
    for (size_t i = 0; i < scene->mNumAnimations; i++)
	{
        aiAnimation* animation = scene->mAnimations[i];
        AnimationState animationState(animation->mDuration, animation->mTicksPerSecond);
        
        // Get the keys for all bones
        for (size_t j = 0; j < animation->mNumChannels; j++)
		{
            const aiNodeAnim* nodeAnim = animation->mChannels[j];

            vector<pair<double, Vector3>> positionKeys, scaleKeys;
            vector<pair<double, Quaternion>> rotationKeys;

            // Get all the key frames
            for (size_t k = 0; k < nodeAnim->mNumPositionKeys; k++)
			{
                const aiVectorKey& positionKey = nodeAnim->mPositionKeys[k];
                const aiVector3D& position = positionKey.mValue;
                positionKeys.push_back(pair<double, Vector3>( positionKey.mTime, Vector3(position.x, position.y, position.z) ));
            }

            for (size_t k = 0; k < nodeAnim->mNumRotationKeys; k++)
			{
                const aiQuatKey& rotationKey = nodeAnim->mRotationKeys[k];
                const aiQuaternion& rotation = rotationKey.mValue;
                rotationKeys.push_back(pair<double, Quaternion>( rotationKey.mTime, Quaternion(rotation.w, rotation.x, rotation.y, rotation.z) ));
            }

            for (size_t k = 0; k < nodeAnim->mNumScalingKeys; k++)
			{
                const aiVectorKey& scalingKey = nodeAnim->mScalingKeys[k];
                const aiVector3D& scale = scalingKey.mValue;
                scaleKeys.push_back(pair<double, Vector3>( scalingKey.mTime, Vector3(scale.x, scale.y, scale.z) ));
            }

            animationState.SetPositionKeysForBone(nodeAnim->mNodeName.data, positionKeys);
            animationState.SetRotationKeysForBone(nodeAnim->mNodeName.data, rotationKeys);
            animationState.SetScaleKeysForBone(nodeAnim->mNodeName.data, scaleKeys);
        }

        skeleton->AddAnimationState(animation->mName.data, animationState);
    }

	assimpImporter->FreeScene();

    return true;
}

}