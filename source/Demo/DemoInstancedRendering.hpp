/**
 * \file
 * \author Sungwoo Yang (based on CS200 Homework 6)
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "CS200/InstancedRenderer2D.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vec2.hpp"
#include <gsl/gsl>
#include <memory>
#include <vector>

class DemoInstancedRendering : public CS230::GameState
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
        Math::vec2 position;
        Math::vec2 velocity;
        double     rotation = 0.0;
        CS200::RGBA tint;
    };

    CS200::InstancedRenderer2D m_Renderer;

    std::shared_ptr<CS230::Texture> m_TextureCat;
    std::shared_ptr<CS230::Texture> m_TextureRobot;

    std::vector<Sprite> m_Sprites;

    int m_SpriteCount = 10000;
    bool m_Animate = true;

    mutable Math::ivec2 m_ScreenSize;
};