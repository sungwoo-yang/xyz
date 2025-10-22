#pragma once

#include "Engine/GameState.hpp"
#include "Engine/Vec2.hpp"
#include <utility> // For std::pair
#include <vector>

#ifndef M_PI
constexpr double PI = 3.14159265358979323846;
#else
constexpr double PI = M_PI;
#endif

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
    double     shieldAngle  = PI;
    double     shieldLength = 100.0;
    Math::vec2 shieldStart;
    Math::vec2 shieldEnd;

    Math::vec2                                     laserOrigin{ 10.0, 10.0 };
    std::vector<std::pair<Math::vec2, Math::vec2>> laserPath;

    void UpdateShield();
    void CalculateLaser();
};