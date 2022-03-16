#include "glad/gl.h"

#include "deferred_renderer.h"

namespace bgfx
{

    DeferredRenderer::DeferredRenderer(int x_res, int y_res)
    {
#ifdef ENABLE_GRAPHICS
        normal_pass_tex = std::make_shared<bgfx::Texture>("normal", x_res, y_res);
        color_pass_tex = std::make_shared<bgfx::Texture>("color", x_res, y_res);
        position_pass_tex = std::make_shared<bgfx::Texture>("position", x_res, y_res);
        depth_pass_tex = std::make_shared<bgfx::Texture>("depth", x_res, y_res);
        normal_pass_tex->to_render(x_res, y_res);
        color_pass_tex->to_render(x_res, y_res);
        position_pass_tex->to_render(x_res, y_res);
        depth_pass_tex->to_depth(x_res, y_res);
        g_buffer = std::make_shared<bgfx::Framebuffer>("g_buffer", x_res, y_res);
        g_buffer->bind();
       
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            printf("GL ERROR!: %d", err);
        }
        std::vector<unsigned int> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments.data());
        g_buffer->attach_texture(position_pass_tex, 0);
        g_buffer->attach_texture(normal_pass_tex, 1);
        g_buffer->attach_texture(color_pass_tex, 2);
        g_buffer->attach_depth_texture(depth_pass_tex);

        while ((err = glGetError()) != GL_NO_ERROR)
        {
            printf("GL ERROR!: %d", err);
        }
        
#endif
        quad_mat = std::make_shared<bgfx::Material>("C:\\Users\\tjwal\\projects\\game_stuff\\bare_gfx\\shaders\\deferred_quad\\VertexShader.glsl", 
            "C:\\Users\\tjwal\\projects\\game_stuff\\bare_gfx\\shaders\\deferred_quad\\FragmentShader.glsl");

        rmesh = std::make_shared<bgfx::RenderableMesh>();
        auto mesh = std::make_shared<bgfx::Mesh>();
        rmesh->set_mesh(mesh);
        rmesh->get_mesh()->set_vertices({ {1,0,0, 0,1,0, 0,0,0, 0,1,0, 1,1,0, 1,0,0} });
        rmesh->get_mesh()->set_uv_coords({ {1,0, 0,1, 0,0, 0,1, 1,1, 1,0} });
        rmesh->set_material(quad_mat);

        gmat = std::make_shared<bgfx::Material>("C:\\Users\\tjwal\\projects\\game_stuff\\bare_gfx\\shaders\\VertexShader.glsl", "C:\\Users\\tjwal\\projects\\game_stuff\\bare_gfx\\shaders\\deferred_shader.glsl");
        bind_default();
    }

    void DeferredRenderer::draw()
    {
        
        g_buffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //std::vector<unsigned int> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        //glDrawBuffers(3, attachments.data());
        gmat->use();
    }

    void DeferredRenderer::bind_default()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}