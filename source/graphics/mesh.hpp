// mesh.hpp
// contains data about a given mesh
#pragma once
#include "vertexArray.hpp"
#include "vertex.hpp"
#include "texture.hpp"
#include "material.hpp"
#include <vector>

struct aiMaterial;
enum aiTextureType;
struct aiNode;
struct aiMesh;
struct aiScene;
class mesh
	{
		private:
			// meta-data for each submesh.
			struct subMesh
				{
					material materials;
					unsigned int indexStartIndex = 0;
					unsigned int indexCount = 0;
				};

			std::vector<subMesh> m_subMeshes;
			vertexArray m_vertices;

			friend class graphicsEngine;

			void loadMaterialTextures(subMesh &subMesh, aiMaterial *material, aiTextureType type);

			void processNode(std::vector<vertex> &vertices, std::vector<unsigned int> &indices, const aiNode *node, const aiScene *scene);
			void processMesh(std::vector<vertex>& vertices, std::vector<unsigned int> &indices, const aiMesh *mesh, const aiScene *scene);

		public:
			void loadFromFile(const char *filepath);
	};
