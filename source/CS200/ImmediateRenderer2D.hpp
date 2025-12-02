/**
 * \file
 * \author Rudy Castan
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "Engine/Matrix.hpp"
#include "IRenderer2D.hpp"
#include "OpenGL/GLConstants.hpp"
#include "OpenGL/Shader.hpp"
#include "OpenGL/VertexArray.hpp"
#include <array>

namespace CS200
{
    enum class PrimitiveType
    {
        Points        = GL_POINTS,
        Lines         = GL_LINES,
        LineLoop      = GL_LINE_LOOP,
        LineStrip     = GL_LINE_STRIP,
        Triangles     = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleFan   = GL_TRIANGLE_FAN
    };

    class ImmediateRenderer2D : public IRenderer2D
    {
    public:
        ImmediateRenderer2D()                                 = default;
        ImmediateRenderer2D(const ImmediateRenderer2D& other) = delete;
        ImmediateRenderer2D(ImmediateRenderer2D&& other) noexcept;
        ImmediateRenderer2D& operator=(const ImmediateRenderer2D& other) = delete;
        ImmediateRenderer2D& operator=(ImmediateRenderer2D&& other) noexcept;
        ~ImmediateRenderer2D() override;

        void Init() override;
        void Shutdown() override;
        void BeginScene(const Math::TransformationMatrix& view_projection) override;
        void EndScene() override;

        void DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor) override;
        void DrawCircle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width) override;
        void DrawRectangle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width) override;
        void DrawLine(const Math::TransformationMatrix& transform, Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width) override;
        void DrawLine(Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width) override;
        void DrawVertices(PrimitiveType mode, const std::vector<Math::vec2>& points, const Math::TransformationMatrix& transform, CS200::RGBA color);

        void DrawVertices(PrimitiveType mode, const std::vector<Math::vec2>& points, CS200::RGBA color)
        {
            DrawVertices(mode, points, Math::TransformationMatrix{}, color);
        }

    private:
        enum class SDFShape : uint8_t
        {
            Circle    = 0,
            Rectangle = 1,
        };

        void DrawSDF(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width, SDFShape sdf_shape);

    private:
        struct Quad
        {
            OpenGL::BufferHandle      vertexBuffer{ 0 };
            OpenGL::BufferHandle      indexBuffer{ 0 };
            OpenGL::VertexArrayHandle vertexArray{ 0 };
        } quad;

        OpenGL::CompiledShader     quadShader{};
        Quad                       sdfQuad;
        OpenGL::CompiledShader     sdfShader{};
        Math::TransformationMatrix view_projection;

        struct PrimitiveBatch
        {
            OpenGL::BufferHandle      vertexBuffer{ 0 };
            OpenGL::VertexArrayHandle vertexArray{ 0 };
            OpenGL::TextureHandle     whiteTexture{ 0 };
        } primitives;

        static constexpr size_t MAX_VERTICES = 10000;
    };
}
