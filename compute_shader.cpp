#include "compute_shader.h"


#include "glad/gl.h"
#include <GLFW/glfw3.h>

namespace bgfx
{

std::string default_compute_shader = R"V0G0N(

#version 460

// _STRUCT_DEFS_

layout(local_size_x = _LOCAL_SIZE_X_, local_size_y = _LOCAL_SIZE_Y, local_size_z = _LOCAL_SIZE_Z_) in;

// _TEXTURE_BINDINGS_

// _BUFFER_BINDINGS_

// _USER_CODE_

)V0G0N";

std::string cs_string = R"V0G0N(

#version 460

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout(rgba32f, binding=1) uniform image2D tex_out;

void main()
{
	const ivec2 dims = imageSize(tex_out);
	const ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	imageStore(tex_out, pixel_coords, vec4(pixel_coords.x*1.0/dims.x));
}

)V0G0N";

void ComputeShader::compile()
{
	for (auto& [bind_point, buf] : _buffers)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bind_point, buf->get_id());
	}

	for (auto& [bind_point, tex] : _textures)
	{
		glBindImageTexture(bind_point, tex->gl_id(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	}

	_shader = std::make_shared<Shader>(Shader::Type::Compute, _user_code, true);
	_program.attach_shader(_shader);
	_program.link();
}

void ComputeShader::run()
{
	_program.use();
	glDispatchCompute(_call_size_x, _call_size_y, _call_size_z);
}

}  // namespace bgfx
