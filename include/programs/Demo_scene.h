#pragma once

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



namespace gl {

	class DemoScene : public Program
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
		void SetUniformMatrix(std::unique_ptr<Shader>& shader) const;
		void RenderScene(std::unique_ptr<Shader>& normalMapShader);
		void RenderSceneDepth(std::unique_ptr<Shader>& shader);
		void RenderQuad();

		unsigned int VAO_;
		unsigned int VBO_;
		unsigned int EBO_;
		unsigned int vertex_shader_;
		unsigned int fragment_shader_;

		int WINDOW_HEIGHT_ = 701;
		int WINDOW_WIDTH_ = 1024;

		std::string path = "../";

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
		std::unique_ptr<Model> ground_;
		std::unique_ptr<CubeMap> skybox_;

		std::unique_ptr<Camera> camera_ = nullptr;
		std::unique_ptr<Shader> normalMapShaders_ = nullptr;
		std::unique_ptr<Shader> diffuseShaders_ = nullptr;
		std::unique_ptr<Shader> skyboxShaders_ = nullptr;
		std::unique_ptr<Shader> simpleDepthShaders_ = nullptr;
		std::unique_ptr<Shader> hdrShaders_ = nullptr;

		glm::mat4 model_ = glm::mat4(1.0f);
		glm::mat4 invModel_ = glm::mat4(1.0f);
		glm::mat4 view_ = glm::mat4(1.0f);
		glm::mat4 projection_ = glm::mat4(1.0f);

		//Lights
		glm::vec3 directionalLightPosition_{ 0.0, 0.0, 30.0 };
		glm::vec3 light_Dir{ 0, 0, 1 };

		//Skybox
		unsigned int skyboxVAO_, skyboxVBO_;
		unsigned int skyboxTexture_;
		std::vector<std::string>texturesFaces_{
			path + "data/textures_test/skybox/right.png",
			path + "data/textures_test/skybox/left.png",
			path + "data/textures_test/skybox/top.png",
			path + "data/textures_test/skybox/bottom.png",
			path + "data/textures_test/skybox/front.png",
			path + "data/textures_test/skybox/back.png"
		};

		//Shadows
		unsigned int depthMapFBO_;
		unsigned int depthMap_;
		const int SHADOW_SIZE_HEIGHT_ = 1024;
		const int SHADOW_SIZE_WIDTH_ = 1024;

		//HDR framebuffer
		unsigned int hdrFBO_ = 0;
		unsigned int colorBuffer_ = 0;
		unsigned int rboDepth_ = 0;

		//Quad
		unsigned int quadVAO_ = 0;
		unsigned int quadVBO_ = 0;
		float quadVertices[20] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
	};
}