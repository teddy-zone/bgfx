#include "renderable_mesh.h"


#include "glad/gl.h"
#include <GLFW/glfw3.h>

namespace bgfx
{

RenderableMesh::RenderableMesh():
	_position(0,0,0),
	_rotation(glm::mat4(1)),
	_scale(1,1,1)
{
	update_transform();
}

void RenderableMesh::update_transform()
{
	_transform = glm::mat4(1);
	auto trans_mat = glm::translate(glm::mat4(1), _position);
	_transform = _transform * trans_mat;
	_transform = glm::scale(_transform, _scale);
	_transform = _transform * _rotation;
}

glm::mat4& RenderableMesh::get_transform()
{
	return _transform;
}

void RenderableMesh::translate(const glm::vec3& translation)
{
	_position += translation;
	_transform = glm::translate(_transform, translation);
}

void RenderableMesh::rotate(const glm::mat4& rotation)
{
	_rotation = _rotation * rotation;
	_transform = _transform * rotation;
}

void RenderableMesh::scale(const glm::vec3& scale)
{
	_scale *= scale;
	_transform = glm::scale(_transform, scale);
}

void RenderableMesh::set_position(const glm::vec3& new_pos)
{
	_position = new_pos;
	update_transform();
}

void RenderableMesh::set_scale(const glm::vec3& new_scale)
{
	_scale = new_scale;
	update_transform();
}

void RenderableMesh::set_rotation(const glm::mat4& new_rotation)
{
	_rotation = new_rotation;
	update_transform();
}

void RenderableMesh::bind(const glm::mat4& view_mat, const glm::mat4& proj_mat, bool use_mat)
{
#ifdef ENABLE_GRAPHICS
	if (use_mat)
	{
		_material->use();
	}
	
	_material->set_transform(_transform, "model_matrix");

	_material->set_transform(proj_mat, "projection_matrix");

	_material->set_transform(view_mat, "view_matrix");

	_mesh->bind();
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}
#endif 
}

void RenderableMesh::draw()
{
#ifdef ENABLE_GRAPHICS
	glDrawArrays(GL_TRIANGLES, 0, _mesh->triangle_count()*3);
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}
#endif
}

void RenderableMesh::set_material(std::shared_ptr<Material> in_mat)
{
	_material = in_mat;
}

void RenderableMesh::set_mesh(std::shared_ptr<Mesh> in_mesh)
{
	_mesh = in_mesh;
}

std::shared_ptr<Mesh> RenderableMesh::get_mesh()
{
	return _mesh;
}

}  // namespace bgfx
