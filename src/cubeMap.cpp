#include "cubeMap.h"


namespace gl
{


	
	CubeMap::CubeMap(std::vector<std::string>& faces)
	{
		skyboxTexture = LoadCubeMap(faces);

		glGenVertexArrays(1, &skyboxVAO_);
		glGenBuffers(1, &skyboxVBO_);
		glBindVertexArray(skyboxVAO_);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}

	void CubeMap::Draw(std::unique_ptr<Shader>& cubeMapShaders, glm::mat4& view, glm::mat4& projection, std::unique_ptr<Camera>& camera)
	{
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		cubeMapShaders->Use();
		view = glm::mat4(glm::mat3(camera->GetViewMatrix())); // remove translation from the view matrix
		cubeMapShaders->SetMat4("view", view);
		cubeMapShaders->SetMat4("projection", projection);

		glBindVertexArray(skyboxVAO_);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
	}

}