#include "mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <spdlog/spdlog.h>
#include <vector>

constexpr const char *getTextureTypeName(const aiTextureType type)
	{
#define TEXTURE_TYPE(textureType) textureType: return #textureType
		switch (type)
			{
				case TEXTURE_TYPE(aiTextureType_DIFFUSE);
				case TEXTURE_TYPE(aiTextureType_NORMALS);
				case TEXTURE_TYPE(aiTextureType_METALNESS);
				case TEXTURE_TYPE(aiTextureType_DIFFUSE_ROUGHNESS);
				case TEXTURE_TYPE(aiTextureType_AMBIENT_OCCLUSION);
				case TEXTURE_TYPE(aiTextureType_NONE);
				case TEXTURE_TYPE(aiTextureType_SPECULAR);
				case TEXTURE_TYPE(aiTextureType_AMBIENT);
				case TEXTURE_TYPE(aiTextureType_EMISSIVE);
				case TEXTURE_TYPE(aiTextureType_HEIGHT);
				case TEXTURE_TYPE(aiTextureType_SHININESS);
				case TEXTURE_TYPE(aiTextureType_OPACITY);
				case TEXTURE_TYPE(aiTextureType_DISPLACEMENT);
				case TEXTURE_TYPE(aiTextureType_LIGHTMAP);
				case TEXTURE_TYPE(aiTextureType_REFLECTION);
				case TEXTURE_TYPE(aiTextureType_BASE_COLOR);
				case TEXTURE_TYPE(aiTextureType_NORMAL_CAMERA);
				case TEXTURE_TYPE(aiTextureType_EMISSION_COLOR);
				case TEXTURE_TYPE(aiTextureType_UNKNOWN);
				case TEXTURE_TYPE(_aiTextureType_Force32Bit);
				default:
					return "Unknown";
					break;
			}
#undef TEXTURE_TYPE
	}

void mesh::loadMaterialTextures(subMesh &subMesh, aiMaterial *material, aiTextureType type)
	{
		if (material->GetTextureCount(type) == 0)
			{
				spdlog::debug("No texture of type '{}' exists in material [{}]", getTextureTypeName(type), material->GetName().C_Str());
				return;
			}

		aiString path;
		material->GetTexture(type, 0, &path);

#define UNSUPPORTED_MATERIAL(materialType) materialType: spdlog::warn("Not supported material type [{}, {}]", #materialType, path.C_Str()); break
		switch (type)
			{
				case aiTextureType_DIFFUSE:
					spdlog::debug("Loading texture aiTextureType_DIFFUSE [{}]", path.C_Str());
					subMesh.materials.albedoMap.loadFromFile(path.C_Str(), true);
					break;
				case aiTextureType_AMBIENT:
					spdlog::debug("Loading texture aiTextureType_AMBIENT [{}]", path.C_Str());
					subMesh.materials.albedoMap.loadFromFile(path.C_Str(), true);
					break;
				case aiTextureType_BASE_COLOR:
					spdlog::debug("Loading texture aiTextureType_DIFFUSE [{}]", path.C_Str());
					subMesh.materials.albedoMap.loadFromFile(path.C_Str(), true);
					break;
				case aiTextureType_NORMALS:
					spdlog::debug("Loading texture aiTextureType_NORMALS [{}]", path.C_Str());
					subMesh.materials.normalMap.loadFromFile(path.C_Str(), false);
					break;
				case aiTextureType_SPECULAR:
					spdlog::debug("Loading texture aiTextureType_SPECULAR [{}]", path.C_Str());
					subMesh.materials.metallicMap.loadFromFile(path.C_Str(), false);
					break;
				case aiTextureType_METALNESS:
					spdlog::debug("Loading texture aiTextureType_METALNESS [{}]", path.C_Str());
					subMesh.materials.metallicMap.loadFromFile(path.C_Str(), false);
					break;
				case aiTextureType_DIFFUSE_ROUGHNESS:
					spdlog::debug("Loading texture aiTextureType_DIFFUSE_ROUGHNESS [{}]", path.C_Str());
					subMesh.materials.roughnessMap.loadFromFile(path.C_Str(), false);
					break;
				case aiTextureType_LIGHTMAP:
					spdlog::debug("Loading texture aiTextureType_LIGHTMAP [{}]", path.C_Str());
					subMesh.materials.ambientOcclusionMap.loadFromFile(path.C_Str(), false);
					break;
				case aiTextureType_AMBIENT_OCCLUSION:
					spdlog::debug("Loading texture aiTextureType_AMBIENT_OCCLUSION [{}]", path.C_Str());
					subMesh.materials.ambientOcclusionMap.loadFromFile(path.C_Str(), false);
					break;
				case UNSUPPORTED_MATERIAL(aiTextureType_NONE);
				case UNSUPPORTED_MATERIAL(aiTextureType_EMISSIVE);
				case UNSUPPORTED_MATERIAL(aiTextureType_HEIGHT);
				case UNSUPPORTED_MATERIAL(aiTextureType_SHININESS);
				case UNSUPPORTED_MATERIAL(aiTextureType_OPACITY);
				case UNSUPPORTED_MATERIAL(aiTextureType_DISPLACEMENT);
				case UNSUPPORTED_MATERIAL(aiTextureType_REFLECTION);
				case UNSUPPORTED_MATERIAL(aiTextureType_NORMAL_CAMERA);
				case UNSUPPORTED_MATERIAL(aiTextureType_EMISSION_COLOR);
				case UNSUPPORTED_MATERIAL(aiTextureType_UNKNOWN);
				case UNSUPPORTED_MATERIAL(_aiTextureType_Force32Bit);
				default:
					spdlog::warn("Unknown Material [{}]", type);
					break;
			}
#undef UNSUPPORTED_MATERIAL
	}

