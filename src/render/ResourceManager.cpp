#include "render/ResourceManager.h"

#include "render/Renderer.h"

#include "system/Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <FreeImage.h>

#include <queue>
using namespace std;

namespace Sketch3D {

ResourceManager ResourceManager::instance_;

const unsigned int ResourceManager::POST_PROCESS_NORMAL = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals;
const unsigned int ResourceManager::POST_PROCESS_UV	= aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |aiProcess_GenUVCoords;
const unsigned int ResourceManager::POST_PROCESS_NORMAL_UV = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_GenUVCoords;
const unsigned int ResourceManager::POST_PROCESS_NORMAL_TANGENT = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;
const unsigned int ResourceManager::POST_PROCESS_NORMAL_TANGENT_UV = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_GenUVCoords;

ResourceManager::ResourceManager() {
	Logger::GetInstance()->Info("Initializing ResourceManager");
	importer_ = new Assimp::Importer();
	SetBaseFilePath("./");
}

ResourceManager::~ResourceManager() {
	Logger::GetInstance()->Info("Freeing all loaded resources from ResourceManager");
	Logger::GetInstance()->Info("Freeing models from ResourceManager");
	delete importer_;

    map<size_t, vector<LoadedModel_t*>>::iterator m_it = models_.begin();
    for (; m_it != models_.end(); ++m_it) {
        for (size_t i = 0; i < m_it->second.size(); i++) {
            delete m_it->second[i];
        }
    }

	Logger::GetInstance()->Info("Freeing images from ResourceManager");
	map<size_t, pair<Texture2D*, unsigned char*>>::iterator t_it = textures_.begin();
	for (; t_it != textures_.end(); ++t_it) {
        delete t_it->second.first;
        delete t_it->second.second;
	}

	Logger::GetInstance()->Info("Done freeing ResourceManager");
}

ResourceManager* ResourceManager::GetInstance() {
	return &instance_;
}

void ResourceManager::SetBaseFilePath(const string& filePath) {
	baseFilePath_ = filePath;
	Logger::GetInstance()->Info("Setting base file path for resources to: " + baseFilePath_);
}

bool ResourceManager::LoadModel(const string& filename, vector<LoadedModel_t*>*& loadedModel, vector<Texture2D*>*& textures) {
    return LoadModel(filename, POST_PROCESS_NORMAL_UV, loadedModel, textures);
}

bool ResourceManager::LoadModel(const string& filename, unsigned int postProcessingFlags, vector<LoadedModel_t*>*& loadedModel,
                                vector<Texture2D*>*& textures)
{
    hash<string> hashFunction;
    size_t filenameHash = hashFunction(filename);
    bool foundModel = false;
    bool foundTextures = false;

	map<size_t, vector<LoadedModel_t*>>::iterator m_it = models_.find(filenameHash);
	if (m_it != models_.end()) {
        loadedModel = &(m_it->second);
		foundModel = true;
	}

    map<size_t, vector<Texture2D*>>::iterator t_it = modelTextures_.find(filenameHash);
    if (t_it != modelTextures_.end()) {
        textures = &(t_it->second);
        foundTextures = true;
    }

    if (foundModel && foundTextures) {
        return true;
    }

    Logger::GetInstance()->Info("Loading model: " + filename + " ...");

	string path = baseFilePath_ + filename;
	const aiScene* scene = importer_->ReadFile(path, postProcessingFlags);
	if (!scene) {
		Logger::GetInstance()->Error("Couldn't load model : " + string(importer_->GetErrorString()));
		return false;
	}

    if (!foundModel) {
        models_[filenameHash] = vector<LoadedModel_t*>();
        vector<LoadedModel_t*>* model = &models_[filenameHash];
        LoadModelGeometryFromFile(scene, *model);
        loadedModel = &(models_[filenameHash]);

        foundModel = true;
    }

    if (!foundTextures) {
        vector<Texture2D*>* tex;
        LoadTexturesFromFile(filename, tex);
        modelTextures_[filenameHash] = *tex;
        textures = &(modelTextures_[filenameHash]);

        foundTextures = true;
    }

    return foundModel;
}

bool ResourceManager::LoadModelGeometryFromFile(const string& filename, vector<LoadedModel_t*>*& loadedModel) {
	return LoadModelGeometryFromFile(filename, POST_PROCESS_NORMAL_UV, loadedModel);
}

bool ResourceManager::LoadModelGeometryFromFile(const string& filename,
								                unsigned int postProcessingFlags,
								                vector<LoadedModel_t*>*& loadedModel)
{
    hash<string> hashFunction;
    size_t filenameHash = hashFunction(filename);

	map<size_t, vector<LoadedModel_t*>>::iterator it = models_.find(filenameHash);
	if (it != models_.end()) {
        loadedModel = &(it->second);

		return true;
	}

	Logger::GetInstance()->Info("Loading model: " + filename + " ...");

	string path = baseFilePath_ + filename;
	const aiScene* scene = importer_->ReadFile(path, postProcessingFlags);
	if (!scene) {
		Logger::GetInstance()->Error("Couldn't load model : " + string(importer_->GetErrorString()));
		return false;
	}

    models_[filenameHash] = vector<LoadedModel_t*>();
    vector<LoadedModel_t*>* model = &models_[filenameHash];
    LoadModelGeometryFromFile(scene, *model);
    loadedModel = &(models_[filenameHash]);

	return true;
}

bool ResourceManager::LoadTexturesFromFile(const string& filename, vector<Texture2D*>*& textures) {
    hash<string> hashFunction;
    size_t filenameHash = hashFunction(filename);

    map<size_t, vector<Texture2D*>>::iterator it = modelTextures_.find(filenameHash);
    if (it != modelTextures_.end()) {
        textures = &(it->second);
        
        return true;
    }

	string path = baseFilePath_ + filename;
	const aiScene* scene = importer_->ReadFile(path, 0);
	if (!scene) {
		Logger::GetInstance()->Error("Couldn't load model : " + string(importer_->GetErrorString()));
		return false;
	}

    vector<Texture2D*> textures_;
    queue<const aiNode*> nodes;
    nodes.push(scene->mRootNode);
    const aiNode* node = nullptr;

    while (!nodes.empty()) {
        node = nodes.front();
        nodes.pop();

        for (size_t i = 0; i < node->mNumMeshes; i++) {
            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            // TODO
            // Allow only one texture. Find a better way?
            aiString textureName;
            if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                aiReturn ret = material->GetTexture(aiTextureType_DIFFUSE, 0, &textureName);

                if (ret != AI_SUCCESS) {
                    textures_.push_back(nullptr);
                    continue;
                }

                Texture2D* texture = Renderer::GetInstance()->CreateTexture2D();
                if (!LoadTexture(textureName.C_Str(), texture)) {
                    // TODO
                    // Pink texture for invalid texture?
                    delete texture;
                    textures_.push_back(nullptr);
                    continue;
                }

                textures_.push_back(texture);
            }
        }

        for (size_t i = 0; i < node->mNumChildren; i++) {
            nodes.push(node->mChildren[i]);
        }
    }

