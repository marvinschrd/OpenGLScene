#include <SDL_main.h>
#include <glad/glad.h>
#include <array>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine.h"
#include "camera.h"
#include "modelAssimp.h"
#include "cubeMap.h"
#include "sceneObject.h"

namespace gl {

	class HelloSkybox : public Program
	{
	public:
		void Init() override;
		void Update(seconds dt, SDL_Window* window) override;
		void Destroy() override;
		void OnEvent(SDL_Event& event) override;
		void DrawImGui() override;

	protected:
		void SetModelMatrix(seconds dt);
		void SetViewMatrix(seconds dt);
		void SetProjectionMatrix();
		void IsError(const std::string& file, int line) const;
		void SetUniformMatrix() const;
		void RenderScene(std::unique_ptr<Shader>& shaders);
		void RenderQuad();
		void Culling(unsigned long begin, unsigned long end);

		unsigned int VAO_;
		unsigned int VBO_;
		unsigned int EBO_;
		unsigned int vertex_shader_;
		unsigned int fragment_shader_;

		unsigned int skyboxVAO, skyboxVBO;
		unsigned int skyboxTexture;

		//unsigned int program_;

		float time_ = 0.0f;
		float delta_time_ = 0.0f;

		//camera use variables
		glm::vec2 windowSize_;
		glm::vec2 windowCenter_;
		bool firstMouse = true;
		float lastX;
		float lastY;
		float fov_ = 45.0f;

		std::unique_ptr<Model> modele_;
		std::unique_ptr<Model> dragonEgg_;
		std::unique_ptr<Model> ground_;
		std::unique_ptr<Model> bigTree_;
		std::unique_ptr<Model> rock_;
		std::unique_ptr<Model> portal_;
		std::unique_ptr<Model> handStatue_;
		std::unique_ptr<Model> fireSphere_;
		std::unique_ptr<CubeMap> skybox_;

		std::unique_ptr<Camera> camera_ = nullptr;
		std::unique_ptr<Camera> depthCamera_ = nullptr;
		std::unique_ptr<Texture> texture_diffuse_2_ = nullptr;
		std::unique_ptr<Shader> generalShaders_ = nullptr;
		std::unique_ptr<Shader> skyboxShaders_ = nullptr;
		std::unique_ptr<Shader> simpleDepthShaders_ = nullptr;
		std::unique_ptr<Shader> hdrShaders_ = nullptr;

		glm::mat4 modelMatrix_ = glm::mat4(1.0f);
		glm::mat4 inv_model_ = glm::mat4(1.0f);
		glm::mat4 view_ = glm::mat4(1.0f);
		glm::mat4 projection_ = glm::mat4(1.0f);

		//glm::vec3 light_position = glm::vec3(0.0, 0.0, 30.0);
		glm::vec3 light_position = glm::vec3(0.0, 20.0, 30.0);
		//glm::vec3 light_Dir = glm::normalize(glm::vec3(0, 25, 50) - glm::vec3(0.0,0.0,0.0));
		glm::vec3 light_Dir = glm::normalize(light_position - glm::vec3(0.0, 0.0, 0.0));
		glm::vec3 pointLight_position = glm::vec3(0.0, 20.0, 37.0);
		
		unsigned int depthMapFBO;
		unsigned int depthMap;

		const int shadowSizeHeight_ = 1024;
		const int shadowSizeWidth_ = 1024;

		//HDR framebuffer
		unsigned int hdrFBO = 0;
		unsigned int colorBuffer = 0;
		unsigned int rboDepth = 0;


		std::vector<glm::vec3> modelsPositions
		{
			glm::vec3(0.0,0.0,0.0),
			glm::vec3(0.0,0.0,-10),
			glm::vec3(10.0,0.0,-7.0),
			glm::vec3(-4.0,0.0,-20)
		};

		
	};

	void HelloSkybox::IsError(const std::string& file, int line) const
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

	void HelloSkybox::Init()
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		
		std::string path = "../";

		simpleDepthShaders_ = std::make_unique<Shader>(
			path + "data/shaders/hello_shadows/simpleShadow.vert",
			path + "data/shaders/hello_shadows/simpleShadow.frag"
			);

		hdrShaders_ = std::make_unique<Shader>(
			path + "data/shaders/hello_HDR/HDRShader.vert",
			path + "data/shaders/hello_HDR/HDRShader.frag"
			);

