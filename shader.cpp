#pragma once

#include "shader.h"

#include <string>
#include <fstream>
#include <vector>

#include "glm/gtc/type_ptr.hpp"

#include "gl_object.h"



Shader::Shader(Shader::Type in_type, const std::string in_string, bool source_text):
    _type(in_type)
{
    if (_type == Shader::Type::Fragment)
    {
        _gl_id = glCreateShader(GL_FRAGMENT_SHADER);
    }
    else if (_type == Shader::Type::Vertex)
    {
        _gl_id = glCreateShader(GL_VERTEX_SHADER);
    }
    else if (_type == Shader::Type::Compute)
    {
        _gl_id = glCreateShader(GL_COMPUTE_SHADER);
    }
    const char* strings[1];
    GLint lengths[1];
    if (!source_text)
    {
        std::ifstream t2(in_string);
        std::vector<char> source_string((std::istreambuf_iterator<char>(t2)),
        std::istreambuf_iterator<char>());
        lengths[0] = source_string.size();
        strings[0] = source_string.data();
    }
    else
    {
        lengths[0] = in_string.size();
        strings[0] = in_string.data();
    }

    glShaderSource(_gl_id, 1, strings, lengths);
    glCompileShader(_gl_id);
    GLint isCompiled = 0;
    glGetShaderiv(_gl_id, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(_gl_id, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(_gl_id, maxLength, &maxLength, &errorLog[0]);
        printf("GL shader compile error: %s\n", errorLog.data());

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(_gl_id); // Don't leak the shader.
    }
}

ShaderProgram::ShaderProgram()
{
    _gl_id = glCreateProgram();
}

void ShaderProgram::attach_shader(std::shared_ptr<Shader> in_shader)
{
    glAttachShader(_gl_id, in_shader->_gl_id);
}

void ShaderProgram::link()
{
    glLinkProgram(_gl_id);
    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(_gl_id, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(_gl_id, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(_gl_id, maxLength, &maxLength, &infoLog[0]);

        printf("Error linking shader program: %s", infoLog.data());
        // We don't need the program anymore.
        glDeleteProgram(_gl_id);
    }

}

GLuint ShaderProgram::get_uniform_location(const std::string& uniform_name)
{
    return glGetUniformLocation(_gl_id, uniform_name.c_str());
}

void ShaderProgram::set_uniform_i1(GLuint uniform_id, GLint in_val)
{
    glUniform1i(uniform_id, in_val);
}

void ShaderProgram::set_uniform_1f(GLuint uniform_id, GLfloat in_val)
{
    glUniform1f(uniform_id, in_val);
}

void ShaderProgram::set_uniform_mat4(GLuint uniform_id, const glm::mat4& in_val)
{
    glUniformMatrix4fv(uniform_id, 1, false, glm::value_ptr(in_val));
}

void ShaderProgram::use()
{
    glUseProgram(_gl_id);
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR!: %d", err);
    }
}
