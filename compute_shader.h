#pragma once

#include <string>
#include <map>
#include "shader.h"
#include "buffer.h"
#include "texture.h"

namespace bgfx
{

extern std::string default_compute_shader;
extern std::string cs_string;

class ComputeShader
{
private:
	int _local_size_x;
	int _local_size_y;
	int _local_size_z;

	int _call_size_x;
	int _call_size_y;
	int _call_size_z;

	std::shared_ptr<Shader> _shader;
	ShaderProgram _program;

	std::map<int, std::shared_ptr<BufferBase>> _buffers;
	std::map<int, std::shared_ptr<Texture>> _textures;

	std::string _user_code;

public:

	void set_local_size(int x, int y, int z)
	{
		_local_size_x = x;
		_local_size_y = y;
		_local_size_z = z;
	}

	void set_call_size(int x, int y, int z)
	{
		_call_size_x = x;
		_call_size_y = y;
		_call_size_z = z;
	}

	void add_buffer_input(std::shared_ptr<BufferBase> in_buf, int bind_point)
	{
		_buffers[bind_point] = in_buf;
	}

	void add_texture(std::shared_ptr<Texture> in_tex, int bind_point)
	{
		_textures[bind_point] = in_tex;
	}

	void set_code(const std::string& in_code)
	{
		_user_code = in_code;
	}

	void compile();

	void run();
};

}  // namespace bgfx
