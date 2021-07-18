#include "modelAssimp.h"


namespace gl
{
	Model::Model(const std::string& filename)
	{
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(
			filename, 
			aiProcess_Triangulate |
			aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR ASSIMP" << importer.GetErrorString() << "\n";
			return;
		}
		directory = filename.substr(0, filename.find_last_of('/'));
		//std::cout << "directory = " << directory << "\n";
		ProcessNode(scene->mRootNode, scene);
		CheckError(__FILE__, __LINE__);

		/*diffuseShader_ = std::make_unique<Shader>(
			std::string("../data/shaders/meshShader.vert"),
			std::string("../data/shaders/meshShader.frag")
			);*/
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh, scene));
			//std::cout << "mesh number = " << i << "\n";
		}
		for(unsigned int i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene);
			//std::cout << "children number = " << i << "\n";
		}
	}

	Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<TextureAssimp> textures;

		//vertex
		for(unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
		//	std::cout << "vertice number : " << i << "\n";
			Vertex vertex;
			glm::vec3 positions;
			glm::vec3 normals;
			glm::vec2 textureCoord;
			glm::vec3 tangents;
			
			positions.x = mesh->mVertices[i].x;
			positions.y = mesh->mVertices[i].y;
			positions.z = mesh->mVertices[i].z;
			vertex.position = positions;

			normals.x = mesh->mNormals[i].x;
			normals.y = mesh->mNormals[i].y;
			normals.z = mesh->mNormals[i].z;
			vertex.normal = normals;

			textureCoord.x = mesh->mTextureCoords[0][i].x;
			textureCoord.y = mesh->mTextureCoords[0][i].y;
			vertex.textureCoord = textureCoord;

			tangents.x = mesh->mTangents[i].x;
			tangents.y = mesh->mTangents[i].y;
			tangents.z = mesh->mTangents[i].z;
			vertex.tangent = tangents;
			
			vertices.push_back(vertex);
		}

		//indices
		for(unsigned int i = 0; i<mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j )
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		//materials/textures
		if(mesh->mMaterialIndex >=0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			
			std::vector<TextureAssimp> diffuseTextures = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
			std::vector<TextureAssimp> specularTextures = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());
			std::vector<TextureAssimp> normalMap = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMap.begin(), normalMap.end());
			
			//every material load an emissive texture even when they don't have one so there is a problem
			/*std::vector<TextureAssimp> emissionMap = LoadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emission");
			textures.insert(textures.end(), emissionMap.begin(), emissionMap.end());*/

		}

		
		return Mesh(vertices, indices, textures);
	}

	std::vector<TextureAssimp> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType textureType, std::string typeName)
	{
		std::vector<TextureAssimp> textures;
		const size_t textureCount = material->GetTextureCount(textureType);
		for(unsigned int i = 0; i < textureCount; ++i)
		{
			aiString string;
			material->GetTexture(textureType, i, &string);
			bool skip = false;
			const size_t sizeOfTextureLoaded = texturesLoaded_.size();
			for(unsigned int j = 0; j < texturesLoaded_.size(); ++j)
			{
				if (std::strcmp(texturesLoaded_[j].path.data(), string.C_Str()) == 0)
				{
					textures.push_back(texturesLoaded_[j]);
					skip = true;
					break;
				}
				assert(j < sizeOfTextureLoaded);
			}
			if(!skip)
			{
				TextureAssimp texture;
				
				texture.ID = LoadTextureFromFile(string.C_Str(), textureDirectory, textureType);
				texture.type = typeName;
				texture.path = string.C_Str();
				textures.push_back(texture);
				texturesLoaded_.push_back(texture);
			}
		}
		return textures;
	}

}