		camera_ = std::make_unique<Camera>(glm::vec3(0.0f, 10.0f, 25.0f));
		/*depthCamera_ = std::make_unique<Camera>(glm::vec3(light_position.x, light_position.y, light_position.z));
		depthCamera_->LookAt(light_position + light_Dir);*/


		std::vector<std::string>textures_faces{
			path + "data/textures_test/skybox/right.png",
			path + "data/textures_test/skybox/left.png",
			path + "data/textures_test/skybox/top.png",
			path + "data/textures_test/skybox/bottom.png",
			path + "data/textures_test/skybox/front.png",
			path + "data/textures_test/skybox/back.png"
		};

		skybox_ = std::make_unique<CubeMap>(textures_faces);
		modele_ = std::make_unique<Model>(path + "data/mesh_test/Winter_Girl.obj");
		dragonEgg_ = std::make_unique<Model>(path + "data/mesh_test/egg.obj");
		bigTree_ = std::make_unique<Model>(path + "data/mesh_test/Tree/bigTree.obj");
		ground_ = std::make_unique<Model>(path + "data/mesh_test/Ground.obj");
		rock_ = std::make_unique<Model>(path + "data/mesh_test/Rocks/rock.obj");
		portal_ = std::make_unique<Model>(path + "data/mesh_test/portal.obj");
		handStatue_ = std::make_unique<Model>(path + "data/mesh_test/handStatue.obj");
		fireSphere_ = std::make_unique<Model>(path + "data/mesh_test/fireSphere.obj");
		
		
		//house_ = std::make_unique<Model>(path + "data/mesh_test/House/house.obj");


