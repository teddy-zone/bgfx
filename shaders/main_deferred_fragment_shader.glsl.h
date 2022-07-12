
#include <string>

inline std::string main_deferred_fragment_shader = R"SHAD(
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
    vec4 location2;
    vec4 color;
    float radius;
    float angle;
    float t;
    int type;
    int object_id;
};

struct Fow
{
    vec4 location;
    float ranges[120];
};

layout (std430, binding=5) buffer point_lights_uni
{
    PointLight point_lights[];
};

layout (std430, binding=6) buffer decals_uni
{
    Decal decals[];
};

layout (std430, binding=7) buffer fows_uni
{
    Fow fows[];
};

uniform int point_light_count;
uniform int decal_count;
uniform int fow_count;
uniform vec3 camera_location;

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D color_tex;
uniform sampler2D object_id_tex;
uniform sampler2D fov_tex;

uniform int selected_object;
uniform int weather_state;
#define RAIN 2
#define SNOW 1

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
    vec3 spec;
    float normal_factor;
};

// Waves decal
void evaluate_decal_1(in Decal in_decal, in vec3 pos, inout vec3 color, inout vec3 normal, inout bool lit_flag)
{
    float wavelength = 1.0;
    float dist = length(in_decal.location.xy - pos.xy);  
    float wave_freq = 10.0/pow(dist + 1,2);
    vec3 radial = vec3(normalize(pos.xy - in_decal.location.xy), 0);  
    float wave_amplitude = 1/pow(dist + 1, 2);
    float val = wave_amplitude*sin(wave_freq*in_decal.t + dist*wavelength);
    float d_val = cos(wave_freq*in_decal.t + dist*wavelength);
    normal = normal + (1.0 - in_decal.t)*normalize(radial + vec3(0,0,1)/(d_val));
    return;
}

// Icy decal
void evaluate_decal_2(in Decal in_decal, in vec3 pos, inout vec3 color, inout vec3 normal, inout bool lit_flag)
{
    return;
}

// Circle targeting decal
void evaluate_decal_circle_targeting(in Decal in_decal, in vec3 pos, inout vec3 color, inout vec3 normal, inout bool lit_flag)
{
    float dist = length(in_decal.location.xy - pos.xy);  
    color = color + vec3(-0.1, -0.1, 0.5) + vec3(1,1,1)*1.0/(in_decal.radius - dist);
    return;
}

// Cone targeting decal
void evaluate_decal_cone_targeting(in Decal in_decal, in vec3 pos, inout vec3 color, inout vec3 normal, inout bool lit_flag)
{
    vec3 direction1 = normalize(in_decal.location - in_decal.location2).xyz;
    vec3 direction2 = normalize(pos - in_decal.location2.xyz);
    float dist = length(in_decal.location2.xy - pos.xy);  
    if (dot(direction1,direction2) < cos(3.14159/4.0))
    {
        return;
    }
    color = color + vec3(-0.1, -0.1, 0.5) + vec3(1,1,1)*1.0/(in_decal.radius - dist);
    return;
}

// Shadow decal 
void evaluate_decal_shadow(in Decal in_decal, in vec3 pos, inout vec3 color, inout vec3 normal, inout bool lit_flag)
{
    float dist = length(in_decal.location.xy - pos.xy);  
    color = color*vec3(0.7,0.7,0.75);// - vec3(0.1, 0.1, 0.05);// + vec3(1,1,1)*1.0/(in_decal.radius - dist);
    return;
}

// Footstep decal 
void evaluate_decal_footstep(in Decal in_decal, in vec3 pos, inout vec3 color, inout vec3 normal, inout bool lit_flag)
{
    float dist = length(in_decal.location.xy - pos.xy);  
    color = color*vec3(0.7,0.7,0.75);// - vec3(0.1, 0.1, 0.05);// + vec3(1,1,1)*1.0/(in_decal.radius - dist);
    return;
}

// Raindrop decal
void evaluate_raindrop_decal(in Decal in_decal, in vec3 pos, inout vec3 color, inout vec3 normal, inout bool lit_flag)
{
    float wavelength = 1.0;
    float dist = length(in_decal.location.xy - pos.xy);  
    float wave_freq = 10.0/pow(dist + 1,2);
    vec3 radial = vec3(normalize(pos.xy - in_decal.location.xy), 0);  
    float wave_amplitude = 1/pow(dist + 1, 2);
    float val = wave_amplitude*sin(wave_freq*in_decal.t + dist*wavelength);
    float d_val = cos(wave_freq*in_decal.t + dist*wavelength);
    normal = normal + 0.3*(1.0 - in_decal.t)*normalize(radial + vec3(0,0,1)/(d_val));
    return;
}

void evaluate_decal(in Decal in_decal, in vec3 pos, inout vec3 color, inout vec3 normal, inout bool lit_flag)
{
    if (in_decal.type == 1)
    {
        evaluate_decal_1(in_decal, pos, color, normal, lit_flag);
    }
    else if (in_decal.type == 2)
    {
        evaluate_decal_2(in_decal, pos, color, normal, lit_flag);
    }
    else if (in_decal.type == 3)
    {
        evaluate_decal_circle_targeting(in_decal, pos, color, normal, lit_flag);
    }
    else if (in_decal.type == 4)
    {
        evaluate_decal_cone_targeting(in_decal, pos, color, normal, lit_flag);
    }
    else if (in_decal.type == 5)
    {
        evaluate_decal_shadow(in_decal, pos, color, normal, lit_flag);
    }
    else if (in_decal.type == 6)
    {
        evaluate_decal_footstep(in_decal, pos, color, normal, lit_flag);
    }
    else if (in_decal.type == 7)
    {
        evaluate_raindrop_decal(in_decal, pos, color, normal, lit_flag);
    }
}

