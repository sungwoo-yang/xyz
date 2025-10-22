/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \brief  Demo state for A* pathfinding and Olim movement
 */

#pragma once

#include "CS200/IRenderer2D.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Texture.hpp"

#include <GL/glew.h>
#include <string>
#include <vector>


class DemoAstar final : public CS230::GameState
{
public:
    DemoAstar() = default;

    void          Load() override;
    void          Update() override;
    void          Draw() const override;
    void          DrawImGui() override;
    void          Unload() override;
    gsl::czstring GetName() const override;

private:
    enum class OlimAnimation
    {
        None,
        OlimWalking,
        OlimJump
    };

    struct OlimFrame
    {
        int    frameIndex;
        double duration;
    };

    struct OlimAnimData
    {
        std::string            name;
        std::vector<OlimFrame> frames;
        int                    loopFrame;
    };

    struct OlimState
    {
        OlimAnimation animation;
        int           frameIndex;
        double        timer;
        Math::vec2    position;
        bool          faceRight;
    };

    struct RenderInfo
    {
        GLuint      Framebuffer{ 0 };
        GLuint      ColorTexture{ 0 };
        Math::ivec2 Size{};
    };

private:
    void initializeOlimAnimations();
    void updateOlimAnimation(OlimState& character, double delta_time);
    void drawOlim(const OlimState& character) const;

    RenderInfo beginOffscreenRendering() const;
    GLuint     endOffscreenRendering(const RenderInfo& render_info) const;

private:
    std::shared_ptr<CS230::Texture> olimTexture{};
    std::vector<OlimAnimData>       olimAnimations{};
    OlimState                       idleOlim{};

    GLuint lastFramebufferTexture{ 0 };
};