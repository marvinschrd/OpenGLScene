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
#include "random"

namespace gl {

	class Demo : public Program
	{
	public:
		void Init() override;
		void Update(seconds dt, SDL_Window* window) override;
		void Destroy() override;
		void OnEvent(SDL_Event& event) override;
		void DrawImGui() override;

	protected:
		void SetModelMatrix(seconds dt);
		void SetViewMatrix();
		void SetProjectionMatrix();
		void SetUniformMatrix() const;
		void RenderScene(std::unique_ptr<Shader>& shaders);
		void RenderQuad();
		void InitModels();
		void InitFramebuffers();
		void InitShaders();
		void CameraMovements();
		void Rendering();
		
		unsigned int skyboxVAO, skyboxVBO;
		unsigned int skyboxTexture;

		float time_ = 0.0f;
		float delta_time_ = 0.0f;

		//camera use variables
		glm::vec2 windowSize_;
		glm::vec2 windowCenter_;
		bool firstMouse = true;
		float lastX;
		float lastY;
		float fov_ = 45.0f;

		//models
		std::unique_ptr<Model> modele_;
		std::unique_ptr<Model> dragonEgg_;
		std::unique_ptr<Model> ground_;
		std::unique_ptr<Model> bigTree_;
		std::unique_ptr<Model> rock_;
		std::unique_ptr<Model> portal_;
		std::unique_ptr<Model> handStatue_;
		std::unique_ptr<Model> fireSphere_;
		std::unique_ptr<Model> grass_;
		std::unique_ptr<Model> window_;
		std::unique_ptr<CubeMap> skybox_;
		std::vector<std::string>textures_faces_;

		std::unique_ptr<Camera> camera_ = nullptr;
		
		//shaders
		std::unique_ptr<Shader> generalShaders_ = nullptr;
		std::unique_ptr<Shader> skyboxShaders_ = nullptr;
		std::unique_ptr<Shader> simpleDepthShaders_ = nullptr;
		std::unique_ptr<Shader> hdrShaders_ = nullptr;

		glm::mat4 modelMatrix_ = glm::mat4(1.0f);
		glm::mat4 inv_model_ = glm::mat4(1.0f);
		glm::mat4 view_ = glm::mat4(1.0f);
		glm::mat4 projection_ = glm::mat4(1.0f);

		//light
		glm::vec3 light_position = glm::vec3(0.0, 20.0, 30.0);
		glm::vec3 light_Dir = glm::normalize(light_position - glm::vec3(0.0, 0.0, 0.0));
		glm::vec3 pointLight_position = glm::vec3(0.0, 20.0, 37.0);

		//shadow
		unsigned int depthMapFBO;
		unsigned int depthMap;
		const int shadowSizeHeight_ = 1024;
		const int shadowSizeWidth_ = 1024;

		//HDR framebuffer
		unsigned int hdrFBO = 0;
		unsigned int colorBuffer = 0;
		unsigned int rboDepth = 0;

		//instanced grass
		const unsigned long maxGrassNmb_ = 100'000;
		const unsigned long minGrassNmb_ = 1'000;
		unsigned long instanceChunkSize_ = 1'000;
		unsigned long grassNmb_ = 250;
		std::vector<glm::vec3> grassPositions_;
		unsigned int instanceVBO_ = 0;

		//Camera position movement
		float cameraTimer_ = 0.f;
		bool hasMoved = false;
		enum POSITION
		{
			FIRST_POSITION,
			SECOND_POSITION,
			THIRD_POSITION,
			FOURTH_POSITION,
			FIFTH_POSITION,
			SIXTH_POSITION
		};
		POSITION CameraPosition_ = FIRST_POSITION;
		std::array<glm::vec3, 6> cameraPositions_{
			glm::vec3(45.0f,40.0f,35.0f),
			glm::vec3(20,25,50),
			glm::vec3(3,3.5f,40),
			glm::vec3(-8.0f,20.0f,40.0f),
			glm::vec3(26.0f,20.0f,8.0f),
			glm::vec3(3.0f,4.0f,-35.0f)
		};
		std::array<float, 6> cameraYaws_{
			-90.0f,
			-35.0f,
			-15.0f,
			35.0f,
			-120.0f,
			-180.0f
		};
		std::array<float, 6> cameraPitchs_{
			-35.f,
			-30.0f,
			-5.0f,
			-30.0f,
			-20.0f,
			5.f
		};

