#include "meshAssimp.h"



namespace gl
{
	
	void Mesh::setUp()
	{
		// VAO binding should be before VAO.
		glGenVertexArrays(1, &VAO_);
		IsError(__FILE__, __LINE__);
		glGenBuffers(1, &VBO_);
		glGenBuffers(1, &EBO_);
		
		glBindVertexArray(VAO_);
		IsError(__FILE__, __LINE__);

		// VBO. 
		IsError(__FILE__, __LINE__);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_);
		IsError(__FILE__, __LINE__);
		glBufferData(
			GL_ARRAY_BUFFER,
			vertices_.size() * sizeof(Vertex),
			vertices_.data(),
			GL_STATIC_DRAW);
		IsError(__FILE__, __LINE__);
		CheckError(__FILE__, __LINE__);
		
		GLintptr vertex_color_offset = 3 * sizeof(float);
		GLintptr vertex_tex_offset = 6 * sizeof(float);
		//positions
		glEnableVertexAttribArray(0);
		IsError(__FILE__, __LINE__);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(GLvoid*)nullptr);
		IsError(__FILE__, __LINE__);

		//normals
		glEnableVertexAttribArray(1);
		IsError(__FILE__, __LINE__);
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(GLvoid*)offsetof(Vertex, normal));
		IsError(__FILE__, __LINE__);

		//texture coordinates
		glEnableVertexAttribArray(2);
		IsError(__FILE__, __LINE__);
		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(GLvoid*)offsetof(Vertex, textureCoord));
		IsError(__FILE__, __LINE__);

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(
			5,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(GLvoid*)offsetof(Vertex, tangent));
		
		
		////tangents
		//glVertexAttribPointer(
		//	5,
		//	3,
		//	GL_FLOAT,
		//	GL_FALSE,
		//	8 * sizeof(float),
		//	(GLvoid*)vertex_tangent_offset);
		//IsError(__FILE__, __LINE__);
		
		/*glEnableVertexAttribArray(5);
		IsError(__FILE__, __LINE__);*/
		IsError(__FILE__, __LINE__);
		CheckError(__FILE__, __LINE__);
		
		// EBO.
		IsError(__FILE__, __LINE__);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
		IsError(__FILE__, __LINE__);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			indices_.size() * sizeof(unsigned int),
			indices_.data(),
			GL_STATIC_DRAW);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(0);

		for (TextureAssimp texture : textures_)
		{
			std::string type = texture.type;
			if(type == "texture_normal")
			{
				hasNormalTexture = true;
				break;
			}
		}
		
	}

	void Mesh::BindTextures(std::unique_ptr<Shader>& shader) const
	{
		unsigned int diffuseNumber = 1;
		unsigned int specularNumber = 1;
		unsigned int normalNumber = 1;
		unsigned int emissionNumber = 1;
		
		for (unsigned int i = 0; i < textures_.size(); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			std::string number;
			std::string name = textures_[i].type;
			if (name == "texture_diffuse")
			{
				//std::cout << "diffuse texture found" << "\n";
				number = std::to_string(diffuseNumber++);
			}
			else if (name == "texture_specular")
			{
				//std::cout << "specular texture found" << "\n";
				number = std::to_string(specularNumber++);
			}
			else if (name == "texture_normal")
			{
				//std::cout << " normal texture found" << "\n";
				number = std::to_string(normalNumber++);
			}
			else if(name == "texture_emission")
			{
				//std::cout << " emission texture found" << "\n";
				number = std::to_string(emissionNumber++);
			}

			//shader->SetFloat(("material." + name + number).c_str(), i);

			//glUniform1i(glGetUniformLocation(shader->id, (name + number).c_str()), i);
			shader->SetInt((name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures_[i].ID);
			CheckError(__FILE__, __LINE__);
		}
		glActiveTexture(GL_TEXTURE0);
	}

	void Mesh::draw(std::unique_ptr<Shader>& shader)
	{
		
		BindTextures(shader);
		
		//draw
		glBindVertexArray(VAO_);
		glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
		CheckError(__FILE__, __LINE__);
		glBindVertexArray(0);
		
	}
	
}