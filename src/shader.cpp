#include "shader.h"

namespace gl
{
	Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
	{
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(
			std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(
			std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(
			std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			// if geometry shader path is present, also load a geometry
			// shader
			if (!geometryPath.empty())
			{
				gShaderFile.open(geometryPath);
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure& e)
		{
			throw std::runtime_error(e.what());
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		// 2. compile shaders
		unsigned int vertex, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		IsError(__FILE__, __LINE__);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		IsError(__FILE__, __LINE__);
		glCompileShader(vertex);
		IsError(__FILE__, __LINE__);
		CheckCompileErrors(vertex, "VERTEX");
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		IsError(__FILE__, __LINE__);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		IsError(__FILE__, __LINE__);
		glCompileShader(fragment);
		IsError(__FILE__, __LINE__);
		CheckCompileErrors(fragment, "FRAGMENT");
		// if geometry shader is given, compile geometry shader
		unsigned int geometry;
		if (!geometryPath.empty())
		{
			const char* gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			IsError(__FILE__, __LINE__);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			IsError(__FILE__, __LINE__);
			glCompileShader(geometry);
			IsError(__FILE__, __LINE__);
			CheckCompileErrors(geometry, "GEOMETRY");
		}
		// shader Program
		id = glCreateProgram();
		IsError(__FILE__, __LINE__);
		glAttachShader(id, vertex);
		IsError(__FILE__, __LINE__);
		glAttachShader(id, fragment);
		IsError(__FILE__, __LINE__);
		if (!geometryPath.empty())
		{
			glAttachShader(id, geometry);
			IsError(__FILE__, __LINE__);
		}
		glLinkProgram(id);
		IsError(__FILE__, __LINE__);
		CheckCompileErrors(id, "PROGRAM");
		IsError(__FILE__, __LINE__);
		//// delete the shaders as they're linked into our program now and no
		//// longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		IsError(__FILE__, __LINE__);
		if (!geometryPath.empty())
		{
			glDeleteShader(geometry);
			IsError(__FILE__, __LINE__);
		}
	}

	void Shader::CheckCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				throw std::runtime_error(
					"Shader compilation error: " + type + " : " + infoLog);
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				/*throw std::runtime_error(
					"Shader linking error: " + type + " : " + infoLog);*/
			}
		}
	}

}
	

