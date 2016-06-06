#include "framework/Mesh.h"

#include "framework/Material.h"
#include "framework/MaterialCodeGenerator.h"
#include "framework/SubMesh.h"

#include "render/Buffer.h"
#include "render/HardwareResourceCreator.h"
#include "render/RenderDevice.h"
#include "render/SamplerState.h"
#include "render/Shader.h"
#include "render/Texture.h"
#include "render/TextureMap.h"

#include "system/Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <cassert>
#include <fstream>
#include <queue>
using namespace std;

namespace Sketch3D {

void FillVertexData(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos_Color(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos_UV(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos_Normal(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos_UV_Normal(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos_UV_Normal_Tangent(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos_UV_Normal_4_Bones(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos_UV_Normal_Tangent_4_Bones(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos_UV_4_Bones(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos_Normal_4_Bones(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos_2_UV(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos_2_UV_Normal(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos_2_UV_Normal_Tangent(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos_2_UV_Normal_4_Bones(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos_2_UV_Normal_Tangent_4_Bones(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);
void FillVertexData_Pos_2_UV_4_Bones(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType);

void FillIndexBuffer(const aiMesh* mesh, const shared_ptr<IndexBuffer>& indexBuffer);
VertexFormatType_t GetMeshVertexFormatType(const aiMesh* mesh, bool loadBones);

void InitializeVertexBuffersInMesh(HardwareResourceCreator* hardwareResourceCreator, const shared_ptr<Mesh>& mesh,
								   const vector<pair<void*, size_t>>& vertexDataPerSubMesh, const vector<VertexFormatType_t>& vertexFormatTypePerSubMesh);

bool CheckIfMeshMaterialHasNormalMap(const aiScene* scene);

bool LoadMeshFromFileInternal(const string& filename, const shared_ptr<RenderDevice>& renderDevice, bool loadMaterial,
							  MaterialCodeGenerator* materialCodeGenerator, bool calculateTangents, bool loadBones,
							  shared_ptr<Mesh>& loadedMesh, vector<pair<void*, size_t>>& vertexDataPerSubMesh,
							  vector<VertexFormatType_t>& vertexFormatTypePerSubMesh);
void LoadMaterial(HardwareResourceCreator* hardwareResourceCreator, const aiMaterial* material, const shared_ptr<Material>& newMaterial, MaterialCodeGenerator* materialCodeGenerator, VertexFormatType_t vertexFormatType);
bool LoadTextureFromMaterial(aiTextureType textureType, HardwareResourceCreator* hardwareResourceCreator, const aiMaterial* material, shared_ptr<Texture2D>& texture, shared_ptr<SamplerState>& samplerState);

// Create a single instance of an Assimp importer to load meshes from files
unique_ptr<Assimp::Importer> assimpImporter(new Assimp::Importer);

Mesh::Mesh()
	: m_IsSkinnedMesh(false)
{
}

void Mesh::Draw(const shared_ptr<RenderDevice>& renderDevice) const
{
    for (const shared_ptr<SubMesh>& subMesh : m_SubMeshes)
    {
        subMesh->Draw(renderDevice);
    }
}

void Mesh::AddSubMesh(const shared_ptr<SubMesh>& subMesh)
{
    m_SubMeshes.push_back(subMesh);
}

void Mesh::RemoveSubMesh(size_t index)
{
    assert(index < m_SubMeshes.size());
    m_SubMeshes.erase(m_SubMeshes.begin() + index);
}

void Mesh::ClearSubMeshes()
{
    m_SubMeshes.clear();
}

void Mesh::SetConstantBuffersForAllMaterials(const vector<shared_ptr<ConstantBuffer>>& vertexShaderConstantBuffers,
                                             const vector<shared_ptr<ConstantBuffer>>& fragmentShaderConstantBuffers,
                                             size_t materialConstantsSlot)
{
    for (const shared_ptr<SubMesh>& subMesh : m_SubMeshes)
    {
        SubMesh* pSubMesh = subMesh.get();
        if (pSubMesh == nullptr)
        {
            continue;
        }

        Material* material = pSubMesh->GetMaterial().get(); 
        if (material == nullptr)
        {
            continue;
        }

        material->SetVertexShaderConstantBuffers(vertexShaderConstantBuffers);
        material->SetFragmentShaderConstantBuffers(fragmentShaderConstantBuffers);
        material->SetMaterialConstantsBufferSlot(materialConstantsSlot);
    }
}

void Mesh::SetMaterialForAllSubMeshes(const shared_ptr<Material>& material)
{
    for (const shared_ptr<SubMesh>& subMesh : m_SubMeshes)
    {
        SubMesh* pSubMesh = subMesh.get();
        if (subMesh == nullptr)
        {
            continue;
        }

        subMesh->SetMaterial(material);
    }
}

void Mesh::SetAmbientColorForAllSubMeshes(const Vector3& ambientColor)
{
    for (const shared_ptr<SubMesh>& subMesh : m_SubMeshes)
    {
        SubMesh* pSubMesh = subMesh.get();
        if (subMesh == nullptr)
        {
            continue;
        }

        Material* material = pSubMesh->GetMaterial().get();
        if (material == nullptr)
        {
            continue;
        }

        material->SetAmbientColor(ambientColor);
    }
}

void Mesh::SetDiffuseColorForAllSubMeshes(const Vector3& diffuseColor)
{
    for (const shared_ptr<SubMesh>& subMesh : m_SubMeshes)
    {
        SubMesh* pSubMesh = subMesh.get();
        if (subMesh == nullptr)
        {
            continue;
        }

        Material* material = pSubMesh->GetMaterial().get();
        if (material == nullptr)
        {
            continue;
        }

        material->SetDiffuseColor(diffuseColor);
    }
}

void Mesh::SetSpecularColorForAllSubMeshes(const Vector3& specularColor)
{
    for (const shared_ptr<SubMesh>& subMesh : m_SubMeshes)
    {
        SubMesh* pSubMesh = subMesh.get();
        if (subMesh == nullptr)
        {
            continue;
        }

        Material* material = pSubMesh->GetMaterial().get();
        if (material == nullptr)
        {
            continue;
        }

        material->SetSpecularColor(specularColor);
    }
}

void Mesh::SetSpecularPowerForAllSubMeshes(float power)
{
    for (const shared_ptr<SubMesh>& subMesh : m_SubMeshes)
    {
        SubMesh* pSubMesh = subMesh.get();
        if (subMesh == nullptr)
        {
            continue;
        }

        Material* material = pSubMesh->GetMaterial().get();
        if (material == nullptr)
        {
            continue;
        }

        material->SetSpecularPower(power);
    }
}

const vector<shared_ptr<SubMesh>>& Mesh::GetSubMeshes() const
{
    return m_SubMeshes;
}

shared_ptr<SubMesh> Mesh::GetSubMesh(size_t index) const
{
    assert(index < m_SubMeshes.size());
    return m_SubMeshes[index];
}

void InitializeVertexBuffersInMesh(HardwareResourceCreator* hardwareResourceCreator, const shared_ptr<Mesh>& mesh,
								   const vector<pair<void*, size_t>>& vertexDataPerSubMesh, const vector<VertexFormatType_t>& vertexFormatTypePerSubMesh)
{
	const vector<shared_ptr<SubMesh>>& subMeshes = mesh->GetSubMeshes();
	for (size_t i = 0; i < subMeshes.size(); i++)
	{
		const shared_ptr<SubMesh>& subMesh = subMeshes[i];

		const pair<void*, size_t>& vertexData = vertexDataPerSubMesh[i];
		void* vertices = vertexData.first;
		size_t numVertices = vertexData.second;

		VertexFormatType_t vertexFormatType = vertexFormatTypePerSubMesh[i];

		shared_ptr<VertexBuffer> vertexBuffer = hardwareResourceCreator->CreateVertexBuffer();
		vertexBuffer->Initialize(vertices, false, false, vertexFormatType, numVertices);

		subMesh->SetVertexBuffer(vertexBuffer);
	}
}

bool LoadMeshFromFile(const string& filename, const shared_ptr<RenderDevice>& renderDevice, shared_ptr<Mesh>& loadedMesh, bool calculateTangents)
{
	vector<pair<void*, size_t>> vertexDataPerSubMesh;
	vector<VertexFormatType_t> vertexFormatTypePerSubMesh;
	bool meshLoadedSuccessfully = LoadMeshFromFileInternal(filename, renderDevice, false, nullptr, calculateTangents, false,
														  loadedMesh, vertexDataPerSubMesh, vertexFormatTypePerSubMesh);

	if (meshLoadedSuccessfully)
	{
		HardwareResourceCreator* hardwareResourceCreator = renderDevice->GetHardwareResourceCreator();
		InitializeVertexBuffersInMesh(hardwareResourceCreator, loadedMesh, vertexDataPerSubMesh, vertexFormatTypePerSubMesh);
	}

	return meshLoadedSuccessfully;
}

bool LoadMeshFromFileWithMaterial(const string& filename, const shared_ptr<RenderDevice>& renderDevice, shared_ptr<Mesh>& loadedMesh,
                                  MaterialCodeGenerator* materialCodeGenerator, bool calculateTangents)
{
	vector<pair<void*, size_t>> vertexDataPerSubMesh;
	vector<VertexFormatType_t> vertexFormatTypePerSubMesh;
    bool meshLoadedSuccessfully = LoadMeshFromFileInternal(filename, renderDevice, (materialCodeGenerator != nullptr), materialCodeGenerator,
														   calculateTangents, false, loadedMesh, vertexDataPerSubMesh, vertexFormatTypePerSubMesh);

	if (meshLoadedSuccessfully)
	{
		HardwareResourceCreator* hardwareResourceCreator = renderDevice->GetHardwareResourceCreator();
		InitializeVertexBuffersInMesh(hardwareResourceCreator, loadedMesh, vertexDataPerSubMesh, vertexFormatTypePerSubMesh);
	}

	return meshLoadedSuccessfully;
}

bool LoadMeshFromFileInternal(const string& filename, const shared_ptr<RenderDevice>& renderDevice, bool loadMaterials,
                              MaterialCodeGenerator* materialCodeGenerator, bool calculateTangents, bool loadBones,
							  shared_ptr<Mesh>& loadedMesh, vector<pair<void*, size_t>>& vertexDataPerSubMesh,
							  vector<VertexFormatType_t>& vertexFormatTypePerSubMesh)
{
    Assimp::Importer* importer = assimpImporter.get();
    if (importer == nullptr)
    {
        Logger::GetInstance()->Error("Assimp's importer is null");
        return false;
    }

    unsigned int flags = aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals |
                         aiProcess_GenUVCoords;
    const aiScene* scene = importer->ReadFile(filename, flags);
    if (scene == nullptr)
    {
        Logger::GetInstance()->Error("Couldn't load mesh " + filename);
        return false;
    }

    // If the meshes' materials has a normal map and there is no tangents in the mesh, then we have to calculate them
    if (loadMaterials && CheckIfMeshMaterialHasNormalMap(scene))
    {
        calculateTangents = true;
    }

    // Little hack to generate tangents if they weren't present and if we asked for it
    if (calculateTangents)
    {
        queue<const aiNode*> nodes;
        nodes.push(scene->mRootNode);

        bool hasTangents = false;
        while (!nodes.empty())
        {
            const aiNode* node = nodes.front();
            nodes.pop();

            for (size_t i = 0; i < node->mNumMeshes; i++)
            {
                const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

                if (mesh->HasTangentsAndBitangents())
                {
                    hasTangents = true;
                    break;
                }
            }

            if (hasTangents)
            {
                break;
            }

            for (size_t i = 0; i < node->mNumChildren; i++)
            {
                nodes.push(node->mChildren[i]);
            }
        }

        if (!hasTangents)
        {
            importer->ApplyPostProcessing(aiProcess_CalcTangentSpace);
        }
    }

    HardwareResourceCreator* hardwareResourceCreator = renderDevice->GetHardwareResourceCreator();

    vector<shared_ptr<Material>> createdMaterials;

    queue<const aiNode*> nodes;
    nodes.push(scene->mRootNode);

    loadedMesh.reset(new Mesh);

    while (!nodes.empty())
    {
        const aiNode* node = nodes.front();
        nodes.pop();

        for (size_t i = 0; i < node->mNumMeshes; i++)
        {
            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            shared_ptr<SubMesh> subMesh(new SubMesh);

			void* vertexData = nullptr;
			VertexFormatType_t vertexFormatType = GetMeshVertexFormatType(mesh, loadBones);
            FillVertexData(mesh, vertexData, vertexFormatType);

			vertexDataPerSubMesh.push_back(pair<void*, size_t>(vertexData, mesh->mNumVertices));
			vertexFormatTypePerSubMesh.push_back(vertexFormatType);

            if (mesh->HasFaces())
            {
                shared_ptr<IndexBuffer> indexBuffer = hardwareResourceCreator->CreateIndexBuffer();
                FillIndexBuffer(mesh, indexBuffer);
                subMesh->SetIndexBuffer(indexBuffer);
            }

            if (loadMaterials && scene->HasMaterials())
            {
                size_t materialIndex = mesh->mMaterialIndex;
                if (materialIndex < createdMaterials.size() && createdMaterials[materialIndex].get() != nullptr)
                {
                    subMesh->SetMaterial(createdMaterials[mesh->mMaterialIndex]);
                }
                else
                {
                    shared_ptr<Material> material(new Material(renderDevice));

                    LoadMaterial(hardwareResourceCreator, scene->mMaterials[materialIndex], material, materialCodeGenerator, vertexFormatType);

                    if (materialIndex < createdMaterials.size())
                    {
                        createdMaterials[materialIndex] = material;
                    }
                    else
                    {
                        size_t size = createdMaterials.size();
                        for (size_t j = size; j < materialIndex; j++)
                        {
                            createdMaterials.push_back(shared_ptr<Material>(nullptr));
                        }

                        createdMaterials.push_back(material);
                    }

                    material->Initialize(vertexFormatType, materialCodeGenerator);
                    subMesh->SetMaterial(material);
                }
            }

            loadedMesh->AddSubMesh(subMesh);
        }

        for (size_t i = 0; i < node->mNumChildren; i++)
        {
            nodes.push(node->mChildren[i]);
        }
    }

    Logger::GetInstance()->Info("Successfully loaded mesh " + filename);

	assimpImporter->FreeScene();

    return true;
}

void FillVertexData(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
    switch (vertexFormatType)
    {
    case VertexFormatType_t::Pos:                               FillVertexData_Pos(mesh, vertexData, vertexFormatType); break;
    case VertexFormatType_t::Pos_Color:                         FillVertexData_Pos_Color(mesh, vertexData, vertexFormatType); break;
    case VertexFormatType_t::Pos_UV:                            FillVertexData_Pos_UV(mesh, vertexData, vertexFormatType); break;
    case VertexFormatType_t::Pos_Normal:                        FillVertexData_Pos_Normal(mesh, vertexData, vertexFormatType); break;
    case VertexFormatType_t::Pos_UV_Normal:                     FillVertexData_Pos_UV_Normal(mesh, vertexData, vertexFormatType); break;
    case VertexFormatType_t::Pos_UV_Normal_Tangent:             FillVertexData_Pos_UV_Normal_Tangent(mesh, vertexData, vertexFormatType); break;
    case VertexFormatType_t::Pos_UV_Normal_4_Bones:             FillVertexData_Pos_UV_Normal_4_Bones(mesh, vertexData, VertexFormatType_t::Pos_UV_Normal); break;
    case VertexFormatType_t::Pos_UV_Normal_Tangent_4_Bones:     FillVertexData_Pos_UV_Normal_Tangent_4_Bones(mesh, vertexData, VertexFormatType_t::Pos_UV_Normal_Tangent); break;
    case VertexFormatType_t::Pos_UV_4_Bones:                    FillVertexData_Pos_UV_4_Bones(mesh, vertexData, vertexFormatType); break;
    case VertexFormatType_t::Pos_Normal_4_Bones:                FillVertexData_Pos_Normal_4_Bones(mesh, vertexData, vertexFormatType); break;
    case VertexFormatType_t::Pos_2_UV:                          FillVertexData_Pos_2_UV(mesh, vertexData, vertexFormatType); break;
    case VertexFormatType_t::Pos_2_UV_Normal:                   FillVertexData_Pos_2_UV_Normal(mesh, vertexData, vertexFormatType); break;
    case VertexFormatType_t::Pos_2_UV_Normal_Tangent:           FillVertexData_Pos_2_UV_Normal_Tangent(mesh, vertexData, vertexFormatType); break;
    case VertexFormatType_t::Pos_2_UV_Normal_4_Bones:           FillVertexData_Pos_2_UV_Normal_4_Bones(mesh, vertexData, vertexFormatType); break;
    case VertexFormatType_t::Pos_2_UV_Normal_Tangent_4_Bones:   FillVertexData_Pos_2_UV_Normal_Tangent_4_Bones(mesh, vertexData, vertexFormatType); break;
    case VertexFormatType_t::Pos_2_UV_4_Bones:                  FillVertexData_Pos_2_UV_4_Bones(mesh, vertexData, vertexFormatType); break;
    }
}

void FillIndexBuffer(const aiMesh* mesh, const shared_ptr<IndexBuffer>& indexBuffer)
{
    // This is a bit stupid, but here we traverse all the indices to get the largest one to know what is the size of the indices
    unsigned int largestIndex = 0;
    unsigned int numIndices = 0;
    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];
        
        for (size_t j = 0; j < face.mNumIndices; j++)
        {
            if (face.mIndices[j] > largestIndex)
            {
                largestIndex = face.mIndices[j];
            }
        }

        numIndices += face.mNumIndices;
    }

    if (largestIndex > 0xffff)
    {
        vector<unsigned int> indices;
        indices.reserve(numIndices);

        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            const aiFace& face = mesh->mFaces[i];

            for (size_t j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        indexBuffer->Initialize((void*)&indices[0], false, false, IndexFormat_t::INT_4_BYTES, numIndices);
    }
    else
    {
        vector<unsigned short> indices;
        indices.reserve(numIndices);

        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            const aiFace& face = mesh->mFaces[i];

            for (size_t j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back((unsigned short)face.mIndices[j]);
            }
        }

        indexBuffer->Initialize((void*)&indices[0], false, false, IndexFormat_t::INT_2_BYTES, numIndices);
    }
}

void LoadMaterial(HardwareResourceCreator* hardwareResourceCreator, const aiMaterial* material, const shared_ptr<Material>& newMaterial,
                  MaterialCodeGenerator* materialCodeGenerator, VertexFormatType_t vertexFormatType)
{
    aiColor3D ambientColor(0.0f, 0.0f, 0.0f);
    material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
    Vector3 ambientColorVec(ambientColor.r, ambientColor.g, ambientColor.b);

    if (ambientColorVec != Vector3::ZERO)
    {
        newMaterial->SetAmbientColor(ambientColorVec);
    }

    aiColor3D diffuseColor(0.0f, 0.0f, 0.0f);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
    Vector3 diffuseColorVec(diffuseColor.r, diffuseColor.g, diffuseColor.b);

    if (diffuseColorVec != Vector3::ZERO)
    {
        newMaterial->SetDiffuseColor(diffuseColorVec);
    }

    aiColor3D specularColor(0.0f, 0.0f, 0.0f);
    material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
    Vector3 specularColorVec(specularColor.r, specularColor.g, specularColor.b);

    if (specularColorVec != Vector3::ZERO)
    {
        newMaterial->SetSpecularColor(specularColorVec);
    }

    float specularPower = 0.0f;
    material->Get(AI_MATKEY_SHININESS, specularPower);
    newMaterial->SetSpecularPower(specularPower);

    shared_ptr<Texture2D> ambientTexture;
    shared_ptr<SamplerState> ambientSamplerState;
    if (LoadTextureFromMaterial(aiTextureType::aiTextureType_AMBIENT, hardwareResourceCreator, material, ambientTexture, ambientSamplerState))
    {
        newMaterial->SetAmbientTexture(ambientTexture);
        newMaterial->SetAmbientSamplerState(ambientSamplerState);
    }

    shared_ptr<Texture2D> diffuseTexture;
    shared_ptr<SamplerState> diffuseSamplerState;
    if (LoadTextureFromMaterial(aiTextureType::aiTextureType_DIFFUSE, hardwareResourceCreator, material, diffuseTexture, diffuseSamplerState))
    {
        newMaterial->SetDiffuseTexture(diffuseTexture);
        newMaterial->SetDiffuseSamplerState(diffuseSamplerState);
    }

    shared_ptr<Texture2D> specularTexture;
    shared_ptr<SamplerState> specularSamplerState;
    if (LoadTextureFromMaterial(aiTextureType::aiTextureType_SPECULAR, hardwareResourceCreator, material, specularTexture, specularSamplerState))
    {
        newMaterial->SetSpecularTexture(specularTexture);
        newMaterial->SetSpecularSamplerState(specularSamplerState);
    }

    shared_ptr<Texture2D> normalMapTexture;
    shared_ptr<SamplerState> normalMapSamplerState;
    if (LoadTextureFromMaterial(aiTextureType::aiTextureType_NORMALS, hardwareResourceCreator, material, normalMapTexture, normalMapSamplerState))
    {
        newMaterial->SetNormalMapTexture(normalMapTexture);
        newMaterial->SetNormalMapSamplerState(normalMapSamplerState);
    }
}

bool LoadTextureFromMaterial(aiTextureType textureType, HardwareResourceCreator* hardwareResourceCreator, const aiMaterial* material, shared_ptr<Texture2D>& texture, shared_ptr<SamplerState>& samplerState)
{
    if (material->GetTextureCount(textureType) > 0)
    {
        aiString textureName;
        aiTextureMapMode mapModes[] = { aiTextureMapMode::aiTextureMapMode_Wrap, aiTextureMapMode::aiTextureMapMode_Wrap, aiTextureMapMode::aiTextureMapMode_Wrap };
        material->GetTexture(textureType, 0, &textureName, nullptr, nullptr, nullptr, nullptr, mapModes);

        string textureFilename = textureName.C_Str();

        shared_ptr<TextureMap> textureMap;
        TextureFormat_t textureFormat;
        LoadTextureMapFromFile("Media/" + textureFilename, textureMap, textureFormat);

        texture = hardwareResourceCreator->CreateTexture2D();
        texture->Initialize(textureMap.get(), textureFormat, false, false);

        AddressMode_t addressModeU;
        AddressMode_t addressModeV;
        AddressMode_t addressModeW;

        for (size_t i = 0; i < 3; i++)
        {
            AddressMode_t* addressMode = &addressModeU;
            if (i == 1)
            {
                addressMode = &addressModeV;
            }
            else if (i == 2)
            {
                addressMode = &addressModeW;
            }

            switch (mapModes[i])
            {
            case aiTextureMapMode::aiTextureMapMode_Clamp:  *addressMode = AddressMode_t::CLAMP; break;
            case aiTextureMapMode::aiTextureMapMode_Wrap:   *addressMode = AddressMode_t::WRAP; break;
            case aiTextureMapMode::aiTextureMapMode_Mirror: *addressMode = AddressMode_t::MIRROR; break;
            }
        }

        samplerState = hardwareResourceCreator->CreateSamplerState();
        samplerState->Initialize(FilterMode_t::LINEAR, addressModeU, addressModeV, addressModeW, ComparisonFunction_t::LESS);

        return true;
    }

    return false;
}

VertexFormatType_t GetMeshVertexFormatType(const aiMesh* mesh, bool loadBones)
{
    if (mesh->HasPositions())
    {
        if (mesh->HasTextureCoords(0))
        {
            if (mesh->HasTextureCoords(1))
            {
                if (mesh->HasNormals())
                {
                    if (mesh->HasTangentsAndBitangents())
                    {
                        if (loadBones && mesh->HasBones())
                        {
                            return VertexFormatType_t::Pos_2_UV_Normal_Tangent_4_Bones;
                        }
                        else
                        {
                            return VertexFormatType_t::Pos_2_UV_Normal_Tangent;
                        }
                    }
                    else
                    {
                        if (loadBones && mesh->HasBones())
                        {
                            return VertexFormatType_t::Pos_2_UV_Normal_4_Bones;
                        }
                        else
                        {
                            return VertexFormatType_t::Pos_2_UV_Normal;
                        }
                    }
                }
                else
                {
                    if (loadBones && mesh->HasBones())
                    {
                        return VertexFormatType_t::Pos_2_UV_4_Bones;
                    }
                    else
                    {
                        return VertexFormatType_t::Pos_2_UV;
                    }
                }
            }
            else
            {
                if (mesh->HasNormals())
                {
                    if (mesh->HasTangentsAndBitangents())
                    {
                        if (loadBones && mesh->HasBones())
                        {
                            return VertexFormatType_t::Pos_UV_Normal_Tangent_4_Bones;
                        }
                        else
                        {
                            return VertexFormatType_t::Pos_UV_Normal_Tangent;
                        }
                    }
                    else
                    {
                        if (loadBones && mesh->HasBones())
                        {
                            return VertexFormatType_t::Pos_UV_Normal_4_Bones;
                        }
                        else
                        {
                            return VertexFormatType_t::Pos_UV_Normal;
                        }
                    }
                }
                else
                {
                    if (loadBones && mesh->HasBones())
                    {
                        return VertexFormatType_t::Pos_UV_4_Bones;
                    }
                    else
                    {
                        return VertexFormatType_t::Pos_UV;
                    }
                }
            }
        }
        else
        {
            if (mesh->HasNormals())
            {
                if (loadBones && mesh->HasBones())
                {
                    return VertexFormatType_t::Pos_Normal_4_Bones;
                }
                else
                {
                    return VertexFormatType_t::Pos_Normal;
                }
            }
            else
            {
                if (mesh->HasVertexColors(0))
                {
                    return VertexFormatType_t::Pos_Color;
                }
                else
                {
                    return VertexFormatType_t::Pos;
                }
            }
        }
    }

    // Shouldn't happen
    Logger::GetInstance()->Error("Couldn't find a valid vertex format");
    return VertexFormatType_t::Pos;
}

bool CheckIfMeshMaterialHasNormalMap(const aiScene* scene)
{
    if (!scene->HasMaterials())
    {
        return false;
    }

    queue<const aiNode*> nodes;
    nodes.push(scene->mRootNode);

    while (!nodes.empty())
    {
        const aiNode* currentNode = nodes.front();
        nodes.pop();

        for (size_t i = 0; i < currentNode->mNumMeshes; i++)
        {
            const aiMesh* mesh = scene->mMeshes[currentNode->mMeshes[i]];
            const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
            {
                return true;
            }
        }

        for (size_t i = 0; i < currentNode->mNumChildren; i++)
        {
            nodes.push(currentNode->mChildren[i]);
        }
    }

    return false;
}

void FillVertexData_Pos(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_t[mesh->mNumVertices];
	Vertex_Pos_t* vertices = (Vertex_Pos_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];

        Vertex_Pos_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

		vertices[i] = vertex;
    }
}

void FillVertexData_Pos_Color(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_Color_t[mesh->mNumVertices];
	Vertex_Pos_Color_t* vertices = (Vertex_Pos_Color_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiColor4D* color = mesh->mColors[0];

        Vertex_Pos_Color_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertex.color.x = color->r;
        vertex.color.y = color->g;
        vertex.color.z = color->b;

        vertices[i] = vertex;
    }
}

void FillVertexData_Pos_UV(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_UV_t[mesh->mNumVertices];
	Vertex_Pos_UV_t* vertices = (Vertex_Pos_UV_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& uv = mesh->mTextureCoords[0][i];

        Vertex_Pos_UV_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertex.uv.x = uv.x;
        vertex.uv.y = uv.y;

        vertices[i] = vertex;
    }
}

void FillVertexData_Pos_Normal(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_Normal_t[mesh->mNumVertices];
	Vertex_Pos_Normal_t* vertices = (Vertex_Pos_Normal_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& normal = mesh->mNormals[i];

        Vertex_Pos_Normal_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertex.normal.x = normal.x;
        vertex.normal.y = normal.y;
        vertex.normal.z = normal.z;

        vertices[i] = vertex;
    }
}

void FillVertexData_Pos_UV_Normal(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_UV_Normal_t[mesh->mNumVertices];
	Vertex_Pos_UV_Normal_t* vertices = (Vertex_Pos_UV_Normal_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& uv = mesh->mTextureCoords[0][i];
        const aiVector3D& normal = mesh->mNormals[i];

        Vertex_Pos_UV_Normal_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertex.uv.x = uv.x;
        vertex.uv.y = uv.y;

        vertex.normal.x = normal.x;
        vertex.normal.y = normal.y;
        vertex.normal.z = normal.z;

        vertices[i] = vertex;
    }
}

void FillVertexData_Pos_UV_Normal_Tangent(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_UV_Normal_Tangent_t[mesh->mNumVertices];
	Vertex_Pos_UV_Normal_Tangent_t* vertices = (Vertex_Pos_UV_Normal_Tangent_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& uv = mesh->mTextureCoords[0][i];
        const aiVector3D& normal = mesh->mNormals[i];
        const aiVector3D& tangent = mesh->mTangents[i];

        Vertex_Pos_UV_Normal_Tangent_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertex.uv.x = uv.x;
        vertex.uv.y = uv.y;

        vertex.normal.x = normal.x;
        vertex.normal.y = normal.y;
        vertex.normal.z = normal.z;

        vertex.tangent.x = tangent.x;
        vertex.tangent.y = tangent.y;
        vertex.tangent.z = tangent.z;

        vertices[i] = vertex;
    }
}

void FillVertexData_Pos_UV_Normal_4_Bones(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_UV_Normal_4_Bones_t[mesh->mNumVertices];
	Vertex_Pos_UV_Normal_4_Bones_t* vertices = (Vertex_Pos_UV_Normal_4_Bones_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& uv = mesh->mTextureCoords[0][i];
        const aiVector3D& normal = mesh->mNormals[i];

        Vertex_Pos_UV_Normal_4_Bones_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertex.uv.x = uv.x;
        vertex.uv.y = uv.y;

        vertex.normal.x = normal.x;
        vertex.normal.y = normal.y;
        vertex.normal.z = normal.z;

        vertices[i] = vertex;

		vertices[i].bones.w = 0.0f;
		vertices[i].weights.w = 0.0f;
    }
}

void FillVertexData_Pos_UV_Normal_Tangent_4_Bones(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_UV_Normal_Tangent_4_Bones_t[mesh->mNumVertices];
	Vertex_Pos_UV_Normal_Tangent_4_Bones_t* vertices = (Vertex_Pos_UV_Normal_Tangent_4_Bones_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& uv = mesh->mTextureCoords[0][i];
        const aiVector3D& normal = mesh->mNormals[i];
        const aiVector3D& tangent = mesh->mTangents[i];

        Vertex_Pos_UV_Normal_Tangent_4_Bones_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertex.uv.x = uv.x;
        vertex.uv.y = uv.y;

        vertex.normal.x = normal.x;
        vertex.normal.y = normal.y;
        vertex.normal.z = normal.z;

        vertex.tangent.x = tangent.x;
        vertex.tangent.y = tangent.y;
        vertex.tangent.z = tangent.z;

        vertices[i] = vertex;

		vertices[i].bones.w = 0.0f;
		vertices[i].weights.w = 0.0f;
    }
}

void FillVertexData_Pos_UV_4_Bones(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_UV_4_Bones_t[mesh->mNumVertices];
	Vertex_Pos_UV_4_Bones_t* vertices = (Vertex_Pos_UV_4_Bones_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& uv = mesh->mTextureCoords[0][i];

        Vertex_Pos_UV_4_Bones_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertex.uv.x = uv.x;
        vertex.uv.y = uv.y;

        vertices[i] = vertex;

		vertices[i].bones.w = 0.0f;
		vertices[i].weights.w = 0.0f;
    }
}

void FillVertexData_Pos_Normal_4_Bones(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_Normal_4_Bones_t[mesh->mNumVertices];
	Vertex_Pos_Normal_4_Bones_t* vertices = (Vertex_Pos_Normal_4_Bones_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& normal = mesh->mNormals[i];

        Vertex_Pos_Normal_4_Bones_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertex.normal.x = normal.x;
        vertex.normal.y = normal.y;
        vertex.normal.z = normal.z;

        vertices[i] = vertex;

		vertices[i].bones.w = 0.0f;
		vertices[i].weights.w = 0.0f;
    }
}

void FillVertexData_Pos_2_UV(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_2_UV_t[mesh->mNumVertices];
	Vertex_Pos_2_UV_t* vertices = (Vertex_Pos_2_UV_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& uv1 = mesh->mTextureCoords[0][i];
        const aiVector3D& uv2 = mesh->mTextureCoords[1][i];

        Vertex_Pos_2_UV_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertex.uv1.x = uv1.x;
        vertex.uv1.y = uv1.y;

        vertex.uv2.x = uv2.x;
        vertex.uv2.y = uv2.y;

        vertices[i] = vertex;
    }
}

void FillVertexData_Pos_2_UV_Normal(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_2_UV_Normal_t[mesh->mNumVertices];
	Vertex_Pos_2_UV_Normal_t* vertices = (Vertex_Pos_2_UV_Normal_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& uv1 = mesh->mTextureCoords[0][i];
        const aiVector3D& uv2 = mesh->mTextureCoords[1][i];
        const aiVector3D& normal = mesh->mNormals[i];

        Vertex_Pos_2_UV_Normal_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertex.uv1.x = uv1.x;
        vertex.uv1.y = uv1.y;

        vertex.uv2.x = uv2.x;
        vertex.uv2.y = uv2.y;

        vertex.normal.x = normal.x;
        vertex.normal.y = normal.y;
        vertex.normal.z = normal.z;

        vertices[i] = vertex;
    }
}

void FillVertexData_Pos_2_UV_Normal_Tangent(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_2_UV_Normal_Tangent_t[mesh->mNumVertices];
	Vertex_Pos_2_UV_Normal_Tangent_t* vertices = (Vertex_Pos_2_UV_Normal_Tangent_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& uv1 = mesh->mTextureCoords[0][i];
        const aiVector3D& uv2 = mesh->mTextureCoords[1][i];
        const aiVector3D& normal = mesh->mNormals[i];
        const aiVector3D& tangent = mesh->mTangents[i];

        Vertex_Pos_2_UV_Normal_Tangent_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertex.uv1.x = uv1.x;
        vertex.uv1.y = uv1.y;

        vertex.uv2.x = uv2.x;
        vertex.uv2.y = uv2.y;

        vertex.normal.x = normal.x;
        vertex.normal.y = normal.y;
        vertex.normal.z = normal.z;

        vertex.tangent.x = tangent.x;
        vertex.tangent.y = tangent.y;
        vertex.tangent.z = tangent.z;

        vertices[i] = vertex;
    }
}

void FillVertexData_Pos_2_UV_Normal_4_Bones(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_2_UV_Normal_4_Bones_t[mesh->mNumVertices];
	Vertex_Pos_2_UV_Normal_4_Bones_t* vertices = (Vertex_Pos_2_UV_Normal_4_Bones_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& uv1 = mesh->mTextureCoords[0][i];
        const aiVector3D& uv2 = mesh->mTextureCoords[1][i];
        const aiVector3D& normal = mesh->mNormals[i];

        Vertex_Pos_2_UV_Normal_4_Bones_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertex.uv1.x = uv1.x;
        vertex.uv1.y = uv1.y;

        vertex.uv2.x = uv2.x;
        vertex.uv2.y = uv2.y;

        vertex.normal.x = normal.x;
        vertex.normal.y = normal.y;
        vertex.normal.z = normal.z;

        vertices[i] = vertex;

		vertices[i].bones.w = 0.0f;
		vertices[i].weights.w = 0.0f;
    }
}

void FillVertexData_Pos_2_UV_Normal_Tangent_4_Bones(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_2_UV_Normal_Tangent_4_Bones_t[mesh->mNumVertices];
    Vertex_Pos_2_UV_Normal_Tangent_4_Bones_t* vertices = (Vertex_Pos_2_UV_Normal_Tangent_4_Bones_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& uv1 = mesh->mTextureCoords[0][i];
        const aiVector3D& uv2 = mesh->mTextureCoords[1][i];
        const aiVector3D& normal = mesh->mNormals[i];
        const aiVector3D& tangent = mesh->mTangents[i];

        Vertex_Pos_2_UV_Normal_Tangent_4_Bones_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertex.uv1.x = uv1.x;
        vertex.uv1.y = uv1.y;

        vertex.uv2.x = uv2.x;
        vertex.uv2.y = uv2.y;

        vertex.normal.x = normal.x;
        vertex.normal.y = normal.y;
        vertex.normal.z = normal.z;

        vertex.tangent.x = tangent.x;
        vertex.tangent.y = tangent.y;
        vertex.tangent.z = tangent.z;

        vertices[i] = vertex;

		vertices[i].bones.w = 0.0f;
		vertices[i].weights.w = 0.0f;
    }
}

void FillVertexData_Pos_2_UV_4_Bones(const aiMesh* mesh, void*& vertexData, VertexFormatType_t vertexFormatType)
{
	vertexData = new Vertex_Pos_2_UV_4_Bones_t[mesh->mNumVertices];
    Vertex_Pos_2_UV_4_Bones_t* vertices = (Vertex_Pos_2_UV_4_Bones_t*)vertexData;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];
        const aiVector3D& uv1 = mesh->mTextureCoords[0][i];
        const aiVector3D& uv2 = mesh->mTextureCoords[1][i];

        Vertex_Pos_2_UV_4_Bones_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertex.uv1.x = uv1.x;
        vertex.uv1.y = uv1.y;

        vertex.uv2.x = uv2.x;
        vertex.uv2.y = uv2.y;

        vertices[i] = vertex;

		vertices[i].bones.w = 0.0f;
		vertices[i].weights.w = 0.0f;
    }
}

}