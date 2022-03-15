#pragma once

#include "texture.h"
#include "framebuffer.h"
#include "material.h"
#include "renderable_mesh.h"

namespace bgfx
{

    class DeferredRenderer
    {


        std::shared_ptr<bgfx::Framebuffer> g_buffer;
        
        

    public:

        std::shared_ptr<bgfx::Texture> normal_pass_tex;
        std::shared_ptr<bgfx::Texture> color_pass_tex;
        std::shared_ptr<bgfx::Texture> position_pass_tex;
        std::shared_ptr<bgfx::Texture> depth_pass_tex;

        std::shared_ptr<bgfx::RenderableMesh> rmesh;
        std::shared_ptr<bgfx::Material> quad_mat;
        std::shared_ptr<bgfx::Material> gmat;
        DeferredRenderer(int x_res, int y_res);

        virtual void draw();

        void bind_default();

    };

}