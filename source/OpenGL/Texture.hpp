/**
 * \file
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "CS200/RGBA.hpp"
#include "Engine/Vec2.hpp"
#include "GLConstants.hpp"
#include "GLTypes.hpp"
#include "Handle.hpp"
#include <filesystem>
#include <span>

namespace CS200
{
    class Image;
}

namespace OpenGL
{
    enum class Filtering : GLint
    {
        NearestPixel = GL_NEAREST,
        Linear       = GL_LINEAR
    };

    enum class Wrapping : GLint
    {
        Repeat            = GL_REPEAT,
        ClampToEdge       = GL_CLAMP_TO_EDGE,
        ClampToBorder     = GL_CLAMP_TO_BORDER,
        MirroredRepeat    = GL_MIRRORED_REPEAT,
        MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
    };

    using TextureHandle = Handle;

    [[nodiscard]] TextureHandle CreateTextureFromImage(const CS200::Image& image, Filtering filtering = Filtering::NearestPixel, Wrapping wrapping = Wrapping::Repeat) noexcept;
    [[nodiscard]] TextureHandle
        CreateTextureFromMemory(Math::ivec2 size, std::span<const CS200::RGBA> colors, Filtering filtering = Filtering::NearestPixel, Wrapping wrapping = Wrapping::Repeat) noexcept;
    [[nodiscard]] TextureHandle CreateRGBATexture(Math::ivec2 size, Filtering filtering = Filtering::NearestPixel, Wrapping wrapping = Wrapping::Repeat) noexcept;

    void SetFiltering(TextureHandle texture_handle, Filtering filtering) noexcept;

    enum TextureCoordinate
    {
        S,
        T,
        Both
    };

    void SetWrapping(TextureHandle texture_handle, Wrapping wrapping, TextureCoordinate coord = TextureCoordinate::Both) noexcept;
}
