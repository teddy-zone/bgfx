#pragma once

#include <tuple>
#include <regex>

#include "shader.h"
#include "texture.h"
#include "material_node.h"

namespace bgfx
{

extern std::string default_mat_vertex_shader;
extern std::string default_mat_fragment_shader;

class Material
{
private:

	
	ShaderProgram _program;
	std::shared_ptr<Shader> _vertex_shader;
	std::shared_ptr<Shader> _fragment_shader;
	std::vector<std::tuple<std::shared_ptr<Texture>, bool>> _textures;
	std::string _vertex_shader_text;
	std::string _fragment_shader_text;
	std::string _texture_sampler_text;
	std::string _texture_execution_text;
	static std::shared_ptr<NodeOutput> _uv_node_out;
	static std::shared_ptr<NodeOutput> _pos_node_out;
	static std::shared_ptr<NodeOutput> _normal_node_out;
    NodeGraph _node_graph;

public:

	Material() :
		_vertex_shader_text(default_mat_vertex_shader),
		_fragment_shader_text(default_mat_fragment_shader),
		_texture_sampler_text(""),
		_texture_execution_text("")
	{

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

	void add_texture(std::shared_ptr<Texture> in_tex, 
		std::shared_ptr<NodeInput> tex_coord, 
		std::shared_ptr<NodeOutput> tex_out)
	{
		_textures.push_back(std::make_tuple(in_tex, true));
		_texture_sampler_text += "uniform sampler2D " + in_tex->name() + ";\n";
		_texture_execution_text += "texture(" + in_tex->name() + ", " + tex_coord->var_name() + ");\n";
	}

	void compile()
	{
		// Compile our structures into our glsl fragment shader
		_fragment_shader_text = std::regex_replace(_fragment_shader_text, 
			std::regex("//_TEX_SAMPLERS_"), 
			_texture_sampler_text);
		_fragment_shader_text = std::regex_replace(_fragment_shader_text,
			std::regex("//_TEX_EXECUTION_"),
			_texture_execution_text);
		printf("complete frag text: %s\n", _fragment_shader_text.c_str());
		_vertex_shader = std::make_shared<Shader>(Shader::Type::Vertex, _vertex_shader_text, true);
		_fragment_shader = std::make_shared<Shader>(Shader::Type::Fragment, _fragment_shader_text, true);
		_program.attach_shader(_vertex_shader);
		_program.attach_shader(_fragment_shader);
		_program.link();
	}

	void make_connection(
		std::shared_ptr<NodeOutput> output,
		std::shared_ptr<NodeInput> input)
	{

	}

	static const std::shared_ptr<NodeOutput> uv_node_out()
	{
		return _uv_node_out;
	}
	static const std::shared_ptr<NodeOutput> pos_node_out()
	{
		return _pos_node_out;
	}
	static const std::shared_ptr<NodeOutput> normal_node_out()
	{
		return _normal_node_out;
	}
};

}  // namespace bgfx
