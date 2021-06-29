#pragma once
#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>
#include <fstream>
#include <vector>

#include "glm/glm.hpp"

class Shader
{
	friend class ShaderProgram;
	unsigned int _gl_id;
public:
	enum class Type
	{
		Fragment,
		Vertex,
		Compute
	};

	Shader(Shader::Type in_type, const std::string in_string, bool source_text=false);

private:
	Shader::Type _type;

};

class ShaderProgram
{
	unsigned int _gl_id;
public:
	ShaderProgram();

	void attach_shader(std::shared_ptr<Shader> in_shader);

	void link();

	unsigned int get_uniform_location(const std::string& uniform_name);

	void set_uniform_i1(unsigned int uniform_id, int in_val);

	void set_uniform_1f(unsigned int uniform_id, float in_val);

	void set_uniform_mat4(unsigned int uniform_id, const glm::mat4& in_val);

	void set_uniform_3f(const std::string& in_name, const glm::vec3& in_val);

	void use();
};

#endif  // _SHADER_H_
