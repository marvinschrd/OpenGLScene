#include "meshe.h"

namespace gl
{
	void Meshe::setUp()
	{
		// VAO binding should be before VAO.
		glGenVertexArrays(1, &VAO_);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(VAO_);
		IsError(__FILE__, __LINE__);

		// EBO.
		glGenBuffers(1, &EBO_);
		IsError(__FILE__, __LINE__);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
		IsError(__FILE__, __LINE__);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			indices_.size() * sizeof(float),
			indices_.data(),
			GL_STATIC_DRAW);
		IsError(__FILE__, __LINE__);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		IsError(__FILE__, __LINE__);

		// VBO. 
		glGenBuffers(1, &VBO_);
		IsError(__FILE__, __LINE__);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_);
		IsError(__FILE__, __LINE__);
		glBufferData(
			GL_ARRAY_BUFFER,
			vertices_.size() * sizeof(Vertex),
			vertices_.data(),
			GL_STATIC_DRAW);
		IsError(__FILE__, __LINE__);

		GLintptr vertex_color_offset = 3 * sizeof(float);
		GLintptr vertex_tex_offset = 6 * sizeof(float);
		GLintptr vertex_tangent_offset = 8 * sizeof(float);
		//positions
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(float),
			0);
		IsError(__FILE__, __LINE__);
		
		//normals
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(float),
			(GLvoid*)vertex_color_offset);
		IsError(__FILE__, __LINE__);

		//texture coordinates
		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(float),
			(GLvoid*)vertex_tex_offset);
		IsError(__FILE__, __LINE__);

		//tangents
		glVertexAttribPointer(
			5,
			3,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(float),
			(GLvoid*)vertex_tangent_offset);
		IsError(__FILE__, __LINE__);
		glEnableVertexAttribArray(0);
		IsError(__FILE__, __LINE__);
		glEnableVertexAttribArray(1);
		IsError(__FILE__, __LINE__);
		glEnableVertexAttribArray(2);
		IsError(__FILE__, __LINE__);
		glEnableVertexAttribArray(5);
		IsError(__FILE__, __LINE__);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		IsError(__FILE__, __LINE__);

		//glBindVertexArray(0);
	}

	void Meshe::draw()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
		IsError(__FILE__, __LINE__);
		Bind();
		//materials + shaders from model materials index
		glDrawElements(GL_TRIANGLES, nb_vertices_, GL_UNSIGNED_INT, 0);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(0);
	}




	
}