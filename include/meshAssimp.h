#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include "material.h"
#include "shader.h"
#include "texture.h"
#include "error.h"




namespace gl
{


	class Vertex
	{
	public:
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textureCoord;
		glm::vec3 tangent;
	};

	/*struct Material
	{
		
	};*/

	/*struct TextureAssimp
	{
		unsigned int ID = 0;
		std::string type;
		std::string path;
	};*/

	class   Mesh
	{
	public:
		unsigned int material_index_;
		unsigned int nb_vertices_;
		bool hasNormalTexture = false;
		Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<TextureAssimp>& textures) : vertices_(vertices), indices_(indices), textures_(textures)
		{
			setUp();
		};

		void Bind() const
		{
			glBindVertexArray(VAO_);
			IsError(__FILE__, __LINE__);
		}
		unsigned int GetVAO() const
		{
			return VAO_;
		}
		TextureAssimp GetTexture(unsigned int textureIndex) const
		{
			return textures_[textureIndex];
		}

		void BindTextures(std::unique_ptr<Shader>& shader) const;
		
		void draw(std::unique_ptr<Shader>& shader);

		std::vector<TextureAssimp> textures_;
		std::vector<unsigned int> indices_;
	private:
		void setUp();

		void IsError(const std::string& file, int line) const
		{
			auto error_code = glGetError();
			if (error_code != GL_NO_ERROR)
			{
				std::cerr
					<< error_code
					<< " in file: " << file
					<< " at line: " << line
					<< "\n";
			}
		}

		unsigned int VAO_ = 0;
		unsigned int VBO_ = 0;
		unsigned int EBO_ = 0;
		//unsigned int material_index_;
		std::vector<Vertex> vertices_;

	};
}