float process_discrete_shading(float in_light, 
                              int number_of_levels,
                              float level_slope,
                              float between_slope)
{
    int level = int(number_of_levels*in_light);
    in_light = float(level)/float(number_of_levels);
    return in_light;
}

void process_cel_shading(inout float in_light)
{
    float shift_start = 0.45;
    float shift_end = 0.55;
    float slope1 = 0.02;
    float slope2 = 10.0;
    float slope3 = 0.02;

    float slope2_y_intercept = slope1*shift_start - slope2*shift_start;
    float slope3_y_intercept = slope2*shift_end + slope2_y_intercept - slope3*shift_end;

    if (in_light < shift_start)
    {
        in_light = in_light*slope1;
    }
    else if (in_light > shift_end)
    {
        in_light = in_light*slope3 + slope3_y_intercept;
    }
    else
    {
        in_light = in_light*slope2 + slope2_y_intercept;
    }
}

void main()
{    
    vec3 norm = texture(normal_tex, uv).xyz;
    vec3 color = texture(color_tex, uv).xyz;
    float object_id = texture(object_id_tex, uv).x;
    vec3 post = texture(position_tex, uv).xyz*100.0;
    vec3 factor = vec3(0.5); 
    float intensity_factor = 1.0;
    
    if (weather_state == SNOW)
    {
        // Snow
        if (dot(norm, vec3(0,0,1)) > 0.8)
        {
            color = vec3(1,1,1);
        }
    }
    else if (weather_state == RAIN)
    {
        
    }

    MatMod full_mod;
    full_mod.normal_factor = 0;
    full_mod.normal = vec3(0);
    full_mod.color = vec4(0);
    float mod_count = 0;
    if (object_id < 0)
    {
        for (int i = 0; i < decal_count; ++i)
        {
            if ((decals[i].type != 4 && length(post.xy - decals[i].location.xy) < decals[i].radius) ||
                (decals[i].type == 4 && length(post.xy - decals[i].location2.xy) < decals[i].radius))
            {
                bool is_lit;
                evaluate_decal(decals[i], post, color, norm, is_lit);
                mod_count += 1;
            }
        }
    }
    
    float fow_factor = 1.0;
    
    float blur_range = 5.0;
    for (int i = 0; i < fow_count; ++i)
    {
        vec3 diff = post.xyz - fows[i].location.xyz;
        float angle = atan(diff.y, diff.x);
        if (angle < 0)
        {
            angle += 2*3.14159;
        }
        int sector = int(120.0*angle/(2*3.14159));
        float frac = 120.0*angle/(2*3.14159) - sector;    
        int sector2 = (sector + 1) % 120;
        float range = (fows[i].ranges[sector]*(1-frac) + fows[i].ranges[sector2]*frac);
        if (length(post.xy - fows[i].location.xy) > range) 
        {
            fow_factor = 0.75 + 0.25*max(0,blur_range - (length(post.xy - fows[i].location.xy) - range))/blur_range;
        }
    }
    
    if (mod_count > 0)
    {
        //full_mod.color = normalize(full_mod.color);
        //full_mod.normal = normalize(full_mod.normal);
        //color += full_mod.color.xyz*full_mod.color.w;
        //norm = normalize(full_mod.normal*full_mod.normal_factor + norm*(1 - full_mod.normal_factor));
    }

    float spec = 0;

    if (point_light_count > 0)
    {
        for (int i = 0; i < point_light_count; ++i)
        {
            vec3 r = point_lights[i].location.xyz - post;
            vec3 point_to_cam = normalize(camera_location - post);
            vec3 d = -r;
            vec3 reflection = d - 2*(norm*dot(d,norm));
            factor += point_lights[i].color.xyz*point_lights[i].intensity*10000*clamp(dot(norm, normalize(r)), 0,1)/(length(r)*length(r));
            spec += max(0,dot(point_to_cam, reflection))*max(0,dot(point_to_cam, reflection));
        }
    }

    if (weather_state == RAIN)
    {
        // Rain
        spec = spec*max(0,dot(norm, vec3(0,0,1)))*max(0,dot(norm, vec3(0,0,1)));
    }
    else
    {
        spec = 0;
    }

    process_discrete_shading(factor.x, 3, 1.0, 1.0);
    process_discrete_shading(factor.y, 3, 1.0, 1.0);
    process_discrete_shading(factor.z, 3, 1.0, 1.0);
    vec3 out_fac = normalize(factor)*process_discrete_shading(length(factor), 8, 1.0, 1.0);
    

    if (int(object_id) == selected_object)
    {
        //color *= vec3(1.0,0.5,2.0);
    }
    float gamma = 2.3;
    vec4 pre_color = vec4(out_fac*color,1);//evaluate_ssao(post, norm);

    if (object_id < 0)
    { 
        float avg = (pre_color.x + pre_color.y + pre_color.z)/3.0;
        pre_color.xyz = (pre_color.xyz*0.4 + vec3(avg,avg,avg)*0.1);
        pre_color.xyz = pre_color.xyz*vec3(0.9,0.9,1.0) + vec3(0.2);
    }
    if (weather_state == RAIN)
    {
        float fog_factor = length(camera_location - post)*0.005;
        diffuseColor = (fow_factor*pre_color*(1 - spec*0.00005) + vec4(1)*spec*0.00003)*(1-fog_factor) + vec4(0.8,0.9,1.0,0.0)*fog_factor;
    }
    else
    {
        diffuseColor = fow_factor*pre_color;
    }
}  
)SHAD";