    modelTextures_[filenameHash] = textures_;
    textures = &(modelTextures_[filenameHash]);

    return true;
}

bool ResourceManager::LoadTexture(const string& filename,
								  Texture2D*& texture)
{
    hash<string> hashFunction;
    size_t filenameHash = hashFunction(filename);

	map<size_t, pair<Texture2D*, unsigned char*>>::iterator it = textures_.find(filenameHash);
	if (it != textures_.end()) {
        texture = it->second.first;
		return true;
	}

	Logger::GetInstance()->Info("Loading image " + filename + " ...");
    string path = baseFilePath_ + filename;

    FREE_IMAGE_FORMAT format = FIF_UNKNOWN;

    format = FreeImage_GetFileType(path.c_str());
    if (format == FIF_UNKNOWN) {
        format = FreeImage_GetFIFFromFilename(path.c_str());
    }

    if ((format == FIF_UNKNOWN) || !FreeImage_FIFSupportsReading(format)) {
        Logger::GetInstance()->Error("File format unsupported for image " + filename);
        return false;
    }

    FIBITMAP* dib = FreeImage_Load(format, path.c_str());

    if (dib == nullptr) {
		Logger::GetInstance()->Error("Couldn't load image " + filename);
		return false;
    }

    size_t width = FreeImage_GetWidth(dib);
    size_t height = FreeImage_GetHeight(dib);
    size_t bpp = FreeImage_GetBPP(dib);

    texture->SetWidth(width);
    texture->SetHeight(height);
    texture->SetFilterMode(FILTER_MODE_BILINEAR);
    texture->SetWrapMode(WRAP_MODE_REPEAT);
    texture->SetTextureFormat((bpp == 24) ? TEXTURE_FORMAT_RGB24 : TEXTURE_FORMAT_RGBA32);

    size_t bytesPerPixel = (bpp == 24) ? 3 : 4;
    unsigned char* data = new unsigned char[width * height * bytesPerPixel];
    memcpy((void*)data, (void*)FreeImage_GetBits(dib), width * height * bytesPerPixel);
    texture->data_ = data;

    FreeImage_Unload(dib);

    if (!texture->Create()) {
        Logger::GetInstance()->Error("Couldn't create texture");
        return false;
    }

	textures_[filenameHash] = pair<Texture2D*, unsigned char*>(texture, data);

	return true;
}

