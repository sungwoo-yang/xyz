#version 300 es

/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

layout (location = 0) in vec2 a_vertex_pos;
layout (location = 1) in vec2 a_vertex_uv;
layout (location = 2) in vec3 a_model_col0;
layout (location = 3) in vec3 a_model_col1;
layout (location = 4) in vec3 a_model_col2;
layout (location = 5) in vec3 a_uv_col0;
layout (location = 6) in vec3 a_uv_col1;
layout (location = 7) in vec3 a_uv_col2;
layout (location = 8) in vec4 a_color;
layout (location = 9) in float a_tex_id;

uniform mat3 u_ndc_matrix;

out vec2 v_uv;
out vec4 v_color;
flat out float v_tex_id;

void main()
{
    mat3 model_matrix = mat3(a_model_col0, a_model_col1, a_model_col2);
    mat3 uv_matrix = mat3(a_uv_col0, a_uv_col1, a_uv_col2);

    v_color = a_color;
    v_tex_id = a_tex_id;

    v_uv = (uv_matrix * vec3(a_vertex_uv, 1.0)).xy;

    vec3 ndc_pos = u_ndc_matrix * model_matrix * vec3(a_vertex_pos, 1.0); 
    gl_Position = vec4(ndc_pos.xy, 0.0, 1.0);
}
