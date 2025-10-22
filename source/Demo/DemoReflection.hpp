#pragma once

#include "CS200/RGBA.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Vec2.hpp"
#include <utility>
#include <vector>

#ifndef M_PI
constexpr double PI = 3.14159265358979323846;
#else
constexpr double PI = M_PI;
#endif

constexpr CS200::RGBA COLOR_RED    = 0xFF0000FF;
constexpr CS200::RGBA COLOR_YELLOW = 0xFFFF00FF;
constexpr CS200::RGBA COLOR_GREEN  = 0x00FF00FF;
constexpr CS200::RGBA COLOR_CYAN   = 0x00FFFFFF;
constexpr CS200::RGBA COLOR_WHITE  = 0xFFFFFFFF;
constexpr CS200::RGBA COLOR_BLACK  = 0x000000FF;

class DemoLaserReflection : public CS230::GameState
{
public:
    void          Load() override;
    void          Update() override;
    void          Unload() override;
    void          Draw() const override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;

private:
    Math::vec2 characterPos{ 400.0, 300.0 };
    double     shieldAngle  = PI / 2.0;
    double     shieldLength = 100.0;
    Math::vec2 shieldStart;
    Math::vec2 shieldEnd;

    Math::vec2                                     laserOrigin;
    std::vector<std::pair<Math::vec2, Math::vec2>> laserPath;

    double       laserTimer      = 0.0;
    bool         isLaserOn       = false;
    CS200::RGBA  laserColor      = COLOR_RED;
    const double laserCycleTime  = 5.0;
    const double laserOnDuration = 2.5;

    bool         isParrying          = false;
    bool         parryWindowActive   = false;
    double       parryTimer          = 0.0;
    const double parryWindowDuration = 0.5;

    Math::vec2  targetPos;
    CS200::RGBA targetColor             = COLOR_RED;
    double      targetRadius            = 15.0;
    bool        targetHitByParriedLaser = false;

    void UpdateShield();
    void CalculateLaser();
    bool CheckCollision() const;
};
