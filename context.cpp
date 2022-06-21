#include "context.h"

#include <chrono>

#include "glad/gl.h"
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

namespace bgfx
{

Context::Context(int window_x, int window_y)
{
    const char* glsl_version = "#version 150";
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        printf("GLFW init failed!\n");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    int x_res = window_x, y_res = window_y;
    _window = glfwCreateWindow(x_res, y_res, "My Title", NULL, NULL);
    if (!_window)
    {
        // Window or context creation failed
    }

    glfwSetKeyCallback(_window, glfw_key_callback);

    glfwMakeContextCurrent(_window);
    gladLoadGL(glfwGetProcAddress);
    //glEnable(GL_FRAMEBUFFER_SRGB); 
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1000.0f);



    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    auto start = std::chrono::high_resolution_clock::now();
    glClearColor(0.5, 0.3, 0.2, 1.0);
    glfwSwapInterval(0);
    
}

bool Context::bgfx_window_should_close()
{
    return glfwWindowShouldClose(_window);
}

void Context::start_frame()
{
    glfwPollEvents();
    
    // Keep running6
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    

}

void Context::end_frame()
{
    
    glfwSwapBuffers(_window);
    time += 0.01;
    //quad.translate(glm::vec3(0, 0, 0.001));
    frame_count += 1;
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed_micro = std::chrono::duration<double, std::micro>(end - start).count();
    frame_average += elapsed_micro;
    if (frame_count == 300)
    {

        printf("FPS: %f\n", 300.0 / (frame_average / 1000000));
        frame_count = 0;
        start = end;
        frame_average = 0;
    }
}

}  // namespace bgfx
