#pragma once

#include <memory>
#include <deque>
#include <string>

#include "texture.h"

namespace bgfx
{

	class Framebuffer
	{
		unsigned int _gl_id;
		unsigned int _size_x;
		unsigned int _size_y;
		std::string _name;

	public:

		Framebuffer(const std::string& name);
		~Framebuffer();
		Framebuffer(const std::string& name, unsigned int size_x, unsigned int size_y);

		void bind();
		const std::string& name();
		unsigned int gl_id() { return _gl_id; }
		void attach_texture(std::shared_ptr<Texture>& in_tex, int in_slot);
	};

}  // namespace bgfx
