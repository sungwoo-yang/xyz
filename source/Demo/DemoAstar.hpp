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
#include "Engine/Dash.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Rect.hpp"
#include "Engine/Texture.hpp"

#include <GL/glew.h>
#include <limits>
#include <optional>
#include <string>
#include <vector>

struct Platform
{
    Math::vec2 start{};
    Math::vec2 end{};
    double     thickness   = 20.0;
    bool       canDropDown = false; // 내려갈 수 있는지 여부
};

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
        OlimAnimation         animation;
        int                   frameIndex;
        double                timer;
        Math::vec2            position;
        bool                  faceRight;
        double                velocityY            = 0.0;
        bool                  isJumping            = false;
        std::optional<size_t> currentPlatformIndex = std::nullopt;
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
    // 추가: 물리 및 플랫폼 관련 상수/변수
    double gravity               = 1500.0; // 중력 가속도 (pixels/sec^2)
    double jumpStrength          = 700.0;  // 점프 시 초기 Y축 속도 (pixels/sec)
    double groundLevel           = 100.0;  // 바닥 플랫폼 상단 Y 좌표
    double platformThickness     = 20.0;   // 바닥 플랫폼 두께
    double thinPlatformThickness = 5.0;


    std::vector<Platform> platforms; // 모든 플랫폼을 저장할 벡터

    // 추가: DashComponent 멤버
    CS230::DashComponent olimDash;

    double      dashGaugeWidth   = 40.0;       // 게이지 바 전체 너비
    double      dashGaugeHeight  = 5.0;        // 게이지 바 높이
    double      dashGaugeOffsetY = 10.0;       // 캐릭터 발 아래로부터의 Y 오프셋
    CS200::RGBA dashGaugeBgColor = 0x55555580; // 게이지 바 배경색 (어두운 회색, 반투명)
    CS200::RGBA dashGaugeFgColor = 0x00BCD4FF;
};