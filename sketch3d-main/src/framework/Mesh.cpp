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

void FillVertexBuffer(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer);
void FillVertexBuffer_Pos(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);
void FillVertexBuffer_Pos_Color(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);
void FillVertexBuffer_Pos_UV(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);
void FillVertexBuffer_Pos_Normal(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);
void FillVertexBuffer_Pos_UV_Normal(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);
void FillVertexBuffer_Pos_UV_Normal_Tangent(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);
void FillVertexBuffer_Pos_UV_Normal_4_Bones(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);
void FillVertexBuffer_Pos_UV_Normal_Tangent_4_Bones(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);
void FillVertexBuffer_Pos_UV_4_Bones(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);
void FillVertexBuffer_Pos_Normal_4_Bones(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);
void FillVertexBuffer_Pos_2_UV(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);
void FillVertexBuffer_Pos_2_UV_Normal(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);
void FillVertexBuffer_Pos_2_UV_Normal_Tangent(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);
void FillVertexBuffer_Pos_2_UV_Normal_4_Bones(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);
void FillVertexBuffer_Pos_2_UV_Normal_Tangent_4_Bones(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);
void FillVertexBuffer_Pos_2_UV_4_Bones(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType);

void FillIndexBuffer(const aiMesh* mesh, const shared_ptr<IndexBuffer>& indexBuffer);
VertexFormatType_t GetMeshVertexFormatType(const aiMesh* mesh);

bool LoadMeshFromFileInternal(const string& filename, const shared_ptr<RenderDevice>& renderDevice, shared_ptr<Mesh>& loadedMesh, bool loadMaterial, MaterialCodeGenerator* materialCodeGenerator, bool calculateTangents);
void LoadMaterial(HardwareResourceCreator* hardwareResourceCreator, const aiMaterial* material, const shared_ptr<Material>& newMaterial, MaterialCodeGenerator* materialCodeGenerator, VertexFormatType_t vertexFormatType);
bool LoadTextureFromMaterial(aiTextureType textureType, HardwareResourceCreator* hardwareResourceCreator, const aiMaterial* material, shared_ptr<Texture2D>& texture, shared_ptr<SamplerState>& samplerState);

// Create a single instance of an Assimp importer to load meshes from files
unique_ptr<Assimp::Importer> assimpImporter(new Assimp::Importer);

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

const vector<shared_ptr<SubMesh>>& Mesh::GetSubMeshes() const
{
    return m_SubMeshes;
}

shared_ptr<SubMesh> Mesh::GetSubMesh(size_t index) const
{
    assert(index < m_SubMeshes.size());
    return m_SubMeshes[index];
}

bool LoadMeshFromFile(const string& filename, const shared_ptr<RenderDevice>& renderDevice, shared_ptr<Mesh>& loadedMesh, bool calculateTangents)
{
    return LoadMeshFromFileInternal(filename, renderDevice, loadedMesh, false, nullptr, calculateTangents);
}

bool LoadMeshFromFileWithMaterial(const string& filename, const shared_ptr<RenderDevice>& renderDevice, shared_ptr<Mesh>& loadedMesh,
                                  MaterialCodeGenerator* materialCodeGenerator, bool calculateTangents)
{
    return LoadMeshFromFileInternal(filename, renderDevice, loadedMesh, (materialCodeGenerator != nullptr), materialCodeGenerator, calculateTangents);
}

bool LoadMeshFromFileInternal(const string& filename, const shared_ptr<RenderDevice>& renderDevice, shared_ptr<Mesh>& loadedMesh, bool loadMaterials,
                              MaterialCodeGenerator* materialCodeGenerator, bool calculateTangents)
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

            shared_ptr<VertexBuffer> vertexBuffer = hardwareResourceCreator->CreateVertexBuffer();
            FillVertexBuffer(mesh, vertexBuffer);
            subMesh->SetVertexBuffer(vertexBuffer);

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

                    LoadMaterial(hardwareResourceCreator, scene->mMaterials[materialIndex], material, materialCodeGenerator, vertexBuffer->GetVertexFormatType());

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

                    material->Initialize(subMesh->GetVertexBuffer()->GetVertexFormatType(), materialCodeGenerator);
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

    return true;
}

