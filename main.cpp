#include "glad/gl.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <random>

#include "renderable_mesh.h"
#include "buffer.h"
#include "camera.h"
#include "material_node.h"

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

GLFWwindow* gl_init(int window_x, int window_y)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        printf("GLFW init failed!\n");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    int x_res = window_x, y_res = window_y;
    GLFWwindow* window = glfwCreateWindow(x_res, y_res, "My Title", NULL, NULL);
    if (!window)
    {
        // Window or context creation failed
    }

    glfwSetKeyCallback(window, glfw_key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    return window;
}

int main()
{


    int x_res = 1200, y_res = 800;
    auto window = gl_init(x_res, y_res);

    auto camera = bgfx::Camera(x_res, y_res);
    camera.set_position(glm::vec3(3, 3, 3));
    camera.set_look_target(glm::vec3(0, 0, 0));

    auto quad_mesh = std::make_shared<bgfx::Mesh>();
    auto quad_mat = std::make_shared<bgfx::Material>();
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR!: %d", err);
    }
    auto quad_tex = std::make_shared<bgfx::Texture>("quad_tex", x_res, y_res);
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR!: %d", err);
    }

    bgfx::Material::MatNode* tex_out;
    tex_out = quad_mat->add_texture(quad_tex);
    auto uv_node = quad_mat->uv_node();
    uv_node->connect("uv_coord", &tex_out->inputs["tex_coord"]);

    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR!: %d", err);
    }
    quad_mat->compile();
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR!: %d", err);
    }
    //quad_mesh->set_vertices({ {1,0,0,0,1,0,0,0,1,1,1,0} });
    quad_mesh->set_vertices({ {1,0,0, 0,1,0, 0,0,0, 0,1,0, 1,1,0, 1,0,0} });
    bgfx::RenderableMesh quad;
    quad.set_mesh(quad_mesh);
    quad.set_material(quad_mat);

    glClearColor(0.5, 0.3, 0.2, 1.0);
    float time = 0;
    int fps_frame_count = 30;
    int frame_count = 0;
    double frame_average = 0;
    glfwSwapInterval(0);
    while (!glfwWindowShouldClose(window))
    {
        auto start = std::chrono::high_resolution_clock::now();
        // Keep running6
        glClear(GL_COLOR_BUFFER_BIT);
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            printf("GL ERROR!: %d", err);
        }
        camera.draw_object(quad);

        glfwSwapBuffers(window);
        time += 0.01;

        frame_count += 1;
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed_micro = std::chrono::duration<double, std::micro>(end - start).count();
        frame_average += elapsed_micro;
        if (frame_count == 30)
        {
            
            printf("FPS: %f\n", 30.0 / (frame_average / 1000000));
            frame_count = 0;
            start = end;
            frame_average = 0;
        }
    }
}
/*
int orig_main()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        printf("GLFW init failed!\n");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    int x_res = 1200, y_res = 800;
    GLFWwindow* window = glfwCreateWindow(x_res, y_res, "My Title", NULL, NULL);
    if (!window)
    {
        // Window or context creation failed
    }

    glfwSetKeyCallback(window, glfw_key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    // Texture for compute shader to render to
    GLuint compute_render_texture;
    glGenTextures(1, &compute_render_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, compute_render_texture);
    //std::vector<GLfloat> test_texture = { {1,0,0,0,1,0,0,0,1,1,1,0} };
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, x_res, y_res, 0, GL_RGB, GL_FLOAT, 0);// test_texture.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindImageTexture(0, compute_render_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    // Vertex data for full screen quad
    std::vector<GLfloat> compute_render_poly_vertex = { {-1,-1,-1,1,1,1,1,1,1,-1,-1,-1} };
    Buffer<float> compute_render_poly_buffer(compute_render_poly_vertex, BufferType::VBO, GL_DYNAMIC_DRAW);

    // VAO
    //GLuint quad_vao;
    VertexArray quad_vao;
    quad_vao.add_buffer(&compute_render_poly_buffer, "vertices", 2, GL_FLOAT);

    Shader f_shader(Shader::Type::Fragment, "quad_fragment_shader.frag");
    Shader v_shader(Shader::Type::Vertex, "quad_vertex_shader.vert");
    Shader ray_compute_shader(Shader::Type::Compute, "ray_compute.shader");
    Shader filter_shader(Shader::Type::Compute, "filter_shader.shader");

    // Vertex shader to only render quad
    ShaderProgram prog;
    prog.attach_shader(f_shader);
    prog.attach_shader(v_shader);

    ShaderProgram compute_prog;
    ShaderProgram filter_prog;
    compute_prog.attach_shader(ray_compute_shader);
    compute_prog.link();
    filter_prog.attach_shader(filter_shader);
    filter_prog.link();
    //glBindImageTexture(0, compute_render_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    prog.link();

    GLuint tex_loc = prog.get_uniform_location("tex");
    prog.set_uniform_i1(tex_loc,2);

 
    compute_prog.use();

    glClearColor(0.5, 0.3, 0.2, 1.0);
    float time = 0;
    int fps_frame_count = 30;
    int frame_count = 0;
    auto start = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        
        // Keep running6
        glClear(GL_COLOR_BUFFER_BIT);

        { // launch compute shaders!
            compute_prog.use();
            auto time_loc = compute_prog.get_uniform_location("time");
            compute_prog.set_uniform_1f(time_loc, time);
            glDispatchCompute((GLuint)x_res, (GLuint)y_res, 1);
        }

        // make sure writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        prog.use();
        //quad_vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, compute_render_poly_vertex.size());
        glfwSwapBuffers(window);
        time += 0.01;
        
        frame_count += 1;
        if (frame_count == 30)
        {
            auto end = std::chrono::high_resolution_clock::now();
            double elapsed_milli = std::chrono::duration<double, std::milli>(end - start).count();
            printf("FPS: %f\n", 30.0 / (elapsed_milli / 1000));
            frame_count = 0;
            start = end;
        }
    }

    glfwDestroyWindow(window);
}
*/
/*
plane_point . plane_normal = plane_normal . (ray_point + ray_dir*t)
plane_point . plane_normal = plane_normal . ray_point + t*(plane_normal . ray_dir)
(plane_point . plane_normal - plane_normal . ray_point)/(plane_normal . ray_dir) = t

((ray_point + ray_dir*t) - sphere_loc) . ((ray_point + ray_dir*t) - sphere_loc) = r^2

(ray_point + ray_dir*t) . (ray_point + ray_dir*t) -
2*((ray_point + ray_dir*t) . sphere_loc) +
(sphere_loc) . (sphere_loc)

ray_point . ray_point + 2t*(ray_dir . ray_point) + t^2*(ray_dir . ray_dir) - 
*/
