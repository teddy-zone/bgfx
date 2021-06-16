#pragma once
#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>
#include <fstream>
#include <vector>

#include "gl_object.h"

class Shader : public GLObject
{
	friend class ShaderProgram;
public:
	enum Type
	{
		Fragment,
		Vertex,
		Compute
	};

	Shader(Shader::Type in_type, std::string source_file):
		_type(in_type)
	{
		if (_type == Shader::Type::Fragment)
		{
			set_gl_id(glCreateShader(GL_FRAGMENT_SHADER));
		}
		else if (_type == Shader::Type::Vertex)
		{
			set_gl_id(glCreateShader(GL_VERTEX_SHADER));
		}
		else if (_type == Shader::Type::Compute)
		{
			set_gl_id(glCreateShader(GL_COMPUTE_SHADER));
		}
		std::ifstream t2(source_file);
		std::vector<char> source_string((std::istreambuf_iterator<char>(t2)),
			std::istreambuf_iterator<char>());
		char* strings[1];
		GLint lengths[1];
		lengths[0] = source_string.size();
		strings[0] = source_string.data();

		glShaderSource(get_gl_id(), 1, strings, lengths);
		glCompileShader(get_gl_id());
		GLint isCompiled = 0;
		glGetShaderiv(get_gl_id(), GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(get_gl_id(), GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(get_gl_id(), maxLength, &maxLength, &errorLog[0]);
			printf("GL shader compile error: %s\n", errorLog.data());

			// Provide the infolog in whatever manor you deem best.
			// Exit with failure.
			glDeleteShader(get_gl_id()); // Don't leak the shader.
		}
	}

private:
	Shader::Type _type;

};

class ShaderProgram : public GLObject
{
public:
	ShaderProgram()
	{
		set_gl_id(glCreateProgram());
	}

	void attach_shader(Shader& in_shader)
	{
		glAttachShader(get_gl_id(), in_shader.get_gl_id());
	}

	void link()
	{
		glLinkProgram(get_gl_id());
		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(get_gl_id(), GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(get_gl_id(), GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(get_gl_id(), maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(get_gl_id());
		}

	}

	GLuint get_uniform_location(const std::string& uniform_name)
	{
		return glGetUniformLocation(get_gl_id(), uniform_name.c_str());
	}

	void set_uniform_i1(GLuint uniform_id, GLint in_val)
	{
		glUniform1i(uniform_id, in_val);
	}

	void set_uniform_1f(GLuint uniform_id, GLfloat in_val)
	{
		glUniform1f(uniform_id, in_val);
	}

	void use()
	{
		glUseProgram(get_gl_id());
	}
};

#endif  // _SHADER_H_
