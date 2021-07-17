#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include "material.h"
#include "meshe.h"
#include <tiny_obj_loader.h>

namespace gl
{
	
	class Modele
	{
	public:

		std::vector<Meshe> meshes;
		std::vector<Material> materials;
		
		Modele(const std::string& filename);
		void Draw()
		{
			for (Meshe mesh : meshes)
			{
				
				//mesh.draw();
			}
		}
	private:

		void ParseMaterial(const tinyobj::material_t& material);
		void ParseMesh(const tinyobj::shape_t& shape, const tinyobj::attrib_t& attrib);
	};
}






