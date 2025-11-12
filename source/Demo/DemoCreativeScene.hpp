/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "CS200/BatchRenderer2D.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vec2.hpp"
#include <gsl/gsl>
#include <memory>
#include <vector>

class DemoCreativeScene : public CS230::GameState
{
public:
    void          Load() override;
    void          Update() override;
    void          Unload() override;
    void          Draw() const override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;

private:
    struct Star
    {
        Math::vec2  position;
        double      scroll_speed;
        CS200::RGBA tint;
    };

    CS200::BatchRenderer2D m_renderer;

    std::shared_ptr<CS230::Texture> m_tex_planets;
    std::shared_ptr<CS230::Texture> m_tex_foreground;
    std::shared_ptr<CS230::Texture> m_tex_white_pixel;

    std::vector<Star> m_stars;

    double         m_bg_offset_planets   = 0.0;
    mutable double m_fg_offset_platforms = 0.0;

    mutable Math::ivec2 m_screen_size;
};