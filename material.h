#pragma once

#include "shader.h"

namespace bgfx
{

extern std::string default_mat_vertex_shader;
extern std::string default_mat_fragment_shader;

class Material
{
	ShaderProgram _program;
	std::shared_ptr<Shader> _vertex_shader;
	std::shared_ptr<Shader> _fragment_shader;
public:
	Material()
	{
		_vertex_shader = std::make_shared<Shader>(Shader::Type::Vertex, default_mat_vertex_shader, true);
		_fragment_shader = std::make_shared<Shader>(Shader::Type::Fragment, default_mat_fragment_shader, true);
		_program.attach_shader(_vertex_shader);
		_program.attach_shader(_fragment_shader);
		_program.link();
	}

	void use()
	{
		_program.use();
	}

	void set_transform(const glm::mat4& transform, std::string mat_name)
	{
		auto matrix_id = _program.get_uniform_location(mat_name);
		_program.set_uniform_mat4(matrix_id, transform);
	}
};

}  // namespace bgfx