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
public:

    typedef Node<std::string> MatNode;

private:

	
	ShaderProgram _program;
	std::shared_ptr<Shader> _vertex_shader;
	std::shared_ptr<Shader> _fragment_shader;
	std::vector<std::shared_ptr<Texture>> _textures;
	std::string _vertex_shader_text;
	std::string _fragment_shader_text;
	std::string _texture_sampler_text;
	std::string _texture_execution_text;
	std::vector<std::pair<std::string, std::string>> _uniforms;
	std::vector<std::tuple<std::string,std::string,std::string>> _constants;
    MatNode* _uv_node;
    MatNode* _pos_node;
	MatNode* _norm_node;
	MatNode* _vert_color_node;
    MatNode* _frag_color_node;

    NodeGraph<std::string> _node_graph;

public:
    
    Material(const std::string& v_shader_file, const std::string& f_shader_file, bool source_text=false)
    {
		if (source_text)
		{
			_vertex_shader = std::make_shared<Shader>(Shader::Type::Vertex, v_shader_file, true);
			_fragment_shader = std::make_shared<Shader>(Shader::Type::Fragment, f_shader_file, true);
		}
		else
		{
			_vertex_shader = std::make_shared<Shader>(Shader::Type::Vertex, v_shader_file);
			_fragment_shader = std::make_shared<Shader>(Shader::Type::Fragment, f_shader_file);
		}
        link();
    }

	Material() :
		_vertex_shader_text(default_mat_vertex_shader),
		_fragment_shader_text(default_mat_fragment_shader),
		_texture_sampler_text(""),
		_texture_execution_text("")
	{
        _uv_node = new MatNode("uv");
        _uv_node->add_output("uv", "vec2");
        _pos_node = new MatNode("pos");
        _pos_node->add_output("pos", "vec3");
        _norm_node = new MatNode("norm");
        _norm_node->add_output("norm", "vec3");
		_vert_color_node = new MatNode("color");
		_vert_color_node->add_output("color", "vec4");
		_uv_node->required = true;
		_norm_node->required = true;
		_pos_node->required = true;
		_vert_color_node->required = true;
        _node_graph.add_node(_uv_node);
        _node_graph.add_node(_pos_node);
		_node_graph.add_node(_norm_node);
		_node_graph.add_node(_vert_color_node);
        _frag_color_node = new MatNode("frag_color");
        _frag_color_node->add_input("gl_FragColor", "");
		_frag_color_node->required = true;
        _node_graph.add_node(_frag_color_node);
	}

	void set_vertex_shader(std::shared_ptr<Shader> v_shader)
	{
		_vertex_shader = v_shader;
	}

	void set_fragment_shader(std::shared_ptr<Shader> f_shader)
	{
		_fragment_shader = f_shader;
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

	MatNode* add_texture(std::shared_ptr<Texture> in_tex)
	{
        MatNode* tex_node = new MatNode(in_tex->name());
        tex_node->add_input("tex_coord", "vec2");
        tex_node->add_output("tex_value", "vec4");
        _node_graph.add_node(tex_node);
		_textures.push_back(in_tex);
        _uniforms.push_back(std::pair<std::string,std::string>("sampler2D", in_tex->name()));
        tex_node->data = "vec4 tex_value = texture(" + in_tex->name() + ", " + "tex_coord" + ");\n";

		return tex_node;
	}

	MatNode* sum_node(const std::string& type)
	{
		static int id_counter = 0;
		MatNode* sum_node = new MatNode("sum" + std::to_string(id_counter++));
		sum_node->add_input("in1", type);
		sum_node->add_input("in2", type);
		sum_node->add_output("sum_out", type);
		sum_node->data = type + " sum_out = in1 + in2;\n";
		_node_graph.add_node(sum_node);
		return sum_node;
	}

	void compile();

	void make_connection(
		std::shared_ptr<NodeOutput> output,
		std::shared_ptr<NodeInput> input)
	{

	}

	MatNode* add_uniform(const std::string& type, const std::string& name)
	{
		MatNode* out_node;
		_uniforms.push_back(std::pair<std::string, std::string>(type, name));
		out_node = new MatNode(name);
		out_node->add_output(name, type);
		out_node->required = true;
		_node_graph.add_node(out_node);
		return out_node;
	}

	MatNode* add_constant(const std::string& type, const std::string& name, const std::string& value)
	{
		MatNode* out_node;
		_constants.push_back(std::make_tuple(type, name, value));
		out_node = new MatNode(name);
		out_node->add_output(name, type);
		out_node->required = true;
		_node_graph.add_node(out_node);
		return out_node;
	}

	MatNode* uv_node()
	{
		return _uv_node;
	}
	MatNode* pos_node()
	{
		return _pos_node;
	}
	MatNode* normal_node()
	{
		return _norm_node;
	}
	MatNode* vertex_color_node()
	{
		return _vert_color_node;
	}
	MatNode* frag_color_node()
	{
		return _frag_color_node;
	}
	void link();

	void set_uniform_i1(const std::string& in_name, int in_val);

	void set_uniform_1f(const std::string& in_name, float in_val);

	void set_uniform_mat4(const std::string& in_name, const glm::mat4& in_val);

	void set_uniform_3f(const std::string& in_name, const glm::vec3& in_val);

	unsigned int get_program_id() const
	{
		return _program.get_id();
	}
};

}  // namespace bgfx
