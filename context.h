#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <cstdio>
#include <chrono>

struct GLFWwindow;

extern void glfw_error_callback(int error, const char* description);

extern void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

namespace bgfx
{

class Context
{
    float time = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    int fps_frame_count = 30;
    int frame_count = 0;
    double frame_average = 0;
    GLFWwindow* _window;

public:
    Context(int window_x, int window_y);

    bool bgfx_window_should_close();

    void start_frame();

    void end_frame();

    GLFWwindow* get_window() { return _window; }
};

}  // namespace bgfx

#endif  // _CONTEXT_H_