void FillVertexBuffer(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer)
{
    VertexFormatType_t vertexFormatType = GetMeshVertexFormatType(mesh);

    switch (vertexFormatType)
    {
    case VertexFormatType_t::Pos:                               FillVertexBuffer_Pos(mesh, vertexBuffer, vertexFormatType); break;
    case VertexFormatType_t::Pos_Color:                         FillVertexBuffer_Pos_Color(mesh, vertexBuffer, vertexFormatType); break;
    case VertexFormatType_t::Pos_UV:                            FillVertexBuffer_Pos_UV(mesh, vertexBuffer, vertexFormatType); break;
    case VertexFormatType_t::Pos_Normal:                        FillVertexBuffer_Pos_Normal(mesh, vertexBuffer, vertexFormatType); break;
    case VertexFormatType_t::Pos_UV_Normal:                     FillVertexBuffer_Pos_UV_Normal(mesh, vertexBuffer, vertexFormatType); break;
    case VertexFormatType_t::Pos_UV_Normal_Tangent:             FillVertexBuffer_Pos_UV_Normal_Tangent(mesh, vertexBuffer, vertexFormatType); break;
    case VertexFormatType_t::Pos_UV_Normal_4_Bones:             FillVertexBuffer_Pos_UV_Normal_4_Bones(mesh, vertexBuffer, VertexFormatType_t::Pos_UV_Normal); break;
    case VertexFormatType_t::Pos_UV_Normal_Tangent_4_Bones:     FillVertexBuffer_Pos_UV_Normal_Tangent_4_Bones(mesh, vertexBuffer, vertexFormatType); break;
    case VertexFormatType_t::Pos_UV_4_Bones:                    FillVertexBuffer_Pos_UV_4_Bones(mesh, vertexBuffer, vertexFormatType); break;
    case VertexFormatType_t::Pos_Normal_4_Bones:                FillVertexBuffer_Pos_Normal_4_Bones(mesh, vertexBuffer, vertexFormatType); break;
    case VertexFormatType_t::Pos_2_UV:                          FillVertexBuffer_Pos_2_UV(mesh, vertexBuffer, vertexFormatType); break;
    case VertexFormatType_t::Pos_2_UV_Normal:                   FillVertexBuffer_Pos_2_UV_Normal(mesh, vertexBuffer, vertexFormatType); break;
    case VertexFormatType_t::Pos_2_UV_Normal_Tangent:           FillVertexBuffer_Pos_2_UV_Normal_Tangent(mesh, vertexBuffer, vertexFormatType); break;
    case VertexFormatType_t::Pos_2_UV_Normal_4_Bones:           FillVertexBuffer_Pos_2_UV_Normal_4_Bones(mesh, vertexBuffer, vertexFormatType); break;
    case VertexFormatType_t::Pos_2_UV_Normal_Tangent_4_Bones:   FillVertexBuffer_Pos_2_UV_Normal_Tangent_4_Bones(mesh, vertexBuffer, vertexFormatType); break;
    case VertexFormatType_t::Pos_2_UV_4_Bones:                  FillVertexBuffer_Pos_2_UV_4_Bones(mesh, vertexBuffer, vertexFormatType); break;
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
    
    if (fabs(specularPower) > EPSILON)
    {
        newMaterial->SetSpecularPower(specularPower);
    }

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

VertexFormatType_t GetMeshVertexFormatType(const aiMesh* mesh)
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
                        if (mesh->HasBones())
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
                        if (mesh->HasBones())
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
                    if (mesh->HasBones())
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
                        if (mesh->HasBones())
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
                        if (mesh->HasBones())
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
                    if (mesh->HasBones())
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
                if (mesh->HasBones())
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

void FillVertexBuffer_Pos(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_t> vertices;
    vertices.reserve(mesh->mNumVertices);

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        const aiVector3D& position = mesh->mVertices[i];

        Vertex_Pos_t vertex;
        vertex.position.x = position.x;
        vertex.position.y = position.y;
        vertex.position.z = position.z;

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

void FillVertexBuffer_Pos_Color(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_Color_t> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

void FillVertexBuffer_Pos_UV(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_UV_t> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

void FillVertexBuffer_Pos_Normal(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_Normal_t> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

void FillVertexBuffer_Pos_UV_Normal(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_UV_Normal_t> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

void FillVertexBuffer_Pos_UV_Normal_Tangent(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_UV_Normal_Tangent_t> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

void FillVertexBuffer_Pos_UV_Normal_4_Bones(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_UV_Normal_t> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

void FillVertexBuffer_Pos_UV_Normal_Tangent_4_Bones(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_UV_Normal_Tangent_t> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

void FillVertexBuffer_Pos_UV_4_Bones(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_UV_t> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

void FillVertexBuffer_Pos_Normal_4_Bones(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_Normal_t> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

void FillVertexBuffer_Pos_2_UV(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_2_UV_t> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

void FillVertexBuffer_Pos_2_UV_Normal(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_2_UV_Normal_t> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

void FillVertexBuffer_Pos_2_UV_Normal_Tangent(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_2_UV_Normal_Tangent_t> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

void FillVertexBuffer_Pos_2_UV_Normal_4_Bones(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_2_UV_Normal_t> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

void FillVertexBuffer_Pos_2_UV_Normal_Tangent_4_Bones(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_2_UV_Normal_Tangent_t> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

void FillVertexBuffer_Pos_2_UV_4_Bones(const aiMesh* mesh, const shared_ptr<VertexBuffer>& vertexBuffer, VertexFormatType_t vertexFormatType)
{
    vector<Vertex_Pos_2_UV_t> vertices;
    vertices.reserve(mesh->mNumVertices);

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

        vertices.push_back(vertex);
    }

    vertexBuffer->Initialize((void*)&vertices[0], false, false, vertexFormatType, mesh->mNumVertices);
}

}