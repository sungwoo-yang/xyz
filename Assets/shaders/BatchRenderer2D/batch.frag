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

uniform sampler2D u_texture;

layout(location = 0) out vec4 frag_color;

void main()
{
    vec4 tex_color = texture(u_texture, v_uv);
    frag_color = tex_color * v_color;
    if (frag_color.a < 0.01)
    {
        discard;
    }
}