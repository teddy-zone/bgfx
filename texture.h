#pragma once

#include <deque>

namespace bgfx
{

class Texture
{
	unsigned int _gl_id;
	unsigned int _size_x;
	unsigned int _size_y;
	int _texture_slot;
	static int _current_texture_slot;
	static std::deque<int> _free_slots;

public:
	Texture();
	~Texture();
	Texture(unsigned int size_x, unsigned int size_y);
	void bind();
	void load_data();
};

}  // namespace bgfx