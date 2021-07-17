#include "error.h"

namespace gl
{
	void CheckError(const std::string& file, int line)
	{
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            // Process/log the error.
            switch (err)
            {
            case GL_INVALID_ENUM:
                std::cerr
                    << err
                    << " in file: " << file
                    << " at line: " << line
                    << " OpenGL: GL_INVALID_ENUM"
                    << "\n";
                break;
            case GL_INVALID_VALUE:
                std::cerr
                    << err
                    << " in file: " << file
                    << " at line: " << line
                    << " OpenGL: GL_INVALID_VALUE"
                    << "\n";
                break;
            case GL_INVALID_OPERATION:
                std::cerr
                    << err
                    << " in file: " << file
                    << " at line: " << line
                    << " OpenGL: GL_INVALID_OPERATION"
                    << "\n";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                std::cerr
                    << err
                    << " in file: " << file
                    << " at line: " << line
                    << " OpenGL: GL_INVALID_FRAMBUFFER_OPERATION"
                    << "\n";
                break;
            }
        }
	}
}