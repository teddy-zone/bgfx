#version 460 core

layout(location = 0) out vec4 diffuseColor;

in vec2 uv;

struct PointLight
{
    vec4 location;
    vec4 color;
    float intensity;
};

layout (std430, binding=5) buffer point_lights_uni
{
    PointLight point_lights[];
};
uniform int point_light_count;

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D color_tex;

void main()
{    
    vec3 norm = texture(normal_tex, uv).xyz;
    vec3 post = texture(position_tex, uv).xyz*100.0;
    vec3 factor = vec3(0.0); 
    if (point_light_count > 1)
    {
    for (int i = 0; i < point_light_count; ++i)
    {
    vec3 r = point_lights[i].location.xyz - post;

    factor += point_lights[i].color.xyz*point_lights[i].intensity*20000*clamp(dot(norm, normalize(r)), 0,1)/(length(r)*length(r));
    }
    }
    diffuseColor = vec4(factor,1)*vec4(1,1,1,1);
}  
