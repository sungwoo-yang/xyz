/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "InstancedRenderer2D.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Texture.hpp"
#include "OpenGL/Buffer.hpp"
#include "OpenGL/GL.hpp"
#include "Renderer2DUtils.hpp"
#include <utility>

namespace CS200
{
    struct UnitVertex
    {
        float x, y;
        float u, v;
    };

    constexpr std::array<UnitVertex, 4> g_UnitQuadVertices = {
        UnitVertex{ -0.5f, -0.5f, 0.0f, 0.0f },
        UnitVertex{  0.5f, -0.5f, 1.0f, 0.0f },
        UnitVertex{  0.5f,  0.5f, 1.0f, 1.0f },
        UnitVertex{ -0.5f,  0.5f, 0.0f, 1.0f }
    };

    constexpr std::array<uint32_t, 6> g_QuadIndices = { 0, 1, 2, 0, 2, 3 };

    InstancedRenderer2D::InstancedRenderer2D()
    {
        m_InstanceData.reserve(MAX_INSTANCES_PER_BATCH);
    }

    InstancedRenderer2D::~InstancedRenderer2D()
    {
        if (m_VAO != 0)
        {
            Shutdown();
        }
    }

    void InstancedRenderer2D::Init()
    {
        m_Shader = OpenGL::CreateShader(std::filesystem::path("Assets/shaders/InstancedRenderer2D/instanced.vert"), std::filesystem::path("Assets/shaders/InstancedRenderer2D/instanced.frag"));

        m_UnitQuadVBO = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, std::as_bytes(std::span{ g_UnitQuadVertices }));

        m_InstanceVBO = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, MAX_INSTANCES_PER_BATCH * sizeof(InstanceData));

        m_EBO = OpenGL::CreateBuffer(OpenGL::BufferType::Indices, std::as_bytes(std::span{ g_QuadIndices }));

        OpenGL::VertexBuffer unitQuadLayout{
            m_UnitQuadVBO, { OpenGL::Attribute::Float2, OpenGL::Attribute::Float2 }
        };

        auto inst_float3 = OpenGL::Attribute::Float3;
        auto inst_float4 = OpenGL::Attribute::Float4;
        auto inst_float  = OpenGL::Attribute::Float;

        OpenGL::VertexBuffer instanceLayout{
            m_InstanceVBO,
            { inst_float3.WithDivisor(1), inst_float3.WithDivisor(1), inst_float3.WithDivisor(1), inst_float3.WithDivisor(1), inst_float3.WithDivisor(1), inst_float3.WithDivisor(1),
              inst_float4.WithDivisor(1), inst_float.WithDivisor(1) }
        };

        m_VAO = OpenGL::CreateVertexArrayObject({ unitQuadLayout, instanceLayout }, m_EBO);

        std::array<int, MAX_TEXTURE_SLOTS> samplers{};
        for (uint32_t i = 0; i < MAX_TEXTURE_SLOTS; ++i)
        {
            samplers[i] = static_cast<int>(i);
        }

        GL::UseProgram(m_Shader.Shader);
        GL::Uniform1iv(m_Shader.UniformLocations.at("u_textures[0]"), MAX_TEXTURE_SLOTS, samplers.data());
        GL::UseProgram(0);
    }

    void InstancedRenderer2D::Shutdown()
    {
        GL::DeleteBuffers(1, &m_UnitQuadVBO);
        GL::DeleteBuffers(1, &m_InstanceVBO);
        GL::DeleteBuffers(1, &m_EBO);
        GL::DeleteVertexArrays(1, &m_VAO);
        OpenGL::DestroyShader(m_Shader);

        m_UnitQuadVBO   = 0;
        m_InstanceVBO   = 0;
        m_EBO           = 0;
        m_VAO           = 0;
        m_Shader.Shader = 0;
    }

    void InstancedRenderer2D::BeginScene(const Math::TransformationMatrix& view_projection)
    {
        m_ViewProjection = view_projection;
        StartBatch();
    }

    void InstancedRenderer2D::EndScene()
    {
        Flush();
    }

    void InstancedRenderer2D::Flush()
    {
        if (m_InstanceCount == 0)
        {
            return;
        }

        GL::UseProgram(m_Shader.Shader);
        GL::BindVertexArray(m_VAO);

        OpenGL::UpdateBufferData(OpenGL::BufferType::Vertices, m_InstanceVBO, std::as_bytes(std::span{ m_InstanceData }));

        for (uint32_t i = 0; i < m_TextureSlotIndex; ++i)
        {
            GL::ActiveTexture(GL_TEXTURE0 + i);
            GL::BindTexture(GL_TEXTURE_2D, m_TextureSlots[i]);
        }

        const auto gl_vp_matrix = Renderer2DUtils::to_opengl_mat3(m_ViewProjection);
        GL::UniformMatrix3fv(m_Shader.UniformLocations.at("u_ndc_matrix"), 1, GL_FALSE, gl_vp_matrix.data());

        GL::DrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(m_InstanceCount));

        GL::BindVertexArray(0);
        GL::UseProgram(0);
    }

    void InstancedRenderer2D::StartBatch()
    {
        m_InstanceData.clear();
        m_InstanceCount    = 0;
        m_TextureSlotIndex = 1;
    }

    float InstancedRenderer2D::GetTextureSlot(OpenGL::TextureHandle texture)
    {
        for (uint32_t i = 1; i < m_TextureSlotIndex; ++i)
        {
            if (m_TextureSlots[i] == texture)
            {
                return static_cast<float>(i);
            }
        }

        if (m_TextureSlotIndex >= MAX_TEXTURE_SLOTS)
        {
            Flush();
            StartBatch();
        }

        m_TextureSlots[m_TextureSlotIndex] = texture;
        return static_cast<float>(m_TextureSlotIndex++);
    }

    void InstancedRenderer2D::DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor)
    {
        if (m_InstanceCount >= MAX_INSTANCES_PER_BATCH)
        {
            Flush();
            StartBatch();
        }

        InstanceData data;

        data.ModelMatrix = Renderer2DUtils::to_opengl_mat3(transform);

        Math::TransformationMatrix uv_transform;
        uv_transform[0][0] = texture_coord_tr.x - texture_coord_bl.x;
        uv_transform[1][1] = texture_coord_tr.y - texture_coord_bl.y;
        uv_transform[0][2] = texture_coord_bl.x;
        uv_transform[1][2] = texture_coord_bl.y;
        data.UVMatrix      = Renderer2DUtils::to_opengl_mat3(uv_transform);

        data.TintColor = CS200::unpack_color(tintColor);
        data.TexID     = GetTextureSlot(texture);

        m_InstanceData.push_back(data);
        m_InstanceCount++;
    }

    void InstancedRenderer2D::DrawCircle(const Math::TransformationMatrix&, CS200::RGBA, CS200::RGBA, double)
    {
    }

    void InstancedRenderer2D::DrawRectangle(const Math::TransformationMatrix&, CS200::RGBA, CS200::RGBA, double)
    {
    }

    void InstancedRenderer2D::DrawLine(const Math::TransformationMatrix&, Math::vec2, Math::vec2, CS200::RGBA, double)
    {
    }

    void InstancedRenderer2D::DrawLine(Math::vec2, Math::vec2, CS200::RGBA, double)
    {
    }
}