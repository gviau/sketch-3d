#include "render/ResourceManager.h"

#include "system/Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <IL/il.h>

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
	ilInit();
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
	map<size_t, pair<ILuint, ILubyte*> >::iterator t_it = textures_.begin();
	for (; t_it != textures_.end(); ++t_it) {
		ilDeleteImages(1, &(t_it->second.first));
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

bool ResourceManager::LoadModel(const string& filename, vector<LoadedModel_t*>*& loadedModel) {
	return LoadModel(filename, POST_PROCESS_NORMAL_UV, loadedModel);
}

bool ResourceManager::LoadModel(const string& filename,
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
    PreallocateMemory(scene, scene->mRootNode, *model);
	LoadCachedModel(scene, scene->mRootNode, 0, *model);

    loadedModel = &(models_[filenameHash]);

	return true;
}

bool ResourceManager::LoadTexture(const string& filename,
								  Texture2D* texture)
{
    hash<string> hashFunction;
    size_t filenameHash = hashFunction(filename);

	map<size_t, pair<ILuint, ILubyte*> >::iterator it = textures_.find(filenameHash);
	if (it != textures_.end()) {
		ilBindImage(it->second.first);
		LoadCachedTexture(it->second, texture);
		return true;
	}

	Logger::GetInstance()->Info("Loading image " + filename + " ...");

	ILuint imageName;
	ilGenImages(1, &imageName);
	ilBindImage(imageName);

	string path = baseFilePath_ + filename;
	if (!ilLoadImage(path.c_str())) {
		Logger::GetInstance()->Error("Couldn't load image " + filename);
		texture = NULL;
		ilDeleteImage(imageName);
		return false;
	}

	ILubyte* data = ilGetData();
	if (!data) {
		Logger::GetInstance()->Error("Couldn't access data from image " + filename);
		texture = NULL;
		ilDeleteImage(imageName);
		return false;
	}

	pair<ILuint, ILubyte*> image = pair<ILuint, ILubyte*>(imageName, data);
	textures_[filenameHash] = image;
	LoadCachedTexture(image, texture);

	return true;
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

void ResourceManager::LoadCachedTexture(const pair<ILuint, ILubyte*>& image,
										Texture2D* texture)
{
	unsigned int width = ilGetInteger(IL_IMAGE_WIDTH);
	unsigned int height = ilGetInteger(IL_IMAGE_HEIGHT);
	unsigned int bytesPerPixel = ilGetInteger(IL_IMAGE_BPP);
	TextureFormat_t format = (bytesPerPixel == 3) ? TEXTURE_FORMAT_RGB24 : TEXTURE_FORMAT_RGBA32;

	size_t textureSize = width * height * bytesPerPixel;
	texture->data_.resize(textureSize);
	memcpy(&texture->data_[0], image.second, textureSize);

	texture->SetWidth(width);
	texture->SetHeight(height);
	texture->SetTextureFormat(format);
}

}