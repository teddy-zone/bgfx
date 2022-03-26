#pragma once

#include <array>

#include "glm/glm.hpp"

#include "vertex_array.h"
#include "buffer.h"

namespace bgfx
{

struct Triangle
{
	std::array<glm::vec3, 3> vertices;

	glm::vec3 normal()
	{
		return glm::normalize(glm::cross(vertices[1] - vertices[0], vertices[2] - vertices[0]));
	}
};

class Mesh
{
protected:
    friend class RenderableMesh;
	Buffer<float> _vertices;
	Buffer<float> _normals;
	Buffer<float> _uv;
	Buffer<float> _vertex_color;
    Buffer<unsigned int> _vertex_indices;
    Buffer<unsigned int> _normal_indices;
	VertexArray _vao;
	bool _indexed;

public:
	std::vector<float> _saved_vertices;
	glm::vec3 _bmin;
	glm::vec3 _bmax;

	Mesh();
	void set_vertices(const std::vector<float>& in_vertices, bool do_calc_normals = false);
	void set_vertex_indices(const std::vector<unsigned int>& in_data);
	void set_normals(const std::vector<float>& in_normals);
	void set_normal_indices(const std::vector<unsigned int>& in_data);
	void set_uv_coords(const std::vector<float>& in_coords);
	void set_vertex_colors(const std::vector<float>& in_colors);
	void set_vertex_color(const glm::vec3& in_color, int index);
	void calc_normals(const std::vector<float>& in_vertices);
	int triangle_count();
	int vertex_count();
	void bind();
	void load_obj(const std::string& in_file, bool indexed=false);
};

}
