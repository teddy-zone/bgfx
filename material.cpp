#include "material.h"


#include "glad/gl.h"
#include <GLFW/glfw3.h>

namespace bgfx
{

std::string default_mat_vertex_shader = R"V0G0N(

#version 430 core
layout (location = 0) in vec3 vpos; // the position variable has attribute position 0
layout (location = 1) in vec3 vnorm; // the position variable has attribute position 0
layout (location = 2) in vec2 vuv; // the position variable has attribute position 0
layout (location = 3) in vec4 vcolor; // the position variable has attribute position 0
 
uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

out vec4 color; // specify a color output to the fragment shader
out vec2 uv;

void main()
{
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vpos, 1.0);
    //gl_Position = vec4(vpos, 1.0);
    color = vcolor; // set the output variable to a dark-red color
    uv = vuv;
}

)V0G0N";

std::string default_mat_fragment_shader = R"V0G0N(

#version 430 core

in vec4 color; // specify a color output to the fragment shader
in vec3 pos; // specify a color output to the fragment shader
in vec3 norm; // specify a color output to the fragment shader
in vec2 uv;
layout(location = 0) out vec4 diffuseColor;

//_UNIFORMS_

void main()
{
    //_NODE_ASSEMBLY_
    //gl_FragColor = vertex_color;
}

)V0G0N";

void Material::compile()
{
	// Compile our structures into our glsl fragment shader
	std::string uniform_shader_string;
	std::string node_assembly_string;
	for (auto& [uni_type, uni_name] : _uniforms)
	{
		uniform_shader_string += "uniform " + uni_type + " " + uni_name + ";\n";
	}
	for (auto& [uni_type, uni_name, value] : _constants)
	{
		uniform_shader_string += "uniform " + uni_type + " " + uni_name + " = " + value + ";\n";
	}
	auto sorted_nodes = _node_graph.sort_nodes();
	for (auto& node : sorted_nodes)
	{
		bool has_output = false;
		for (auto& [output_name, output] : node->outputs)
		{
			if (output.connections.size())
			{
				node_assembly_string += output.type + " " + output.name + "_" + output.id_string() + ";\n";
				has_output = true;
			}
		}
		if (has_output || node->required)
		{
			node_assembly_string += "{\n";
			for (auto& [input_name, input] : node->inputs)
			{
				if (input.connection)
				{
					node_assembly_string += input.type + " " + input.name + " = " + input.connection->name + "_" + input.connection->id_string() + ";\n";
				}
			}
			node_assembly_string += node->data;
			for (auto& [output_name, output] : node->outputs)
			{
				if (output.connections.size())
				{
					node_assembly_string += output.name + "_" + output.id_string() + " = " + output.name + ";\n";
				}
			}
			node_assembly_string += "}\n";
		}
		printf("Node name: %s\n", node->data.c_str());
	}
	_fragment_shader_text = std::regex_replace(_fragment_shader_text,
		std::regex("//_UNIFORMS_"),
		uniform_shader_string);
	_fragment_shader_text = std::regex_replace(_fragment_shader_text,
		std::regex("//_NODE_ASSEMBLY_"),
		node_assembly_string);
	printf("complete frag text: %s\n", _fragment_shader_text.c_str());
	_vertex_shader = std::make_shared<Shader>(Shader::Type::Vertex, _vertex_shader_text, true);
	_fragment_shader = std::make_shared<Shader>(Shader::Type::Fragment, _fragment_shader_text, true);

	unsigned int err;
	_program.attach_shader(_vertex_shader);
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}
	_program.attach_shader(_fragment_shader);
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}
	_program.link();
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}
	_program.use();
	for (auto& tex : _textures)
	{

		auto loc = _program.get_uniform_location(tex->name());
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			printf("GL ERROR!: %d", err);
		}
		glUniform1i(loc, tex->slot());
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			printf("GL ERROR!: %d", err);
		}
	}
}

void Material::link()
{
	unsigned int err;
	_program.attach_shader(_vertex_shader);
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}
	_program.attach_shader(_fragment_shader);
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}
	_program.link();
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}
	_program.use();
}

void Material::set_uniform_i1(const std::string& in_name, int in_val)
{
	auto loc = _program.get_uniform_location(in_name);
	_program.set_uniform_i1(loc, in_val);
}

void Material::set_uniform_1f(const std::string& in_name, float in_val)
{
	auto loc = _program.get_uniform_location(in_name);
	_program.set_uniform_1f(loc, in_val);
}

void Material::set_uniform_mat4(const std::string& in_name, const glm::mat4& in_val)
{
	auto loc = _program.get_uniform_location(in_name);
	_program.set_uniform_mat4(loc, in_val);
}

void Material::set_uniform_3f(const std::string& in_name, const glm::vec3& in_val)
{
	_program.set_uniform_3f(in_name, in_val);
}

}  // namespace bgfx
