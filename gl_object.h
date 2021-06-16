#pragma once
#ifndef _GL_OBJECT_H_
#define _GL_OBJECT_H_

#include "glad/gl.h"
#include "GLFW/glfw3.h"

class GLObject
{
private:
	GLuint _gl_id;
	GLuint _bindpoint;
protected:
	
	void set_gl_id(GLuint gl_id) { _gl_id = gl_id; }
	
	void set_bindpoint(GLuint bindpoint) { _bindpoint = bindpoint; }
	
public:
	GLuint& get_bindpoint() { return _bindpoint; }
	virtual void bind(GLuint bindpoint = -1) {}
	GLuint& get_gl_id() { return _gl_id; }
	GLObject();
};

#endif  // _GL_OBJECT_H_