		//HDR FrameBuffer
		glGenFramebuffers(1, &hdrFBO);
		glGenTextures(1, &colorBuffer);
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1024, 701, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// create depth buffer (renderbuffer)
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 701);
		// attach buffers
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		hdrShaders_->Use();
		hdrShaders_->SetInt("hdrBuffer", 0);

		

		//Shadow map FrameBuffer
		glGenFramebuffers(1, &depthMapFBO);
		// create depth texture
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		CheckError(__FILE__, __LINE__);
		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		CheckError(__FILE__, __LINE__);
		GLenum drawbuffer = GL_NONE;
		glDrawBuffers(0,&drawbuffer);
		glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		CheckError(__FILE__, __LINE__);

		generalShaders_ = std::make_unique<Shader>(
			path + "data/shaders/hello_shadows/shadow.vert",
			path + "data/shaders/hello_shadows/shadow.frag");
		generalShaders_->Use();
		generalShaders_->SetInt("shadowMap", 4);
		//shaders_->SetInt("skybox", 0);

		skyboxShaders_ = std::make_unique<Shader>(
			path + "data/shaders/skyBoxShader.vert",
			path + "data/shaders/skyBoxShader.frag");
		skyboxShaders_->Use();
		skyboxShaders_->SetInt("skybox", 0);

		glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
		IsError(__FILE__, __LINE__);
	}

	void HelloSkybox::SetModelMatrix(seconds dt)
	{
		modelMatrix_ = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.f, 1.f, 0.f));
		inv_model_ = glm::transpose(glm::inverse(modelMatrix_));
	}

	void HelloSkybox::SetViewMatrix(seconds dt)
	{
		view_ = camera_->GetViewMatrix();
	}

	void HelloSkybox::SetProjectionMatrix()
	{
		projection_ = glm::perspective(glm::radians(fov_), 4.0f / 3.0f, 0.1f, 200.f);
	}

	void HelloSkybox::SetUniformMatrix() const
	{
		generalShaders_->Use();
		generalShaders_->SetMat4("model", modelMatrix_);
		generalShaders_->SetMat4("view", view_);
		generalShaders_->SetMat4("projection", projection_);
		generalShaders_->SetMat4("inv_model", inv_model_);
		generalShaders_->SetVec3("cameraPosition", camera_->position);
	}

	void HelloSkybox::Update(seconds dt, SDL_Window* window)
	{
		int x;
		int y;
		SDL_GetWindowSize(window, &x, &y);
		windowSize_ = glm::vec2(x, y);
		delta_time_ = dt.count();
		time_ += delta_time_;
		light_Dir = glm::normalize(light_position - glm::vec3(0.0, 0.0, 0.0));

		

		/*glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		lightProjection = depthCamera_->GetProjection();
		lightView = depthCamera_->GetViewMatrix();
		lightSpaceMatrix = lightProjection * lightView;*/

		//camera_->ProcessKeyboard(CameraMovementEnum::RIGHT, delta_time_*0.25f);
		

		
		glm::mat4 light_Projection, light_View;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 100.0f;
		light_Projection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
		light_View = glm::lookAt(light_position, light_position - light_Dir, glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = light_Projection * light_View;

		//Shadow Pass
		simpleDepthShaders_->Use();
		simpleDepthShaders_->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
		
		glViewport(0, 0, shadowSizeHeight_, shadowSizeWidth_);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glActiveTexture(GL_TEXTURE0);
		glCullFace(GL_FRONT);
		RenderScene(simpleDepthShaders_);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, windowSize_.x, windowSize_.y);
		glClear( GL_DEPTH_BUFFER_BIT);
		// End of shadow pass

		
		//normal pass with HDR framebuffer
		glViewport(0, 0, windowSize_.x, windowSize_.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		generalShaders_->Use();
		SetViewMatrix(dt);
		SetProjectionMatrix();
		generalShaders_->SetMat4("view", view_);
		generalShaders_->SetMat4("projection", projection_);
		generalShaders_->SetVec3("cameraPosition", camera_->position);
		generalShaders_->SetVec3("lightPosition", light_position);
		generalShaders_->SetVec3("lightDirection", light_Dir);
		generalShaders_->SetVec3("pointLightPosition", pointLight_position);
		
		generalShaders_->SetFloat("constant", 1.0f);
		generalShaders_->SetFloat("linear", 0.14f);
		generalShaders_->SetFloat("quadratic", 0.07f);
		
		generalShaders_->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE4);
		generalShaders_->SetInt("shadowMap", 4);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glActiveTexture(GL_TEXTURE0);
		
		RenderScene(generalShaders_);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		hdrShaders_->Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		RenderQuad();
		
		IsError(__FILE__, __LINE__);
		CheckError(__FILE__, __LINE__);
	}

	void HelloSkybox::RenderScene(std::unique_ptr<Shader>& Shaders)
	{
		/*glActiveTexture(GL_TEXTURE6);
		Shaders->SetInt("shadowMap", 6);
		glBindTexture(GL_TEXTURE_2D, depthMap);*/

		/*modelMatrix_ = glm::mat4(1.0f);
		modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(0.0f, 0.0f, -5.0f));
		modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(3.0f, 3.0f, 3.0f));
		modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(90.0f), glm::vec3(0.f, 1.f, 0.f));
		inv_model_= glm::transpose(glm::inverse(modelMatrix_));
		Shaders->SetMat4("model", modelMatrix_);
		Shaders->SetMat4("inv_model", inv_model_);
		modele_->Draw(Shaders);*/


		modelMatrix_ = glm::mat4(1.0f);
		modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(-1.0f, 0.0f, -30.0f));
		modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(90.0f), glm::vec3(0.f, 1.f, 0.f));
		modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(1.5f, 1.5f, 1.5f));
		inv_model_ = glm::transpose(glm::inverse(modelMatrix_));
		Shaders->SetMat4("model", modelMatrix_);
		Shaders->SetMat4("inv_model", inv_model_);
		bigTree_->Draw(Shaders);

		//modelMatrix_ = glm::mat4(1.0f);
		//modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(0.0f, 2.0f, -15.0f));
		////modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(90.0f), glm::vec3(0.f, 1.f, 0.f));
		//modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(4.0f, 4.0f, 4.0f));
		//inv_model_ = glm::transpose(glm::inverse(modelMatrix_));
		//Shaders->SetMat4("model", modelMatrix_);
		//Shaders->SetMat4("inv_model", inv_model_);
		//rock_->Draw(Shaders);

		modelMatrix_ = glm::mat4(1.0f);
		modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(0.0f, 0.0f, 18.0f));
		//modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(90.0f), glm::vec3(0.f, 1.f, 0.f));
		modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(1.5, 1.5f, 1.5f));
		inv_model_ = glm::transpose(glm::inverse(modelMatrix_));
		Shaders->SetMat4("model", modelMatrix_);
		Shaders->SetMat4("inv_model", inv_model_);
		portal_->Draw(Shaders);

		modelMatrix_ = glm::mat4(1.0f);
		modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(0.0f, 2.5f, 45.0f));
		//modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(90.0f), glm::vec3(0.f, 1.f, 0.f));
		modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(3, 3, 3));
		inv_model_ = glm::transpose(glm::inverse(modelMatrix_));
		Shaders->SetMat4("model", modelMatrix_);
		Shaders->SetMat4("inv_model", inv_model_);
		handStatue_->Draw(Shaders);

		glm::vec3 spherePos(0.0f, 20.0f, 37.0f);
		
		
		modelMatrix_ = glm::mat4(1.0f);
		modelMatrix_ = glm::translate(modelMatrix_, spherePos);
		modelMatrix_ = glm::rotate(modelMatrix_, time_, glm::vec3(0.f, 1.f, 0.f));
		modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(8, 8, 8));
		inv_model_ = glm::transpose(glm::inverse(modelMatrix_));
		Shaders->SetMat4("model", modelMatrix_);
		Shaders->SetMat4("inv_model", inv_model_);
		fireSphere_->Draw(Shaders);

		modelMatrix_ = glm::mat4(1.0f);
		modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(0.0f, 1.75f, 23.0f));
		//modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(90.0f), glm::vec3(0.f, 1.f, 0.f));
		modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(1.5, 1.5f, 1.5f));
		inv_model_ = glm::transpose(glm::inverse(modelMatrix_));
		Shaders->SetMat4("model", modelMatrix_);
		Shaders->SetMat4("inv_model", inv_model_);
		dragonEgg_->Draw(Shaders);

		modelMatrix_ = glm::mat4(1.0f);
		modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(0.0f, -0.1f, 0.0f));
		//modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(90.0f), glm::vec3(0.f, 1.f, 0.f));
		modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(1, 1.5f, 1.5f));
		inv_model_ = glm::transpose(glm::inverse(modelMatrix_));
		Shaders->SetMat4("model", modelMatrix_);
		Shaders->SetMat4("inv_model", inv_model_);
		ground_->Draw(Shaders);

		skybox_->Draw(skyboxShaders_, view_, projection_, camera_);
	}

	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;
	void HelloSkybox::RenderQuad()
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

	}


	void HelloSkybox::Destroy()
	{
	}

	void HelloSkybox::OnEvent(SDL_Event& event)
	{
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
				exit(0);
			if (event.key.keysym.sym == SDLK_w)
			{
				camera_->ProcessKeyboard(CameraMovementEnum::FORWARD, delta_time_);
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				camera_->ProcessKeyboard(CameraMovementEnum::BACKWARD, delta_time_);
			}
			if (event.key.keysym.sym == SDLK_a)
			{
				camera_->ProcessKeyboard(CameraMovementEnum::LEFT, delta_time_);
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				camera_->ProcessKeyboard(CameraMovementEnum::RIGHT, delta_time_);
			}

			/*if(event.key.keysym.sym == SDLK_m)
			{
				camera_->MoveCamera(glm::vec3(0.0,40.0,0.0), -90,-89.0f);
			}*/
		}

		if (event.type == SDL_MOUSEMOTION)
		{
			int x;
			int y;
			SDL_GetMouseState(&x, &y);

			if (firstMouse)
			{
				lastX = x;
				lastY = y;
				firstMouse = false;
			}

			//std::cout << "x mouse offset = " << x << "\n";

			float currX = event.motion.x;
			float currY = event.motion.y;

			/*float xOffset = x - lastX;
			float yOffset = lastY - y;*/

			float xOffset = currX - lastX;
			float yOffset = lastY - currY;

			lastX = currX;
			lastY = currY;

			camera_->ProcessMouseMovement(xOffset, yOffset, time_);
		}

		if (event.type == SDL_MOUSEWHEEL)
		{
			fov_ -= event.wheel.y;
			if (fov_ < 1.0f)
				fov_ = 1.0f;
			if (fov_ > 45.0f)
				fov_ = 45.0f;
		}
	}

	void HelloSkybox::DrawImGui()
	{

	}




} // End namespace gl.

int main(int argc, char** argv)
{
	gl::HelloSkybox program;
	gl::Engine engine(program);
	engine.Run();
	return EXIT_SUCCESS;
}