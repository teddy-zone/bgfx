#include "shader.h"

#include <string>
#include <fstream>
#include <vector>
#include <sstream>

#include "glm/gtc/type_ptr.hpp"

#include "glad/gl.h"
#include <GLFW/glfw3.h>

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
    std::ifstream in_stream(in_string);
    std::stringstream strstream;
    std::string source_string;
    if (!source_text)
    {
        strstream << in_stream.rdbuf();
        source_string = strstream.str();
        lengths[0] = strstream.str().size();
        strings[0] = source_string.c_str();
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

unsigned int ShaderProgram::get_uniform_location(const std::string& uniform_name)
{
    auto loc = glGetUniformLocation(_gl_id, uniform_name.c_str());
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR!: %d", err);
		throw "opengl error";
    }

    return loc;
}

void ShaderProgram::set_uniform_i1(unsigned int uniform_id, int in_val)
{
    glUniform1i(uniform_id, in_val);
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR!: %d", err);
		throw "opengl error";
    }
}

void ShaderProgram::set_uniform_1f(unsigned int uniform_id, float in_val)
{
    glUniform1f(uniform_id, in_val);
}

void ShaderProgram::set_uniform_mat4(unsigned int uniform_id, const glm::mat4& in_val)
{
    glUniformMatrix4fv(uniform_id, 1, false, glm::value_ptr(in_val));
}

void ShaderProgram::set_uniform_3f(const std::string& in_name, const glm::vec3& in_val)
{
    glUniform3fv(get_uniform_location(in_name), 1, glm::value_ptr(in_val));
}

void ShaderProgram::use()
{
    glUseProgram(_gl_id);
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR!: %d", err);
		throw "opengl error";
    }
}
