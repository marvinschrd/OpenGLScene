#pragma once

#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "quad.h"
#include "shader.h"
#include "error.h"

namespace gl
{

	enum Type : unsigned int
	{
		COLOR_ATTACHMENT_0 = 1u,
		DEPTH_RBO = 1u << 1u,
		NO_DRAW = 1u << 2u,
		HDR = 1u << 3u,
		DEPTH_ATTACHMENT = 1u << 4u,
		DEFAULT = COLOR_ATTACHMENT_0
	};


	
	class FrameBuffer
	{
	public:
		
		void generate();

		void destroy();

		void Reload();

		void Draw(std::unique_ptr<Shader>& screenShader);

		void Bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
			CheckError(__FILE__, __LINE__);
		}
		 void Unbind()
		{
			 glBindFramebuffer(GL_FRAMEBUFFER, 0);
			 CheckError(__FILE__, __LINE__);
		}

		void resize(glm::vec2 newSize)
		{
			size_ = newSize;
		}

		unsigned int GetDepthTextureID() const
		{
			return frameBufferTexture ;
		}

		void SetType(unsigned int frameBufferType)
		{
			type_ = frameBufferType;
		}

	private:
		unsigned int fbo_ = 0;
		unsigned int rbo_ = 0;
		unsigned int frameBufferTexture = 0;
		unsigned int type_ = DEFAULT;
		glm::vec2 size_ = glm::vec2(1024, 701);
	};


	
}
