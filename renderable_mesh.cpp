#include "renderable_mesh.h"


#include "glad/gl.h"
#include <GLFW/glfw3.h>

namespace bgfx
{

RenderableMesh::RenderableMesh()
{
}

void RenderableMesh::bind(const glm::mat4& view_mat, const glm::mat4& proj_mat, std::shared_ptr<bgfx::Material> in_mat)
{

	if (!in_mat)
	{
		in_mat = _material;
	}

#ifdef ENABLE_GRAPHICS
	in_mat->use();
	
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}
	auto translation = glm::vec3(get_transform()[3]);
	translation.z = 0;
	in_mat->set_transform(get_transform(), "model_matrix");

	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}

	in_mat->set_transform(proj_mat, "projection_matrix");

	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}

	in_mat->set_transform(view_mat, "view_matrix");

	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}

	_mesh->bind();
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
