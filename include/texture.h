#pragma once

#include <string>
#include <fstream>
#include <glad/glad.h>
#include "error.h"
#include <vector>
#include <cassert>
#include <assimp/material.h>

namespace gl {

	unsigned int LoadCubeMap(std::vector<std::string> faces);
	unsigned int LoadTextureFromFile(const char* path, const std::string& directory, aiTextureType textureType);
	
	class Texture {
	public:
		unsigned int id;
		Texture() {}
		Texture(const std::string& file_name);
		void Bind(unsigned int i = 0) const
		{
			glActiveTexture(GL_TEXTURE0 + i);
			IsError(__FILE__, __LINE__);
			glBindTexture(GL_TEXTURE_2D, id);
			IsError(__FILE__, __LINE__);
		}
		void UnBind() const
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		
	protected:
		void IsError(const char* file, int line) const {
			auto error_code = glGetError();
			if (error_code != GL_NO_ERROR)
			{
				throw std::runtime_error(
					std::to_string(error_code) + 
					" in file: " + file + 
					" at line: " + std::to_string(line));
			}
		}
	};

	struct  TextureAssimp
	{
		unsigned int ID = 0;
		std::string type;
		std::string path;
	};

	//ajouter textureAssimp class avec fonction texture from file;

} // End namespace gl.
