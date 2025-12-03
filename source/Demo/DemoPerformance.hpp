/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "CS200/IRenderer2D.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vec2.hpp"
#include <gsl/gsl>
#include <memory>
#include <vector>

enum class RendererType
{
    Immediate,
    Batch,
    Instanced
};

class DemoPerformance : public CS230::GameState
{
public:
    void          Load() override;
    void          Update() override;
    void          Unload() override;
    void          Draw() const override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;

private:
    struct Sprite
    {
        Math::vec2  position;
        Math::vec2  velocity;
        double      rotation = 0.0;
        CS200::RGBA tint;

        int         textureIndex;
        Math::vec2  uv_bl;
        Math::vec2  uv_tr;
        Math::ivec2 frame_size;
    };

    void InitializeSprite(Sprite& sprite);
    void ChangeSpriteCount(int new_count);
    void SwitchRenderer(RendererType type);

    std::unique_ptr<CS200::IRenderer2D> m_Renderer;
    RendererType                        m_CurrentRenderer = RendererType::Immediate;

    std::vector<std::shared_ptr<CS230::Texture>> m_SpriteTextures;
    std::vector<Sprite>                          m_Sprites;

    int  m_SpriteCount = 1;
    bool m_Animate     = true;

    mutable Math::ivec2 m_ScreenSize;
};