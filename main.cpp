#include "glad/gl.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <random>

#include "buffer.h"
#include "vertex_array.h"
#include "shader.h"
#include "gl_octree.h"
#include "SimplexNoise.h"

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}


int main()
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

    GLuint compute_hit_loc;
    glGenTextures(1, &compute_hit_loc);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, compute_hit_loc);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, x_res, y_res, 0, GL_RGB, GL_FLOAT, 0);// test_texture.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindImageTexture(1, compute_hit_loc, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    GLuint compute_hit_norm;
    glGenTextures(1, &compute_hit_norm);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, compute_hit_norm);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, x_res, y_res, 0, GL_RGB, GL_FLOAT, 0);// test_texture.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindImageTexture(2, compute_hit_norm, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    GLuint out_color_tex;
    glGenTextures(1, &out_color_tex);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, out_color_tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, x_res, y_res, 0, GL_RGB, GL_FLOAT, 0);// test_texture.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindImageTexture(3, out_color_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    // Vertex data for full screen quad
    std::vector<GLfloat> compute_render_poly_vertex = { {-1,-1,-1,1,1,1,1,1,1,-1,-1,-1} };
    Buffer<GLfloat> compute_render_poly_buffer(compute_render_poly_vertex, BufferType::VBO, GL_DYNAMIC_DRAW);

    // VAO
    //GLuint quad_vao;
    VertexArray quad_vao;
    quad_vao.add_buffer(&compute_render_poly_buffer, "vertices", 2);

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

    //tex_loc = compute_prog.get_uniform_location("img_output");
    //compute_prog.set_uniform_i1(tex_loc, 0);
    
    GLOctree octree(1024);
    //octree.nodes[0].data = 0;

    SimplexNoise sn(10.1f,
        1.0f,1000);

    for (int i = 1; i < 400; ++i)
    {
        for (int j = 0; j < 400; ++j)
        {
            double prob = std::rand() * 1.0 / RAND_MAX;
            if (prob < 0.01)
            {
                //place_node(octree.nodes, 0, glm::vec3(0, 0, 0), 128, glm::vec3(i, j, 8), 4.0);
                //place_node(octree.nodes, 0, glm::vec3(0, 0, 0), 128, glm::vec3(i, j, 12), 4.0);
                //place_node(octree.nodes, 0, glm::vec3(0, 0, 0), 128, glm::vec3(i, j, 16), 4.0);
                //place_node(octree.nodes, 0, glm::vec3(0, 0, 0), 128, glm::vec3(i, j, 20), 4.0);
            }
        }
    }
    float freq = 50.0;
    float freq2 = 8.0;
    float freq3 = 120.0;
    for (int i = 1; i < 800; ++i)
    {
        for (int j = 0; j < 800; ++j)
        {
            int height = std::rand() * 15.0 / RAND_MAX;
            int size = std::rand() * 4.0 / RAND_MAX - 4;
            if (size == 0) { size == 1; }
            float k = 0;

            //while (k < height)
            {
                float height_val = 6.0 * (sn.noise(i / freq, j / freq) + 1.0);
                height_val += 1.0 * (sn.noise(i / freq2 + 1000, j / freq2 + 1000) + 1.0);
                height_val += 20.0 * (sn.noise(i / freq3 + 21000, j / freq3 + 6500) + 1.0);

                int data = 1;
                if (height_val < 20.0) { data = 2; }
                place_node(octree.nodes, 0, glm::vec3(0, 0, 0), 1024, glm::vec3(i, j, height_val), 1.0, data);// std::pow(2.0, float(size)));
                //place_node(octree.nodes, 0, glm::vec3(0, 0, 0), 256, glm::vec3(i, j, height_val + 1), pow(2.0, size), 1);// std::pow(2.0, float(size)));
                place_node(octree.nodes, 0, glm::vec3(0, 0, 0), 1024, glm::vec3(i, j, height_val - 1), 1.0, data);
                place_node(octree.nodes, 0, glm::vec3(0, 0, 0), 1024, glm::vec3(i, j, 1), 1.0, 1);
                for (int m = 0; m < 18; ++m)
                {
                    place_node(octree.nodes, 0, glm::vec3(0, 0, 0), 1024, glm::vec3(i, j, m), 1.0, 4);
                }
                //place_node(octree.nodes, 0, glm::vec3(0, 0, 0), 2048, glm::vec3(i, j, 3), 1.0, 4);
                //k += std::pow(2.0, float(size));
            }
        }
    }

    octree.refresh_data();
    compute_prog.use();
    auto root_size_loc = compute_prog.get_uniform_location("root_size");
    compute_prog.set_uniform_1f(root_size_loc, octree.root_size);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, octree.node_buffer->get_gl_id());

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
        { // launch compute shaders!
            filter_prog.use();
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
