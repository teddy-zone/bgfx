#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "mesh.h"
#include "material.h"
#include "bgfx_entity.h"

namespace bgfx
{

class RenderableMesh : public BGFXEntity
{

	std::shared_ptr<Material> _material;

public:
	std::shared_ptr<Mesh> _mesh;

	RenderableMesh();

	void bind(const glm::mat4& view_mat, const glm::mat4& proj_mat, std::shared_ptr<bgfx::Material> in_mat = nullptr);

	void draw();

	void set_material(std::shared_ptr<Material> in_mat);

	void set_mesh(std::shared_ptr<Mesh> in_mesh);

	std::shared_ptr<Mesh> get_mesh();
};

}  // namespace bgfx
