#include "mesh.h"

#include <cassert>

namespace bgfx
{

Mesh::Mesh()
{
	_vao.add_buffer(dynamic_cast<BufferBase*>(&_vertices), "vpos", 3, bgfx::AttributeType::FLOAT);
	_vao.add_buffer(dynamic_cast<BufferBase*>(&_normals), "vnorm", 3, bgfx::AttributeType::FLOAT);
	_vao.add_buffer(dynamic_cast<BufferBase*>(&_uv), "vuv", 2, bgfx::AttributeType::FLOAT);
	_vao.add_buffer(dynamic_cast<BufferBase*>(&_vertex_color), "vcolor", 4, bgfx::AttributeType::FLOAT);
}

void Mesh::set_vertices(const std::vector<float>& in_vertices, bool do_calc_normals)
{
	if (do_calc_normals)
	{
		calc_normals(in_vertices);
	}
	_vertices.set_data(in_vertices);
}

void Mesh::set_uv_coords(const std::vector<float>& in_coords)
{
	_uv.set_data(in_coords);
}

void Mesh::set_vertex_colors(const std::vector<float>& in_colors)
{
	_vertex_color.set_data(in_colors);
}

void Mesh::calc_normals(const std::vector<float>& in_vertices)
{
	assert(in_vertices.size() % 3 == 0);
	std::vector<float> normals(in_vertices.size() / 3);
	for (size_t i = 0; i < in_vertices.size(); i += 3)
	{
		const glm::vec3* v1 = reinterpret_cast<const glm::vec3*>(&in_vertices[i]);
		const glm::vec3* v2 = reinterpret_cast<const glm::vec3*>(&in_vertices[i]) + 1;
		const glm::vec3* v3 = reinterpret_cast<const glm::vec3*>(&in_vertices[i]) + 2;
		glm::vec3* n = reinterpret_cast<glm::vec3*>(&normals[i / 3]);
		*n = glm::normalize(glm::cross(*v2 - *v1, *v3 - *v1));
	}
	_normals.set_data(normals);
}

int Mesh::triangle_count()
{
	return _vertices.get_size() / 9;
}

void Mesh::bind()
{
	_vao.bind();
}


}  // namespace bgfx