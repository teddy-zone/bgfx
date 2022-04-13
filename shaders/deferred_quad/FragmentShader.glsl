#version 460 core

layout(location = 0) out vec4 diffuseColor;

in vec2 uv;

struct PointLight
{
    vec4 location;
    vec4 color;
    float intensity;
};

struct Decal
{
    vec4 location;
    vec4 color;
    float radius;
    float intensity;
    float t;
    int type;
    int object_id;
};

layout (std430, binding=5) buffer point_lights_uni
{
    PointLight point_lights[];
};

layout (std430, binding=6) buffer decals_uni
{
    Decal decals[];
};

uniform int point_light_count;
uniform int decal_count;

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

struct MatMod
{
    vec4 color;
    vec3 normal;
    float normal_factor;
    vec3 spec;
};

// Waves decal
MatMod evaluate_decal_1(in Decal in_decal, in vec3 pos)
{
    float wavelength = 1.0;
    float dist = length(in_decal.location.xy - pos.xy);  
    float wave_freq = 10.0/pow(dist + 1,2);
    vec3 radial = vec3(normalize(pos.xy - in_decal.location.xy), 0);  
    float wave_amplitude = 1/pow(dist + 1, 2);
    float val = wave_amplitude*sin(wave_freq*in_decal.t + dist*wavelength);
    float d_val = cos(wave_freq*in_decal.t + dist*wavelength);
    MatMod out_mod;
    out_mod.color = in_decal.color*val;
    out_mod.color.w = 1.0 - in_decal.t;
    out_mod.normal_factor = 1.0 - in_decal.t;
    out_mod.normal = normalize(radial + vec3(0,0,1)/(d_val));
    return out_mod;
}

// Icy decal
MatMod evaluate_decal_2(in Decal in_decal, in vec3 pos)
{
    MatMod out_mod;
    return out_mod;
}

MatMod evaluate_decal(in Decal in_decal, in vec3 pos)
{
    if (in_decal.type == 1)
    {
        return evaluate_decal_1(in_decal, pos);
    }
    else if (in_decal.type == 2)
    {
        return evaluate_decal_2(in_decal, pos);
    }
}

void main()
{    
    vec3 norm = texture(normal_tex, uv).xyz;
    vec3 color = texture(color_tex, uv).xyz;
    float object_id = texture(object_id_tex, uv).x;
    vec3 post = texture(position_tex, uv).xyz*100.0;
    vec3 factor = vec3(0.0); 

    MatMod full_mod;
    float mod_count = 0;
    for (int i = 0; i < decal_count; ++i)
    {
        if (length(post.xy - decals[i].location.xy) < decals[i].radius)
        {
            MatMod single_mod = evaluate_decal(decals[i], post);
            full_mod.color += single_mod.color;
            full_mod.normal += single_mod.normal;
            full_mod.normal_factor += single_mod.normal_factor;
            mod_count += 1;
        }
    }

    if (mod_count > 0)
    {
        //full_mod.color = normalize(full_mod.color);
        full_mod.normal= normalize(full_mod.normal);
        color += full_mod.color.xyz*full_mod.color.w;
        norm = normalize(full_mod.normal*full_mod.normal_factor + norm*(1 - full_mod.normal_factor));
    }

    if (point_light_count > 0)
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
