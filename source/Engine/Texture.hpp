/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Matrix.hpp"
#include "OpenGL/Texture.hpp"
#include "Vec2.hpp"
#include <filesystem>

namespace CS230
{
    class Font;

    class Texture
    {
    public:
        friend class TextureManager;
        friend class Font;

        void Draw(const Math::TransformationMatrix& display_matrix, unsigned int color = 0xFFFFFFFF);
        void Draw(const Math::TransformationMatrix& display_matrix, Math::ivec2 texel_position, Math::ivec2 frame_size, unsigned int color = 0xFFFFFFFF);

        Math::ivec2 GetSize() const;
        ~Texture();

        [[nodiscard]] OpenGL::TextureHandle GetHandle() const
        {
            return textureHandle;
        }

    private:
        explicit Texture(const std::filesystem::path& file_name);
        Texture(OpenGL::TextureHandle given_texture, Math::ivec2 the_size);

    public:
        Texture(const Texture&)            = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& temporary) noexcept;
        Texture& operator=(Texture&& temporary) noexcept;

    private:
        OpenGL::TextureHandle textureHandle{};
        Math::ivec2           size{};
    };
}
