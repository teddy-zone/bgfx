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
uniform sampler2D object_id_tex;

uniform int selected_object;

float evaluate_ssao(vec3 cur_loc, vec3 cur_norm)
{
    ivec2 texture_size = textureSize(position_tex, 0);
    float div_factor_x = 1.0/texture_size.x;
    float div_factor_y = 1.0/texture_size.y;
    const int spread = 6;
    float ssao_factor = 1.0;
    for (int x = -spread; x <= spread; x+=2)
    {
        for (int y = -(spread-x); y <= (spread-x); y+=2)
        {
	    vec3 other_pos = texture(position_tex, uv+vec2(x*div_factor_x,y*div_factor_y)).xyz*100.0;
	    float dist = length(other_pos - cur_loc);
	    float angle_factor = clamp(pow(dot(cur_norm, normalize(other_pos - cur_loc)),1), 0, 1);
	    if (dist < 1.5 && angle_factor > 0.5)
            {
                //ssao_factor += clamp(pow(dot(cur_norm, normalize(other_pos - cur_loc)),2), 0, 1)*1.0/pow(1+dist,2);
                ssao_factor += angle_factor*1;
            }
        }
    }
    return ssao_factor;
}

void main()
{    
    vec3 norm = texture(normal_tex, uv).xyz;
    vec3 color = texture(color_tex, uv).xyz;
    float object_id = texture(object_id_tex, uv).x;
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
    if (int(object_id) == selected_object)
    {
        color *= vec3(1.0,0.5,2.0);
    }
    float gamma = 2.3;
    vec4 pre_color = vec4(factor*color,1);//evaluate_ssao(post, norm);
    diffuseColor = vec4(pow(pre_color.rgb, vec3(1.0/gamma)), 1);
}  
