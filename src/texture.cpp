#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace gl
{
	unsigned int LoadCubeMap(std::vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			CheckError(__FILE__, __LINE__);
			if (data)
			{
				/*std::cout << nrChannels;*/
				switch (nrChannels)
				{
				case 1:
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
					CheckError(__FILE__, __LINE__);
					break;
				case 3:
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					CheckError(__FILE__, __LINE__);
					break;
				case 4:
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					CheckError(__FILE__, __LINE__);
					break;


				}
				/*glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);*/
				stbi_image_free(data);
				CheckError(__FILE__, __LINE__);
			}
			else
			{
				std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		CheckError(__FILE__, __LINE__);

		return textureID;
	}

	unsigned int LoadTextureFromFile(const char* path, const std::string& directory, aiTextureType textureType)
	{
		
		std::string filename = std::string(path);
		std::cout << "filename = " << filename << "\n";
		filename = directory + "/" + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			GLenum format2;
			if (nrComponents == 1)
			{
				format = GL_RED;
				format2 = GL_RED;
			}
			else if (nrComponents == 2)
			{
				format = GL_RG;
				format2 = GL_RG;
			}	
			else if (nrComponents == 3)
			{
				if (textureType == aiTextureType_DIFFUSE)
				{
					format = GL_SRGB;
				}
				else
				{
					format = GL_RGB;
				}
				format2 = GL_RGB;
			}
			else if (nrComponents == 4)
			{
				if (textureType == aiTextureType_DIFFUSE)
				{
					format = GL_SRGB8_ALPHA8;
				}
				else
				{
					format = GL_RGBA;
				}
				format2 = GL_RGBA;
			}

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format2, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}

	/*Texture::Texture(const std::string& file_name)
	{
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* dataDiffuse = stbi_load(
			file_name.c_str(),
			&width,
			&height,
			&nrChannels,
			0);
		assert(dataDiffuse);

		glGenTextures(1, &id);
		IsError(__FILE__, __LINE__);
		glBindTexture(GL_TEXTURE_2D, id);
		IsError(__FILE__, __LINE__);
		if (nrChannels == 1)
		{
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				width,
				height,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				dataDiffuse);
			IsError(__FILE__, __LINE__);
		}
		if (nrChannels == 3)
		{
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				dataDiffuse);
			IsError(__FILE__, __LINE__);
		}
		if (nrChannels == 4)
		{
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				width,
				height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				dataDiffuse);
			IsError(__FILE__, __LINE__);
		}
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_S,
			GL_CLAMP_TO_EDGE);
		IsError(__FILE__, __LINE__);
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_T,
			GL_CLAMP_TO_EDGE);
		IsError(__FILE__, __LINE__);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		IsError(__FILE__, __LINE__);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		IsError(__FILE__, __LINE__);
		glGenerateMipmap(GL_TEXTURE_2D);
		IsError(__FILE__, __LINE__);
		glBindTexture(GL_TEXTURE_2D, 0);
		IsError(__FILE__, __LINE__);
		STBI_FREE(dataDiffuse);
	}*/

}