#pragma once

#include "modelAssimp.h"

namespace gl
{
	struct SceneObject
	{
		SceneObject();
		
		std::unique_ptr<Model> model_ = NULL;
		glm::vec3 position_{ 0.0,0.0,0.0 };
	};
}