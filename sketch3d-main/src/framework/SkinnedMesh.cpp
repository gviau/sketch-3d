#include "framework/SkinnedMesh.h"

#include "framework/AnimationState.h"
#include "framework/Skeleton.h"

#include "render/RenderDevice.h"

#include "system/Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <map>
#include <queue>
#include <string>
using namespace std;

namespace Sketch3D
{

// Create a single instance of an Assimp importer to load meshes from files
extern unique_ptr<Assimp::Importer> assimpImporter;

extern bool LoadMeshFromFileInternal(const string& filename, const shared_ptr<RenderDevice>& renderDevice, bool loadMaterial,
									 MaterialCodeGenerator* materialCodeGenerator, bool calculateTangents, bool loadBones,
									 shared_ptr<Mesh>& loadedMesh, vector<pair<void*, size_t>>& vertexDataPerSubMesh,
									 vector<VertexFormatType_t>& vertexFormatTypePerSubMesh);
extern void InitializeVertexBuffersInMesh(HardwareResourceCreator* hardwareResourceCreator, const shared_ptr<Mesh>& mesh,
										  const vector<pair<void*, size_t>>& vertexDataPerSubMesh,
										  const vector<VertexFormatType_t>& vertexFormatTypePerSubMesh);
extern bool LoadSkeletonFromFileInternal(const string& filename, shared_ptr<Skeleton>& skeleton, map<string, size_t>& bonesNameToIndex);

bool VerifyVertexFormatTypes(const vector<VertexFormatType_t>& vertexFormatTypePerSubMesh);
void MapVerticesToBones(const string& filename, const vector<pair<void*, size_t>>& vertexDataPerSubMesh,
						const vector<VertexFormatType_t>& vertexFormatTypePerSubMesh, const map<string, size_t>& bonesNameToIndex);
void MapVerticesToBones_Pos_UV_Normal_4_Bones(const aiBone* bone, void* vertexData, size_t numVertices, const map<string, size_t>& bonesNameToIndex);
void MapVerticesToBones_Pos_UV_Normal_Tangent_4_Bones(const aiBone* bone, void* vertexData, size_t numVertices, const map<string, size_t>& bonesNameToIndex);
void MapVerticesToBones_Pos_UV_4_Bones(const aiBone* bone, void* vertexData, size_t numVertices, const map<string, size_t>& bonesNameToIndex);
void MapVerticesToBones_Pos_Normal_4_Bones(const aiBone* bone, void* vertexData, size_t numVertices, const map<string, size_t>& bonesNameToIndex);
void MapVerticesToBones_Pos_2_UV_Normal_4_Bones(const aiBone* bone, void* vertexData, size_t numVertices, const map<string, size_t>& bonesNameToIndex);
void MapVerticesToBones_Pos_2_UV_Normal_Tangent_4_Bones(const aiBone* bone, void* vertexData, size_t numVertices, const map<string, size_t>& bonesNameToIndex);
void MapVerticesToBones_Pos_2_UV_4_Bones(const aiBone* bone, void* vertexData, size_t numVertices, const map<string, size_t>& bonesNameToIndex);
void AssignBoneAndWeightToVertex(size_t boneIndex, float boneWeight, Vector4& contribuingBones, Vector4& contribuingWeights);

SkinnedMesh::SkinnedMesh()
    : m_Time(0.0)
    , m_CurrentAnimationState(nullptr)
	, m_IsAnimating(false)
    , m_IsLooping(false)
{
	m_IsSkinnedMesh = true;
}

SkinnedMesh::SkinnedMesh(const Mesh& mesh)
	: m_Time(0.0)
	, m_CurrentAnimationState(nullptr)
	, m_IsAnimating(false)
	, m_IsLooping(false)
{
	m_SubMeshes = mesh.GetSubMeshes();

	m_IsSkinnedMesh = true;
}

bool SkinnedMesh::Animate(double deltaTime)
{
	if (m_CurrentAnimationState == nullptr || !m_IsAnimating)
	{
		return false;
	}

	m_Time += deltaTime;
	map<const Bone_t*, Matrix4x4> transformationMatrices;
	if (!m_Skeleton->GetTransformationMatrices(m_Time, m_CurrentAnimationState, m_IsLooping, transformationMatrices))
	{
		return false;
	}

	for (auto it = m_BoneToIndex.begin(); it != m_BoneToIndex.end(); ++it)
	{
		m_TransformationMatrices[it->second] = transformationMatrices[it->first];
	}

    return true;
}

void SkinnedMesh::SetAnimationState(const string& name)
{
	m_CurrentAnimationState = m_Skeleton->GetAnimationState(name);
	m_Time = 0.0;
}

void SkinnedMesh::SetAnimationLoop(bool looping)
{
	m_IsLooping = looping;
}

void SkinnedMesh::StartAnimation()
{
	m_IsAnimating = true;
	m_Time = 0.0;
}

void SkinnedMesh::StopAnimation()
{
	m_IsAnimating = false;
}

bool SkinnedMesh::IsAnimationRunning() const
{
    return (m_CurrentAnimationState != nullptr);
}

void SkinnedMesh::SetSkeleton(const shared_ptr<Skeleton>& skeleton, const map<string, size_t>& bonesNameToIndex)
{
	m_Skeleton = skeleton;

	m_TransformationMatrices.clear();
	m_TransformationMatrices.reserve(m_Skeleton->GetNumberOfBones());
	m_TransformationMatrices.resize(m_Skeleton->GetNumberOfBones());

	for (auto it = bonesNameToIndex.begin(); it != bonesNameToIndex.end(); ++it)
	{
		Bone_t* bone = m_Skeleton->FindBoneByName(it->first);
		m_BoneToIndex[bone] = it->second;
	}
}

bool LoadSkinnedMeshFromFile(const string& filename, const shared_ptr<RenderDevice>& renderDevice, shared_ptr<SkinnedMesh>& loadedMesh,
                             bool calculateTangents)
{
	shared_ptr<Mesh> meshPointer(loadedMesh);
	vector<pair<void*, size_t>> vertexDataPerSubMesh;
	vector<VertexFormatType_t> vertexFormatTypePerSubMesh;

	bool meshLoadedSuccessfully = LoadMeshFromFileInternal(filename, renderDevice, false, nullptr, calculateTangents, true,
														   meshPointer, vertexDataPerSubMesh, vertexFormatTypePerSubMesh);

	if (!meshLoadedSuccessfully)
	{
		return false;
	}

	if (!VerifyVertexFormatTypes(vertexFormatTypePerSubMesh))
	{
		Logger::GetInstance()->Error("Skinned mesh " + filename + " doesn't contain any bones!");
		return false;
	}
	
	shared_ptr<Skeleton> skeleton;
	map<string, size_t> bonesNameToIndex;
	bool skeletonLoadedSuccessfully = LoadSkeletonFromFileInternal(filename, skeleton, bonesNameToIndex);

	if (!skeletonLoadedSuccessfully)
	{
		return false;
	}

	MapVerticesToBones(filename, vertexDataPerSubMesh, vertexFormatTypePerSubMesh, bonesNameToIndex);

	HardwareResourceCreator* hardwareResourceCreator = renderDevice->GetHardwareResourceCreator();
	InitializeVertexBuffersInMesh(hardwareResourceCreator, meshPointer, vertexDataPerSubMesh, vertexFormatTypePerSubMesh);

	loadedMesh = shared_ptr<SkinnedMesh>(new SkinnedMesh(*meshPointer));
	loadedMesh->SetSkeleton(skeleton, bonesNameToIndex);

	return true;
}

bool LoadSkinnedMeshFromFileWithMaterial(const string& filename, const shared_ptr<RenderDevice>& renderDevice, shared_ptr<SkinnedMesh>& loadedMesh,
                                         MaterialCodeGenerator* materialCodeGenerator, bool calculateTangents)
{
	shared_ptr<Mesh> meshPointer(loadedMesh);
	vector<pair<void*, size_t>> vertexDataPerSubMesh;
	vector<VertexFormatType_t> vertexFormatTypePerSubMesh;

	bool meshLoadedSuccessfully = LoadMeshFromFileInternal(filename, renderDevice, (materialCodeGenerator != nullptr), materialCodeGenerator,
														   calculateTangents, true, meshPointer, vertexDataPerSubMesh, vertexFormatTypePerSubMesh);

	if (!meshLoadedSuccessfully)
	{
		return false;
	}

	if (!VerifyVertexFormatTypes(vertexFormatTypePerSubMesh))
	{
		Logger::GetInstance()->Error("Skinned mesh " + filename + " doesn't contain any bones!");
		return false;
	}

	shared_ptr<Skeleton> skeleton;
	map<string, size_t> bonesNameToIndex;
	bool skeletonLoadedSuccessfully = LoadSkeletonFromFileInternal(filename, skeleton, bonesNameToIndex);

	if (!skeletonLoadedSuccessfully)
	{
		return false;
	}

	MapVerticesToBones(filename, vertexDataPerSubMesh, vertexFormatTypePerSubMesh, bonesNameToIndex);

	HardwareResourceCreator* hardwareResourceCreator = renderDevice->GetHardwareResourceCreator();
	InitializeVertexBuffersInMesh(hardwareResourceCreator, meshPointer, vertexDataPerSubMesh, vertexFormatTypePerSubMesh);

	loadedMesh = shared_ptr<SkinnedMesh>(new SkinnedMesh(*meshPointer));
	loadedMesh->SetSkeleton(skeleton, bonesNameToIndex);

	return true;
}

bool VerifyVertexFormatTypes(const vector<VertexFormatType_t>& vertexFormatTypePerSubMesh)
{
	for (const VertexFormatType_t& vertexFormatType : vertexFormatTypePerSubMesh)
	{
		if (!VertexFormatTypeContainsBones(vertexFormatType))
		{
			return false;
		}
	}

	return true;
}

void MapVerticesToBones(const string& filename, const vector<pair<void*, size_t>>& vertexDataPerSubMesh,
						const vector<VertexFormatType_t>& vertexFormatTypePerSubMesh, const map<string, size_t>& bonesNameToIndex)
{
	const aiScene* scene = assimpImporter->ReadFile(filename, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_LimitBoneWeights);

	if (scene == nullptr)
	{
		Logger::GetInstance()->Error("Couldn't load skinned mesh " + filename);
		return;
	}

	queue<const aiNode*> nodes;
	nodes.push(scene->mRootNode);

	while (!nodes.empty())
	{
		const aiNode* node = nodes.front();
		nodes.pop();

		for (size_t i = 0; i < node->mNumMeshes; i++)
		{
			const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			if (mesh->HasBones())
			{
				VertexFormatType_t vertexFormatType = vertexFormatTypePerSubMesh[node->mMeshes[i]];

				void* vertexData = vertexDataPerSubMesh[node->mMeshes[i]].first;
				size_t numVertices = vertexDataPerSubMesh[node->mMeshes[i]].second;

				for (size_t j = 0; j < mesh->mNumBones; j++)
				{
					const aiBone* bone = mesh->mBones[j];

					switch (vertexFormatType)
					{
					case VertexFormatType_t::Pos_Normal_4_Bones:				MapVerticesToBones_Pos_Normal_4_Bones(bone, vertexData, numVertices, bonesNameToIndex); break;
					case VertexFormatType_t::Pos_UV_4_Bones:					MapVerticesToBones_Pos_UV_4_Bones(bone, vertexData, numVertices, bonesNameToIndex); break;
					case VertexFormatType_t::Pos_UV_Normal_4_Bones:				MapVerticesToBones_Pos_UV_Normal_4_Bones(bone, vertexData, numVertices, bonesNameToIndex); break;
					case VertexFormatType_t::Pos_UV_Normal_Tangent_4_Bones:		MapVerticesToBones_Pos_UV_Normal_Tangent_4_Bones(bone, vertexData, numVertices, bonesNameToIndex); break;
					case VertexFormatType_t::Pos_2_UV_4_Bones:					MapVerticesToBones_Pos_2_UV_4_Bones(bone, vertexData, numVertices, bonesNameToIndex); break;
					case VertexFormatType_t::Pos_2_UV_Normal_4_Bones:			MapVerticesToBones_Pos_2_UV_Normal_4_Bones(bone, vertexData, numVertices, bonesNameToIndex); break;
					case VertexFormatType_t::Pos_2_UV_Normal_Tangent_4_Bones:	MapVerticesToBones_Pos_2_UV_Normal_Tangent_4_Bones(bone, vertexData, numVertices, bonesNameToIndex); break;
					}
				}
			}
		}

		for (size_t i = 0; i < node->mNumChildren; i++)
		{
			nodes.push(node->mChildren[i]);
		}
	}

	assimpImporter->FreeScene();
}

void MapVerticesToBones_Pos_UV_Normal_4_Bones(const aiBone* bone, void* vertexData, size_t numVertices, const map<string, size_t>& bonesNameToIndex)
{
	Vertex_Pos_UV_Normal_4_Bones_t* vertices = (Vertex_Pos_UV_Normal_4_Bones_t*)vertexData;

	auto it = bonesNameToIndex.find(bone->mName.data);
	if (it == bonesNameToIndex.end())
	{
		return;
	}

	size_t boneIndex = it->second;

	// Assign the bones' index and weights to the corresponding vertices
	for (size_t i = 0; i < bone->mNumWeights; i++)
	{
		const aiVertexWeight& vertexWeight = bone->mWeights[i];
		Vertex_Pos_UV_Normal_4_Bones_t& vertex = vertices[vertexWeight.mVertexId];

		AssignBoneAndWeightToVertex(boneIndex, vertexWeight.mWeight, vertex.bones, vertex.weights);
	}
}

void MapVerticesToBones_Pos_UV_Normal_Tangent_4_Bones(const aiBone* bone, void* vertexData, size_t numVertices, const map<string, size_t>& bonesNameToIndex)
{
	Vertex_Pos_UV_Normal_Tangent_4_Bones_t* vertices = (Vertex_Pos_UV_Normal_Tangent_4_Bones_t*)vertexData;

	auto it = bonesNameToIndex.find(bone->mName.data);
	if (it == bonesNameToIndex.end())
	{
		return;
	}

	size_t boneIndex = it->second;

	for (size_t i = 0; i < numVertices; i++)
	{
		Vertex_Pos_UV_Normal_Tangent_4_Bones_t& vertex = vertices[i];

		// Assign the bones' index and weights to the corresponding vertices
		for (size_t j = 0; j < bone->mNumWeights; j++)
		{
			const aiVertexWeight& vertexWeight = bone->mWeights[j];

			AssignBoneAndWeightToVertex(boneIndex, vertexWeight.mWeight, vertex.bones, vertex.weights);
		}
	}
}

void MapVerticesToBones_Pos_UV_4_Bones(const aiBone* bone, void* vertexData, size_t numVertices, const map<string, size_t>& bonesNameToIndex)
{
	Vertex_Pos_UV_4_Bones_t* vertices = (Vertex_Pos_UV_4_Bones_t*)vertexData;

	auto it = bonesNameToIndex.find(bone->mName.data);
	if (it == bonesNameToIndex.end())
	{
		return;
	}

	size_t boneIndex = it->second;

	for (size_t i = 0; i < numVertices; i++)
	{
		Vertex_Pos_UV_4_Bones_t& vertex = vertices[i];

		// Assign the bones' index and weights to the corresponding vertices
		for (size_t j = 0; j < bone->mNumWeights; j++)
		{
			const aiVertexWeight& vertexWeight = bone->mWeights[j];

			AssignBoneAndWeightToVertex(boneIndex, vertexWeight.mWeight, vertex.bones, vertex.weights);
		}
	}
}

void MapVerticesToBones_Pos_Normal_4_Bones(const aiBone* bone, void* vertexData, size_t numVertices, const map<string, size_t>& bonesNameToIndex)
{
	Vertex_Pos_Normal_4_Bones_t* vertices = (Vertex_Pos_Normal_4_Bones_t*)vertexData;

	auto it = bonesNameToIndex.find(bone->mName.data);
	if (it == bonesNameToIndex.end())
	{
		return;
	}

	size_t boneIndex = it->second;

	for (size_t i = 0; i < numVertices; i++)
	{
		Vertex_Pos_Normal_4_Bones_t& vertex = vertices[i];

		// Assign the bones' index and weights to the corresponding vertices
		for (size_t j = 0; j < bone->mNumWeights; j++)
		{
			const aiVertexWeight& vertexWeight = bone->mWeights[j];

			AssignBoneAndWeightToVertex(boneIndex, vertexWeight.mWeight, vertex.bones, vertex.weights);
		}
	}
}

void MapVerticesToBones_Pos_2_UV_Normal_4_Bones(const aiBone* bone, void* vertexData, size_t numVertices, const map<string, size_t>& bonesNameToIndex)
{
	Vertex_Pos_2_UV_Normal_4_Bones_t* vertices = (Vertex_Pos_2_UV_Normal_4_Bones_t*)vertexData;

	auto it = bonesNameToIndex.find(bone->mName.data);
	if (it == bonesNameToIndex.end())
	{
		return;
	}

	size_t boneIndex = it->second;

	for (size_t i = 0; i < numVertices; i++)
	{
		Vertex_Pos_2_UV_Normal_4_Bones_t& vertex = vertices[i];

		// Assign the bones' index and weights to the corresponding vertices
		for (size_t j = 0; j < bone->mNumWeights; j++)
		{
			const aiVertexWeight& vertexWeight = bone->mWeights[j];

			AssignBoneAndWeightToVertex(boneIndex, vertexWeight.mWeight, vertex.bones, vertex.weights);
		}
	}
}

void MapVerticesToBones_Pos_2_UV_Normal_Tangent_4_Bones(const aiBone* bone, void* vertexData, size_t numVertices, const map<string, size_t>& bonesNameToIndex)
{
	Vertex_Pos_2_UV_Normal_Tangent_4_Bones_t* vertices = (Vertex_Pos_2_UV_Normal_Tangent_4_Bones_t*)vertexData;

	auto it = bonesNameToIndex.find(bone->mName.data);
	if (it == bonesNameToIndex.end())
	{
		return;
	}

	size_t boneIndex = it->second;

	for (size_t i = 0; i < numVertices; i++)
	{
		Vertex_Pos_2_UV_Normal_Tangent_4_Bones_t& vertex = vertices[i];

		// Assign the bones' index and weights to the corresponding vertices
		for (size_t j = 0; j < bone->mNumWeights; j++)
		{
			const aiVertexWeight& vertexWeight = bone->mWeights[j];

			AssignBoneAndWeightToVertex(boneIndex, vertexWeight.mWeight, vertex.bones, vertex.weights);
		}
	}
}

void MapVerticesToBones_Pos_2_UV_4_Bones(const aiBone* bone, void* vertexData, size_t numVertices, const map<string, size_t>& bonesNameToIndex)
{
	Vertex_Pos_2_UV_4_Bones_t* vertices = (Vertex_Pos_2_UV_4_Bones_t*)vertexData;

	auto it = bonesNameToIndex.find(bone->mName.data);
	if (it == bonesNameToIndex.end())
	{
		return;
	}

	size_t boneIndex = it->second;

	for (size_t i = 0; i < numVertices; i++)
	{
		Vertex_Pos_2_UV_4_Bones_t& vertex = vertices[i];

		// Assign the bones' index and weights to the corresponding vertices
		for (size_t j = 0; j < bone->mNumWeights; j++)
		{
			const aiVertexWeight& vertexWeight = bone->mWeights[j];

			AssignBoneAndWeightToVertex(boneIndex, vertexWeight.mWeight, vertex.bones, vertex.weights);
		}
	}
}

void AssignBoneAndWeightToVertex(size_t boneIndex, float boneWeight, Vector4& contribuingBones, Vector4& contribuingWeights)
{
	if (contribuingWeights.x == 0.0f)
	{
		contribuingBones.x = (float)boneIndex;
		contribuingWeights.x = boneWeight;
	}
	else if (contribuingWeights.y == 0.0f)
	{
		contribuingBones.y = (float)boneIndex;
		contribuingWeights.y = boneWeight;
	}
	else if (contribuingWeights.z == 0.0f)
	{
		contribuingBones.z = (float)boneIndex;
		contribuingWeights.z = boneWeight;
	}
	else
	{
		contribuingBones.w = (float)boneIndex;
		contribuingWeights.w = boneWeight;
	}
}

}