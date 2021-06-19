#pragma once
#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>
#include <fstream>
#include <vector>

#include "glm/glm.hpp"

#include "gl_object.h"

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

	GLuint get_uniform_location(const std::string& uniform_name);

	void set_uniform_i1(GLuint uniform_id, GLint in_val);

	void set_uniform_1f(GLuint uniform_id, GLfloat in_val);

	void set_uniform_mat4(GLuint uniform_id, const glm::mat4& in_val);

	void use();
};

#endif  // _SHADER_H_