void mesh::processNode(std::vector<vertex> &vertices, std::vector<unsigned int> &indices, const aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
				processMesh(vertices, indices, mesh, scene);
			}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				processNode(vertices, indices, node->mChildren[i], scene);
			}
	}

void mesh::processMesh(std::vector<vertex> &vertices, std::vector<unsigned int> &indices, const aiMesh *mesh, const aiScene *scene)
	{
		subMesh subMesh;
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				vertex v;

				v.position.x = mesh->mVertices[i].x;
				v.position.y = mesh->mVertices[i].y;
				v.position.z = mesh->mVertices[i].z;

				v.normal.x = mesh->mNormals[i].x;
				v.normal.y = mesh->mNormals[i].y;
				v.normal.z = mesh->mNormals[i].z;

				if (mesh->mTextureCoords[0]) 
					{
						v.textureCoordinate.x = mesh->mTextureCoords[0][i].x;
						v.textureCoordinate.y = mesh->mTextureCoords[0][i].y;
					}

				v.tangent.x = mesh->mTangents[i].x;
				v.tangent.y = mesh->mTangents[i].y;
				v.tangent.z = mesh->mTangents[i].z;

				vertices.push_back(v);
			}

		subMesh.indexStartIndex = indices.size();
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					{
						indices.push_back(face.mIndices[j]);
					}
			}
		subMesh.indexCount = indices.size() - subMesh.indexStartIndex;

		if (mesh->mMaterialIndex >= 0)
			{
				aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
				std::vector<aiString> foo;
				for (unsigned int i = 0; i < material->mNumProperties; i++)
				{
					foo.push_back(material->mProperties[i]->mKey);
				}

				spdlog::debug("~~ Processing material [{}] ~~", material->GetName().C_Str());
				for (int i = 1; i < aiTextureType::aiTextureType_UNKNOWN; i++)
					{
						loadMaterialTextures(subMesh, material, static_cast<aiTextureType>(i));
					}
			}

		m_subMeshes.push_back(subMesh);
	}

void mesh::loadFromFile(const char *filepath)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				spdlog::error("Could not load [{}]", filepath);
				return;
			}

		std::vector<vertex> vertices;
		std::vector<unsigned int> indices;
		processNode(vertices, indices, scene->mRootNode, scene);

		m_vertices.bindVertices(vertices);
		m_vertices.bindIndices(indices);
		m_vertices.use(vertex::attributes::POSITION | vertex::attributes::NORMAL | vertex::attributes::TANGENT | vertex::attributes::TEXTURE);
	}
