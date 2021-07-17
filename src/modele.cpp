#include "modele.h"



namespace gl
{
	Modele::Modele(const std::string& filename)
	{
		std::string directory;
		unsigned int nb_vertices = 0;

		tinyobj::ObjReader reader;
		if (!reader.ParseFromFile(filename))
		{
			throw std::runtime_error("cannot load file: " + filename);
		};
		auto& attrib = reader.GetAttrib();
		auto& shapes = reader.GetShapes();
		auto& materials = reader.GetMaterials();
		for (const auto& material : materials)
		{
			ParseMaterial(material);
		}

		for (const auto& shape : shapes)
		{

			ParseMesh(shape, attrib);

		}

		//assert(shapes.size() == 1);


	}

	void Modele::ParseMaterial(const tinyobj::material_t& material)
	{
		Material mat{};
		std::string path = "../data/textures_test/";
		mat.color = Texture(path + material.diffuse_texname);
		mat.specular = Texture(path + material.roughness_texname);
		if(material.bump_texname != "")
		{
			mat.normalMap = Texture(path + material.bump_texname);
		}
		mat.specular_pow = material.shininess;
		mat.specular_vec = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
		materials.push_back(mat);
	}

	void Modele::ParseMesh(const tinyobj::shape_t& shape, const tinyobj::attrib_t& attrib)
	{
		//assert(shapes.size() == 1);
		unsigned int material_id = 0;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		unsigned int nb_vertices = 0;

		int index_offset = 0;
		for (std::size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f)
		{
			int fv = shape.mesh.num_face_vertices[f];
			if (fv != 3)
			{
				throw std::runtime_error("should be triangles");
			}

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				Vertex vertex{};
				// access to vertex
				tinyobj::index_t idx =
					shape.mesh.indices[index_offset + v];
				vertex.position.x = attrib.vertices[3 * idx.vertex_index + 0];
				vertex.position.y = attrib.vertices[3 * idx.vertex_index + 1];
				vertex.position.z = attrib.vertices[3 * idx.vertex_index + 2];
				vertex.normal.x = attrib.normals[3 * idx.normal_index + 0];
				vertex.normal.y = attrib.normals[3 * idx.normal_index + 1];
				vertex.normal.z = attrib.normals[3 * idx.normal_index + 2];
				vertex.textureCoord.x =
					attrib.texcoords[2 * idx.texcoord_index + 0];
				vertex.textureCoord.y =
					attrib.texcoords[2 * idx.texcoord_index + 1];

				
				vertices.push_back(vertex);
				indices.push_back(static_cast<uint32_t>(indices.size()));
			}
			index_offset += fv;

		}

		//calculate tangents
		for(int i = 0; i< vertices.size(); i+=3)
		{
			//positions
			glm::vec3& v0 = vertices[i + 0].position;
			glm::vec3& v1 = vertices[i + 1].position;
			glm::vec3& v2 = vertices[i + 2].position;

			//texcoords
			glm::vec2& uv0 = vertices[i + 0].textureCoord;
			glm::vec2& uv1 = vertices[i + 1].textureCoord;
			glm::vec2& uv2 = vertices[i + 2].textureCoord;

			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;

			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

			vertices[i].tangent = tangent;
		}


		
		material_id = shape.mesh.material_ids[0];
		assert(material_id < 100);
		nb_vertices = index_offset;
		//Meshe mesh(vertices, indices, nb_vertices);
		meshes.emplace_back(vertices, indices, nb_vertices, material_id);
	}

}