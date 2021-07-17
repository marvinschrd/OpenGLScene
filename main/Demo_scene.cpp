#include "programs/Demo_scene.h"

namespace gl
{
	

	void DemoScene::Init()
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		camera_ = std::make_unique<Camera>(glm::vec3(0.0f, 10.0f, 25.0f));

		//Shaders initialization
		normalMapShaders_ = std::make_unique<Shader>(
			path + "data/shaders/hello_shadows/shadow.vert",
			path + "data/shaders/hello_shadows/shadow.frag");
		
		simpleDepthShaders_ = std::make_unique<Shader>(
			path + "data/shaders/hello_shadows/simpleShadow.vert",
			path + "data/shaders/hello_shadows/simpleShadow.frag"
			);

		hdrShaders_ = std::make_unique<Shader>(
			path + "data/shaders/hello_HDR/HDRShader.vert",
			path + "data/shaders/hello_HDR/HDRShader.frag"
			);

		//Models initialization
		skybox_ = std::make_unique<CubeMap>(texturesFaces_);
		modele_ = std::make_unique<Model>(path + "data/mesh_test/Dragon.obj");

		//HDR FrameBuffer
		glGenFramebuffers(1, &hdrFBO_);
		glGenTextures(1, &colorBuffer_);
		glBindTexture(GL_TEXTURE_2D, colorBuffer_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH_, WINDOW_HEIGHT_, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// create depth buffer (renderbuffer)
		glGenRenderbuffers(1, &rboDepth_);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth_);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH_, WINDOW_HEIGHT_);
		// attach buffers
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO_);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer_, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth_);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		hdrShaders_->Use();
		hdrShaders_->SetInt("hdrBuffer", 0);

		//Shadow map FrameBuffer
		glGenFramebuffers(1, &depthMapFBO_);
		// create depth texture
		glGenTextures(1, &depthMap_);
		glBindTexture(GL_TEXTURE_2D, depthMap_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_SIZE_WIDTH_, SHADOW_SIZE_HEIGHT_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		CheckError(__FILE__, __LINE__);
		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap_, 0);
		CheckError(__FILE__, __LINE__);
		GLenum drawbuffer = GL_NONE;
		glDrawBuffers(0, &drawbuffer);
		glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		CheckError(__FILE__, __LINE__);
		glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
	}

	void DemoScene::Update(seconds dt, SDL_Window* window)
	{
		//window size
		int x;
		int y;
		SDL_GetWindowSize(window, &x, &y);
		WINDOW_WIDTH_ = x;
		WINDOW_HEIGHT_ = y;

		glm::mat4 light_Projection, light_View;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 100.0f;
		light_Projection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
		light_View = glm::lookAt(directionalLightPosition_, directionalLightPosition_ - light_Dir, glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = light_Projection * light_View;

		//Shadow Pass
		simpleDepthShaders_->Use();
		simpleDepthShaders_->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_SIZE_WIDTH_, SHADOW_SIZE_HEIGHT_);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glActiveTexture(GL_TEXTURE0);
		glCullFace(GL_FRONT);
		RenderScene(simpleDepthShaders_);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, windowSize_.x, windowSize_.y);
		glClear(GL_DEPTH_BUFFER_BIT);
		// End of shadow pass


		//normal pass with HDR framebuffer
		glViewport(0, 0, windowSize_.x, windowSize_.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO_);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		normalMapShaders_->Use();
		SetViewMatrix();
		SetProjectionMatrix();
		normalMapShaders_->SetMat4("view", view_);
		normalMapShaders_->SetMat4("projection", projection_);
		normalMapShaders_->SetVec3("cameraPosition", camera_->position);
		normalMapShaders_->SetVec3("lightPosition", directionalLightPosition_);
		normalMapShaders_->SetVec3("lightDirection", light_Dir);
		normalMapShaders_->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE4);
		normalMapShaders_->SetInt("shadowMap", 4);
		glBindTexture(GL_TEXTURE_2D, depthMap_);
		glActiveTexture(GL_TEXTURE0);

		RenderScene(normalMapShaders_);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		hdrShaders_->Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffer_);
		RenderQuad();
		//CheckError(__FILE__, __LINE__);
	}

	void DemoScene::RenderScene(std::unique_ptr<Shader>& normalMapShader)
	{
		//Set model matrices and draw
		model_ = glm::mat4(1.0);
		invModel_ = glm::transpose(glm::inverse(model_));
		normalMapShader->SetMat4("model", model_);
		normalMapShader->SetMat4("inv_model", invModel_);
		modele_->Draw(normalMapShader);

		//Draw skybox
		skybox_->Draw(skyboxShaders_, view_, projection_, camera_);
	}

	void DemoScene::RenderQuad()
	{
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO_);
		glGenBuffers(1, &quadVBO_);
		glBindVertexArray(quadVAO_);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		glBindVertexArray(quadVAO_);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	void DemoScene::Destroy()
	{
	}

	void DemoScene::SetViewMatrix()
	{
		view_ = camera_->GetViewMatrix();
		normalMapShaders_->SetMat4("view", view_);
	}

	void DemoScene::SetProjectionMatrix()
	{
		projection_ = glm::perspective(glm::radians(fov_), 4.0f / 3.0f, 0.1f, 200.f);
		normalMapShaders_->SetMat4("projection", projection_);
	}



	void DemoScene::OnEvent(SDL_Event& event)
	{
	}

	void DemoScene::DrawImGui()
	{
	}

} // namespace gl

int main(int argc, char** argv)
{
	gl::DemoScene program;
	gl::Engine engine(program);
	engine.Run();
	return EXIT_SUCCESS;
}