#version 430 core

layout(location = 0) out vec4 diffuseColor;

in vec2 uv;

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D color_tex;

void main()
{    
    diffuseColor = texture(normal_tex, uv);
}  