		std::string path_ = "../";
	};


	void Demo::Init()
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		camera_ = std::make_unique<Camera>(glm::vec3(0.0f, 10.0f, 25.0f));

		InitShaders();
		InitModels();
		InitFramebuffers();

		glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
		CheckError(__FILE__, __LINE__);
	}

	void Demo::SetModelMatrix(seconds dt)
	{
		modelMatrix_ = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.f, 1.f, 0.f));
		inv_model_ = glm::transpose(glm::inverse(modelMatrix_));
	}

	void Demo::SetViewMatrix()
	{
		view_ = camera_->GetViewMatrix();
	}

	void Demo::SetProjectionMatrix()
	{
		projection_ = glm::perspective(glm::radians(fov_), 4.0f / 3.0f, 0.1f, 200.f);
	}

	void Demo::SetUniformMatrix() const
	{
		generalShaders_->Use();
		generalShaders_->SetMat4("model", modelMatrix_);
		generalShaders_->SetMat4("view", view_);
		generalShaders_->SetMat4("projection", projection_);
		generalShaders_->SetMat4("inv_model", inv_model_);
		generalShaders_->SetVec3("cameraPosition", camera_->position);
	}

	void Demo::Update(seconds dt, SDL_Window* window)
	{
		int x;
		int y;
		SDL_GetWindowSize(window, &x, &y);
		windowSize_ = glm::vec2(x, y);
		delta_time_ = dt.count();
		time_ += delta_time_;

		CameraMovements();
		Rendering();

	}

	void Demo::RenderScene(std::unique_ptr<Shader>& Shaders)
	{
		
		modelMatrix_ = glm::mat4(1.0f);
		modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(-1.0f, 0.0f, -30.0f));
		modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(90.0f), glm::vec3(0.f, 1.f, 0.f));
		modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(1.5f, 1.5f, 1.5f));
		inv_model_ = glm::transpose(glm::inverse(modelMatrix_));
		Shaders->SetMat4("model", modelMatrix_);
		Shaders->SetMat4("inv_model", inv_model_);
		bigTree_->Draw(Shaders);

		modelMatrix_ = glm::mat4(1.0f);
		modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(0.0f, 2.0f, -15.0f));
		modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(4.0f, 4.0f, 4.0f));
		inv_model_ = glm::transpose(glm::inverse(modelMatrix_));
		Shaders->SetMat4("model", modelMatrix_);
		Shaders->SetMat4("inv_model", inv_model_);
		rock_->Draw(Shaders);

		modelMatrix_ = glm::mat4(1.0f);
		modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(0.0f, 0.0f, 18.0f));
		modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(1.5, 1.5f, 1.5f));
		inv_model_ = glm::transpose(glm::inverse(modelMatrix_));
		Shaders->SetMat4("model", modelMatrix_);
		Shaders->SetMat4("inv_model", inv_model_);
		portal_->Draw(Shaders);

		modelMatrix_ = glm::mat4(1.0f);
		modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(0.0f, 2.5f, 45.0f));
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
		modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(1.5, 1.5f, 1.5f));
		inv_model_ = glm::transpose(glm::inverse(modelMatrix_));
		Shaders->SetMat4("model", modelMatrix_);
		Shaders->SetMat4("inv_model", inv_model_);
		dragonEgg_->Draw(Shaders);

		modelMatrix_ = glm::mat4(1.0f);
		modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(0.0f, -0.1f, 0.0f));
		modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(1.25, 1.75f, 1.75f));
		inv_model_ = glm::transpose(glm::inverse(modelMatrix_));
		Shaders->SetMat4("model", modelMatrix_);
		Shaders->SetMat4("inv_model", inv_model_);
		ground_->Draw(Shaders);

		//instanced grass
		for (unsigned int i = 0; i < grass_->meshes.size(); i++)
		{
			const auto& wallMesh = grass_->getMesh(i);
			wallMesh.BindTextures(Shaders);
			modelMatrix_ = glm::mat4(1.0f);
			modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(3.5, 3.5f, 3.5f));
			Shaders->SetMat4("view", view_);
			Shaders->SetMat4("projection", projection_);
			Shaders->SetMat4("model", modelMatrix_);
			Shaders->SetMat4("inv_model", inv_model_);
			for (std::size_t chunk = 0; chunk < grassNmb_ / instanceChunkSize_ + 1; chunk++)
			{
				const std::size_t chunkBeginIndex = chunk * instanceChunkSize_;
				const std::size_t chunkEndIndex = std::min(grassNmb_,
					static_cast<unsigned long>(chunk + 1) *
					instanceChunkSize_);

				if (chunkEndIndex > chunkBeginIndex)
				{
					const size_t chunkSize = chunkEndIndex - chunkBeginIndex;
					{
						glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_);
						glBufferData(GL_ARRAY_BUFFER,
							sizeof(glm::vec3) * chunkSize,
							&grassPositions_[chunkBeginIndex],
							GL_DYNAMIC_DRAW);
						glBindBuffer(GL_ARRAY_BUFFER, 0);
					}
					glBindVertexArray(wallMesh.GetVAO());
					glDrawElementsInstanced(GL_TRIANGLES,
						wallMesh.indices_.size(),
						GL_UNSIGNED_INT, 0,
						chunkSize);
					glBindVertexArray(0);
				}
			}
		}

		modelMatrix_ = glm::mat4(1.0f);
		modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(-7.0f, 8.f, 23.0f));
		modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(45.0f), glm::vec3(-1.f, 0.f, 0.f));
		modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(7.f, 7.f, 7.f));
		inv_model_ = glm::transpose(glm::inverse(modelMatrix_));
		Shaders->SetMat4("model", modelMatrix_);
		Shaders->SetMat4("inv_model", inv_model_);
		modele_->Draw(Shaders);
		
		skybox_->Draw(skyboxShaders_, view_, projection_, camera_);
	}

	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;
	void Demo::RenderQuad()
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

	void Demo::InitModels()
	{
		textures_faces_ = {
			path_ + "data/textures_test/skybox/right.png",
			path_ + "data/textures_test/skybox/left.png",
			path_ + "data/textures_test/skybox/top.png",
			path_ + "data/textures_test/skybox/bottom.png",
			path_ + "data/textures_test/skybox/front.png",
			path_ + "data/textures_test/skybox/back.png"
		};
		
		skybox_ = std::make_unique<CubeMap>(textures_faces_);
		modele_ = std::make_unique<Model>(path_ + "data/mesh_test/flyingCreature.obj");
		dragonEgg_ = std::make_unique<Model>(path_ + "data/mesh_test/egg.obj");
		bigTree_ = std::make_unique<Model>(path_ + "data/mesh_test/Tree/bigTree.obj");
		ground_ = std::make_unique<Model>(path_ + "data/mesh_test/Ground.obj");
		rock_ = std::make_unique<Model>(path_ + "data/mesh_test/Rocks/rock.obj");
		portal_ = std::make_unique<Model>(path_ + "data/mesh_test/portal.obj");
		handStatue_ = std::make_unique<Model>(path_ + "data/mesh_test/handStatue.obj");
		fireSphere_ = std::make_unique<Model>(path_ + "data/mesh_test/fireSphere.obj");
		grass_ = std::make_unique<Model>(path_ + "data/mesh_test/grass.obj");

		//caclulate random grass positions
		grassPositions_.resize(grassNmb_);
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> disX(8.0, -8.0);
		std::uniform_real_distribution<> disZ(16.0, -16.0);
		for (unsigned int i = 0; i < grassNmb_; ++i)
		{
			const float x = disX(gen);
			const float z = disZ(gen);

			glm::vec3 position = glm::vec3(x, 0.0f, z);
			grassPositions_[i] = position;
		}
		const auto& grassMesh = grass_->getMesh(0);
		glBindVertexArray(grassMesh.GetVAO());
		glGenBuffers(1, &instanceVBO_);

		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_);
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
			(void*)0);
		glVertexAttribDivisor(6, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Demo::InitFramebuffers()
	{
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
		glDrawBuffers(0, &drawbuffer);
		glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		CheckError(__FILE__, __LINE__);
	}

	void Demo::InitShaders()
	{
		generalShaders_ = std::make_unique<Shader>(
			path_ + "data/shaders/hello_shadows/shadow.vert",
			path_ + "data/shaders/hello_shadows/shadow.frag");
		generalShaders_->Use();
		generalShaders_->SetInt("shadowMap", 4);

		skyboxShaders_ = std::make_unique<Shader>(
			path_ + "data/shaders/skyBoxShader.vert",
			path_ + "data/shaders/skyBoxShader.frag");
		skyboxShaders_->Use();
		skyboxShaders_->SetInt("skybox", 0);
		
		simpleDepthShaders_ = std::make_unique<Shader>(
			path_ + "data/shaders/hello_shadows/simpleShadow.vert",
			path_ + "data/shaders/hello_shadows/simpleShadow.frag"
			);

		hdrShaders_ = std::make_unique<Shader>(
			path_ + "data/shaders/hello_HDR/HDRShader.vert",
			path_ + "data/shaders/hello_HDR/HDRShader.frag"
			);
	}

	void Demo::CameraMovements()
	{
		cameraTimer_ += delta_time_;
		switch (CameraPosition_)
		{
		case FIRST_POSITION:
			if (!hasMoved)
			{
				camera_->MoveCamera(cameraPositions_[0], cameraYaws_[0], cameraPitchs_[0]);
				hasMoved = true;
			}
			camera_->ProcessKeyboard(CameraMovementEnum::RIGHT, delta_time_ * 0.15f);
			if (cameraTimer_ > 8.0f)
			{
				cameraTimer_ = 0.0f;
				hasMoved = false;
				CameraPosition_ = SECOND_POSITION;
			}
			break;
		case SECOND_POSITION:
			if (!hasMoved)
			{
				camera_->MoveCamera(cameraPositions_[1], cameraYaws_[1], cameraPitchs_[1]);
				hasMoved = true;
			}
			camera_->ProcessKeyboard(CameraMovementEnum::RIGHT, delta_time_ * 0.05f);
			if (cameraTimer_ > 6.0f)
			{
				cameraTimer_ = 0.0f;
				hasMoved = false;
				CameraPosition_ = THIRD_POSITION;
			}
			break;
		case THIRD_POSITION:
			if (!hasMoved)
			{
				camera_->MoveCamera(cameraPositions_[2], cameraYaws_[2], cameraPitchs_[2]);
				hasMoved = true;
			}
			camera_->ProcessKeyboard(CameraMovementEnum::RIGHT, delta_time_ * 0.05f);
			if (cameraTimer_ > 6.0f)
			{
				cameraTimer_ = 0.0f;
				hasMoved = false;
				CameraPosition_ = FOURTH_POSITION;
			}
			break;
		case FOURTH_POSITION:
			if (!hasMoved)
			{
				camera_->MoveCamera(cameraPositions_[3], cameraYaws_[3], cameraPitchs_[3]);
				hasMoved = true;
			}
			camera_->ProcessKeyboard(CameraMovementEnum::LEFT, delta_time_ * 0.05f);
			if (cameraTimer_ > 6.0f)
			{
				cameraTimer_ = 0.0f;
				hasMoved = false;
				CameraPosition_ = FIFTH_POSITION;
			}
			break;
		case FIFTH_POSITION:
			if (!hasMoved)
			{
				camera_->MoveCamera(cameraPositions_[4], cameraYaws_[4], cameraPitchs_[4]);
				hasMoved = true;
			}
			camera_->ProcessKeyboard(CameraMovementEnum::LEFT, delta_time_ * 0.05f);
			if (cameraTimer_ > 6.0f)
			{
				cameraTimer_ = 0.0f;
				hasMoved = false;
				CameraPosition_ = SIXTH_POSITION;
			}
			break;
		case SIXTH_POSITION:
			if (!hasMoved)
			{
				camera_->MoveCamera(cameraPositions_[5], cameraYaws_[5], cameraPitchs_[5]);
				hasMoved = true;
			}
			camera_->ProcessKeyboard(CameraMovementEnum::LEFT, delta_time_ * 0.05f);
			if (cameraTimer_ > 6.0f)
			{
				cameraTimer_ = 0.0f;
				hasMoved = false;
				CameraPosition_ = FIRST_POSITION;
			}
			break;
		}
	}

	void gl::Demo::Rendering()
	{
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

		//normal pass with HDR framebuffer
		glViewport(0, 0, windowSize_.x, windowSize_.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		generalShaders_->Use();
		SetViewMatrix();
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

		CheckError(__FILE__, __LINE__);
	}



	void Demo::Destroy()
	{
	}

	void Demo::OnEvent(SDL_Event& event)
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

	void Demo::DrawImGui()
	{

	}




} // End namespace gl.

int main(int argc, char** argv)
{
	gl::Demo program;
	gl::Engine engine(program);
	engine.Run();
	return EXIT_SUCCESS;
}