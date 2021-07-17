#include "frameBuffer.h"

namespace gl
{	
	void FrameBuffer::generate()
	{
		if (type_ == DEFAULT || COLOR_ATTACHMENT_0)
		{
			//Fbo
			glGenFramebuffers(1, &fbo_);
			Bind();
			CheckError(__FILE__, __LINE__);

			//colorTexture
			glGenTextures(1, &frameBufferTexture);
			glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size_.x, size_.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);
			CheckError(__FILE__, __LINE__);

			//rbo
			glGenRenderbuffers(1, &rbo_);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size_.x, size_.y); // use a single renderbuffer object for both a depth AND stencil buffer.
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_);

			CheckError(__FILE__, __LINE__);

			
			
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << "\n";
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			CheckError(__FILE__, __LINE__);
		}
		else if (type_ == DEPTH_ATTACHMENT)
		{
			//Fbo
			glGenFramebuffers(1, &fbo_);
			Bind();
			CheckError(__FILE__, __LINE__);
			
			glGenTextures(1, &frameBufferTexture);
			glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size_.x, size_.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			CheckError(__FILE__, __LINE__);
			// attach depth texture as FBO's depth buffer
			glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frameBufferTexture, 0);
			glDrawBuffers(1,GL_NONE);
			glReadBuffer(GL_NONE);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << "\n";
			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}

	void FrameBuffer::destroy()
	{
		if(fbo_)
		{
			glDeleteFramebuffers(1, &fbo_);
		}
		if(rbo_)
		{
			glDeleteRenderbuffers(1, &rbo_);
		}
		if(frameBufferTexture)
		{
			glDeleteTextures(1, &frameBufferTexture);
		}
	}

	void FrameBuffer::Reload()
	{
		Unbind();
		destroy();
		generate();
	}

	void FrameBuffer::Draw(std::unique_ptr<Shader>& screenShader)
	{
		Unbind();
		glClearColor(0.1f, 0.1f, 0.1f, 0.1f); 
		glClear(GL_COLOR_BUFFER_BIT);
		CheckError(__FILE__, __LINE__);
		//glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		screenShader->Use();
		screenShader->SetInt("screenTexture", 0);
		CheckError(__FILE__, __LINE__);
		glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
		CheckError(__FILE__, __LINE__);
		
	}

}
