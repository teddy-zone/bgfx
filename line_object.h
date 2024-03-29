#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "mesh.h"
#include "material.h"

namespace bgfx
{

class LineObject 
{

	glm::vec3 _position;
	glm::mat4 _rotation;
	glm::vec3 _scale;
	glm::mat4 _transform;
	std::shared_ptr<Material> _material;

	void update_transform();

public:

	bool strip = true;

	std::shared_ptr<Mesh> _mesh;

	LineObject();

	glm::mat4& get_transform();

	void translate(const glm::vec3& translation);

	void rotate(const glm::mat4& rotation);

	void scale(const glm::vec3& scale);

	void set_position(const glm::vec3& new_pos);

	void set_scale(const glm::vec3& new_scale);

	void bind(const glm::mat4& view_mat, const glm::mat4& proj_mat, std::shared_ptr<bgfx::Material> in_mat=nullptr);

	void draw();

	void set_material(std::shared_ptr<Material> in_mat);

	void set_mesh(std::shared_ptr<Mesh> in_mesh);

	std::shared_ptr<Mesh> get_mesh();
};

}  // namespace bgfx
