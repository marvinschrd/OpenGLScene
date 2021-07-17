#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "texture.h"

namespace gl {

	class Shader
	{
	public:
		unsigned int id;
		// constructor generates the shader on the fly
		Shader(
			const std::string& vertexPath,
			const std::string& fragmentPath,
			const std::string& geometryPath = "");
		// activate the shader
		void Use()
		{
			glUseProgram(id);
			IsError(__FILE__, __LINE__);
		}
		// utility uniform functions
		void SetBool(const std::string& name, bool value) const
		{
			glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
			IsError(__FILE__, __LINE__);
		}
		void SetInt(const std::string& name, int value) const
		{
			glUniform1i(glGetUniformLocation(id, name.c_str()), value);
			IsError(__FILE__, __LINE__);
		}
		void SetFloat(const std::string& name, float value) const
		{
			glUniform1f(glGetUniformLocation(id, name.c_str()), value);
			IsError(__FILE__, __LINE__);
		}
		void SetVec2(const std::string& name, const glm::vec2& value) const
		{
			glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
			IsError(__FILE__, __LINE__);
		}
		void SetVec2(const std::string& name, float x, float y) const
		{
			glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
			IsError(__FILE__, __LINE__);
		}
		void SetVec3(const std::string& name, const glm::vec3& value) const
		{
			glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
			IsError(__FILE__, __LINE__);
		}
		void SetVec3(const std::string& name, float x, float y, float z) const
		{
			glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
			IsError(__FILE__, __LINE__);
		}
		void SetVec4(const std::string& name, const glm::vec4& value) const
		{
			glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
			IsError(__FILE__, __LINE__);
		}
		void SetVec4(
			const std::string& name, 
			float x, float y, float z, float w)
		{
			glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
			IsError(__FILE__, __LINE__);
		}
		void SetMat2(const std::string& name, const glm::mat2& mat) const
		{
			glUniformMatrix2fv(
				glGetUniformLocation(id, name.c_str()), 
				1, 
				GL_FALSE, 
				&mat[0][0]);
			IsError(__FILE__, __LINE__);
		}
		void SetMat3(const std::string& name, const glm::mat3& mat) const
		{
			glUniformMatrix3fv(
				glGetUniformLocation(id, name.c_str()), 
				1, 
				GL_FALSE, 
				&mat[0][0]);
			IsError(__FILE__, __LINE__);
		}
		void SetMat4(const std::string& name, const glm::mat4& mat) const
		{
			glUniformMatrix4fv(
				glGetUniformLocation(id, name.c_str()), 
				1, 
				GL_FALSE, 
				&mat[0][0]);
			//CheckError(__FILE__, __LINE__);
			IsError(__FILE__, __LINE__);
		}

		void SetTexture(const std::string& name, const unsigned int textureID, int value)
		{
			glActiveTexture(GL_TEXTURE0 + value);
			glBindTexture(GL_TEXTURE_2D, textureID);
			//glUniform1i(GetUniformLocation(uniformName), value);
			glUniform1i(glGetUniformLocation(id, name.c_str()), value);
			IsError(__FILE__, __LINE__);
		}

	private:
		// utility function for checking shader compilation/linking errors.
		void CheckCompileErrors(GLuint shader, std::string type);
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

} // End namespace gl.