void ResourceManager::LoadModelGeometryFromFile(const aiScene* scene, vector<LoadedModel_t*>& loadedModel) const {
    PreallocateMemory(scene, scene->mRootNode, loadedModel);
	LoadCachedModel(scene, scene->mRootNode, 0, loadedModel);
}

void ResourceManager::PreallocateMemory(const aiScene* scene, const aiNode* node,
                                       vector<LoadedModel_t*>& loadedModel) const
{
    queue<const aiNode*> nodes;
    nodes.push(node);
    const aiNode* nodeToProcess = nullptr;

    while (!nodes.empty()) {
        nodeToProcess = nodes.front();
        nodes.pop();

        for (size_t i = 0; i < nodeToProcess->mNumMeshes; i++) {
            const aiMesh* mesh = scene->mMeshes[nodeToProcess->mMeshes[i]];
            LoadedModel_t* model = new LoadedModel_t;

            if (mesh->HasPositions()) {
                model->vertices.reserve(mesh->mNumVertices * 3);
            }

            if (mesh->HasNormals()) {
                model->normals.reserve(mesh->mNumVertices * 3);
            }

            if (mesh->HasTextureCoords(0)) {
                model->uvs.reserve(mesh->mNumVertices * 2);
            }

            if (mesh->HasTangentsAndBitangents()) {
                model->tangent.reserve(mesh->mNumVertices * 3);
            }

            if (mesh->HasFaces()) {
                model->indices.reserve(mesh->mNumFaces * 3);
            }

            loadedModel.push_back(model);
        }

        for (size_t j = 0; j < nodeToProcess->mNumChildren; j++) {
            nodes.push(nodeToProcess->mChildren[j]);
        }
    }
}

int ResourceManager::LoadCachedModel(const aiScene* scene, const aiNode* node, int index,
									 vector<LoadedModel_t*>& loadedModel) const
{
	for (size_t i = 0; i < node->mNumMeshes; i++) {
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        LoadedModel_t* model = loadedModel[index++];

		LoadMeshVertices(mesh, model);
		LoadMeshIndices(mesh, model);
		LoadMeshNormals(mesh, model);
		LoadMeshTextureCoords(mesh, model);
		LoadMeshTangents(mesh, model);
	}

	for (size_t i = 0; i < node->mNumChildren; i++) {
		index = LoadCachedModel(scene, node->mChildren[i], index, loadedModel);
	}

    return index;
}

void ResourceManager::LoadMeshVertices(const aiMesh* mesh,
									   LoadedModel_t* loadedModel) const
{
	// Each vertex is composed of 3 floats
	if (mesh->HasPositions()) {
		for (size_t j = 0; j < mesh->mNumVertices; j++) {
			const aiVector3D& vertex = mesh->mVertices[j];
			loadedModel->vertices.push_back(vertex.x);
			loadedModel->vertices.push_back(vertex.y);
			loadedModel->vertices.push_back(vertex.z);
		}
	}
}

void ResourceManager::LoadMeshIndices(const aiMesh* mesh,
									  LoadedModel_t* loadedModel) const
{
	if (mesh->HasFaces()) {
		for (size_t j = 0; j < mesh->mNumFaces; j++) {
			const aiFace& face = mesh->mFaces[j];

			for (size_t k = 0; k < face.mNumIndices; k++) {
				loadedModel->indices.push_back(face.mIndices[k]);
			}
		}
	}
}

void ResourceManager::LoadMeshNormals(const aiMesh* mesh,
									  LoadedModel_t* loadedModel) const
{
	if (mesh->HasNormals()) {
		for (size_t j = 0; j < mesh->mNumVertices; j++) {
			const aiVector3D& normal = mesh->mNormals[j];
			loadedModel->normals.push_back(normal.x);
			loadedModel->normals.push_back(normal.y);
			loadedModel->normals.push_back(normal.z);
		}
	}
}

void ResourceManager::LoadMeshTextureCoords(const aiMesh* mesh,
											LoadedModel_t* loadedModel) const
{
	// TODO
	// Support 3D texture coordinate
	if (mesh->HasTextureCoords(0)) {
		for (size_t j = 0; j < mesh->mNumVertices; j++) {
			const aiVector3D& uv = mesh->mTextureCoords[0][j];
			loadedModel->uvs.push_back(uv.x);
			loadedModel->uvs.push_back(uv.y);
		}
	}
}

void ResourceManager::LoadMeshTangents(const aiMesh* mesh,
									   LoadedModel_t* loadedModel) const
{
	// Load the tangents
	if (mesh->HasTangentsAndBitangents()) {
		for (size_t j = 0; j < mesh->mNumVertices; j++) {
			const aiVector3D& tangent = mesh->mTangents[j];
			loadedModel->tangent.push_back(tangent.x);
			loadedModel->tangent.push_back(tangent.y);
			loadedModel->tangent.push_back(tangent.z);
		}
	}
}

}