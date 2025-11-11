#version 300 es
precision mediump float;
precision mediump sampler2D;

/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */


in vec2 v_uv;
in vec4 v_color;
flat in float v_tex_id;

uniform sampler2D u_textures[16];

layout(location = 0) out vec4 frag_color;

void main()
{
    int tex_index = int(v_tex_id + 0.5);
    vec4 tex_color;

    switch(tex_index)
    {
        case 1: tex_color = texture(u_textures[1], v_uv); break;
        case 2: tex_color = texture(u_textures[2], v_uv); break;
        case 3: tex_color = texture(u_textures[3], v_uv); break;
        case 4: tex_color = texture(u_textures[4], v_uv); break;
        case 5: tex_color = texture(u_textures[5], v_uv); break;
        case 6: tex_color = texture(u_textures[6], v_uv); break;
        case 7: tex_color = texture(u_textures[7], v_uv); break;
        case 8: tex_color = texture(u_textures[8], v_uv); break;
        case 9: tex_color = texture(u_textures[9], v_uv); break;
        case 10: tex_color = texture(u_textures[10], v_uv); break;
        case 11: tex_color = texture(u_textures[11], v_uv); break;
        case 12: tex_color = texture(u_textures[12], v_uv); break;
        case 13: tex_color = texture(u_textures[13], v_uv); break;
        case 14: tex_color = texture(u_textures[14], v_uv); break;
        case 15: tex_color = texture(u_textures[15], v_uv); break;
        default: tex_color = vec4(1.0, 0.0, 1.0, 1.0);
    }
    
    frag_color = tex_color * v_color;

    if (frag_color.a < 0.01)
    {
        discard;
    }
}