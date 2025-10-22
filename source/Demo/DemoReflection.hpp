#pragma once

#include "CS200/RGBA.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Vec2.hpp"
#include <array>
#include <utility>
#include <vector>

#ifndef M_PI
constexpr double PI = 3.14159265358979323846;
#else
constexpr double PI = M_PI;
#endif

// --- 색상 정의 ---
constexpr CS200::RGBA COLOR_RED           = 0xFF0000FF;
constexpr CS200::RGBA COLOR_YELLOW        = 0xFFFF00FF;
constexpr CS200::RGBA COLOR_GREEN         = 0x00FF00FF;
constexpr CS200::RGBA COLOR_CYAN          = 0x00FFFFFF;
constexpr CS200::RGBA COLOR_WHITE         = 0xFFFFFFFF;
constexpr CS200::RGBA COLOR_BLACK         = 0x000000FF;
constexpr CS200::RGBA COLOR_WARNING       = 0xFF000080;
constexpr CS200::RGBA COLOR_PARRY_WARNING = 0x00FFFF80;

struct TargetState
{
    Math::vec2  position;
    CS200::RGBA color             = COLOR_RED;
    double      radius            = 25.0;
    bool        hitByParriedLaser = false;
};

class DemoLaserReflection : public CS230::GameState
{
public:
    // ... (함수 선언 동일) ...
    void          Load() override;
    void          Update() override;
    void          Unload() override;
    void          Draw() const override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;


private:
    Math::vec2           characterPos{ 400.0, 300.0 };
    double               shieldAngle  = PI / 2.0;
    double               shieldLength = 100.0;
    Math::vec2           shieldStart;
    Math::vec2           shieldEnd;
    // --- 쉴드 색상 관련 변수 ---
    CS200::RGBA          shieldColor = COLOR_CYAN;
    std::array<float, 4> currentShieldColor{};
    std::array<float, 4> targetShieldColor{};
    double               shieldHitTimer          = 0.0;
    const double         shieldColorRecoveryTime = 1.0;
    // --- 쉴드 고정 상태 변수 ---
    bool                 isShieldFrozen          = false;
    double               shieldFrozenTimer       = 0.0;
    const double         shieldFreezeDuration    = 3.0;

    Math::vec2                                     laserOrigin;
    std::vector<std::pair<Math::vec2, Math::vec2>> laserPath;
    std::vector<std::pair<Math::vec2, Math::vec2>> warningLaserPath;

    // ... (레이저, 패링, 타겟 변수 동일) ...
    double laserTimer          = 0.0;
    bool   isLaserOn           = false;
    bool   showingWarningLaser = false;

    CS200::RGBA  warningLaserColor          = COLOR_WARNING;
    const double warningLaserLeadTime       = 5.0;
    const double parryWindowStartTimeOffset = warningLaserLeadTime - parryWindowDuration;

    const double mainLaserWidth    = 5.0;
    const double warningLaserWidth = 1.0;
    CS200::RGBA  laserColor        = COLOR_RED;
    const double laserCycleTime    = 10.0;
    const double laserOnDuration   = 2.5;

    bool         isParrying          = false;
    bool         parryWindowActive   = false;
    double       parryTimer          = 0.0;
    const double parryWindowDuration = 0.5;

    std::vector<TargetState> targets;


    void UpdateShield();
    void CalculateLaser(bool parrySuccess);
    bool CheckCollision(const TargetState& target) const;
    bool CheckShieldCollision() const;
    void UpdateShieldColor(double dt);
};