#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include "material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "meshAssimp.h"
#include "shader.h"
#include "error.h"

namespace gl
{
	class Model
	{
	public:
		std::string directory;
		std::string textureDirectory = "../data/textures_test";
		std::vector<Mesh> meshes;
		std::vector<Material> materials;

		Model(const std::string& filename);
		void Draw(std::unique_ptr<Shader>& normalShader)
		{
			for (unsigned int i = 0; i < meshes.size(); ++i)
			{
				//check if mesh has a normal texture or not
				//to use the correct shader program
					meshes[i].draw(normalShader);
				/*if(meshes[i].hasNormalTexture)
				{
				}
				else
				{
					meshes[i].draw(diffuseShader_);
				}*/
				//std::cout << "mesh number : " << i << " drawn" << "\n";
			}
		}

		const Mesh& getMesh(unsigned int meshIndex)
		{
			return meshes[meshIndex];
		}

	private:
		std::vector<TextureAssimp> texturesLoaded_;
		//std::unique_ptr<Shader> diffuseShader_;
		
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<TextureAssimp> LoadMaterialTextures(aiMaterial* material, aiTextureType textureType, std::string typeName);
	};
}
