#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include "material.h"


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
	
	class   Meshe
	{
	public:
		unsigned int material_index_;
		unsigned int nb_vertices_;
		Meshe(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, unsigned int nb_vertices, const unsigned int material_id)
			: vertices_(vertices), indices_(indices), nb_vertices_(nb_vertices), material_index_(material_id)
		{
			setUp();
		};

		void Bind() const
		{
			glBindVertexArray(VAO_);
			IsError(__FILE__, __LINE__);
		}

		void draw();

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
		std::vector<uint32_t> indices_;

	};
}