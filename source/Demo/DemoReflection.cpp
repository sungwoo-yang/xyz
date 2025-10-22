#include "DemoReflection.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Physics/Reflection.hpp"
#include "Engine/Window.hpp"

#include <algorithm>
#include <cmath>
#include <imgui.h>
#include <limits>
#include <numbers>

namespace
{
    inline double dot(const Math::vec2& a, const Math::vec2& b)
    {
        return a.x * b.x + a.y * b.y;
    }

    inline Math::vec2 perpendicular(const Math::vec2& v)
    {
        return Math::vec2{ -v.y, v.x };
    }

    bool LineCircleIntersection(Math::vec2 p1, Math::vec2 p2, Math::vec2 center, double radius, Math::vec2& intersection)
    {
        Math::vec2 d = p2 - p1;
        Math::vec2 f = p1 - center;
        double     a = dot(d, d);
        double     b = 2.0 * dot(f, d);
        double     c = dot(f, f) - radius * radius;

        double discriminant = b * b - 4.0 * a * c;
        if (discriminant < 0)
        {
            return false;
        }
        else
        {
            discriminant = std::sqrt(discriminant);
            double t1    = (-b - discriminant) / (2.0 * a);
            double t2    = (-b + discriminant) / (2.0 * a);

            bool intersected = false;
            if (t1 >= 0 && t1 <= 1)
            {
                intersection = p1 + d * t1;
                intersected  = true;
            }
            if (t2 >= 0 && t2 <= 1)
            {
                if (!intersected)
                    intersection = p1 + d * t2;
                intersected = true;
            }
            return intersected;
        }
    }

}

void DemoLaserReflection::Load()
{
    CS200::RenderingAPI::SetClearColor(COLOR_BLACK);
    const auto windowSize   = Engine::GetWindow().GetSize();
    characterPos            = { windowSize.x / 2.0, windowSize.y / 2.0 };
    laserOrigin             = { 0.0, static_cast<double>(windowSize.y) };
    targetPos               = { windowSize.x - 50.0, windowSize.y - 50.0 };
    shieldAngle             = PI / 2.0;
    isLaserOn               = false;
    laserTimer              = 0.0;
    laserColor              = COLOR_RED;
    isParrying              = false;
    parryWindowActive       = false;
    parryTimer              = 0.0;
    targetColor             = COLOR_RED;
    targetHitByParriedLaser = false;

    UpdateShield();
}

void DemoLaserReflection::Unload()
{
}

gsl::czstring DemoLaserReflection::GetName() const
{
    return "Demo: Laser Reflection Parry";
}

void DemoLaserReflection::Update()
{
    const auto&  input = Engine::GetInput();
    const double dt    = Engine::GetWindowEnvironment().DeltaTime;

    laserTimer += dt;
    double cycleTime = fmod(laserTimer, laserCycleTime);

    bool prevLaserState = isLaserOn;
    isLaserOn           = (cycleTime < laserOnDuration);

    parryWindowActive = !isLaserOn && (cycleTime >= laserCycleTime - parryWindowDuration);

    if (parryWindowActive)
    {
        parryTimer += dt;
        if (input.KeyJustPressed(CS230::Input::Keys::Space))
        {
            isParrying = true;
            Engine::GetLogger().LogEvent("Parry Success!");
        }
    }
    else
    {
        parryTimer = 0.0;
    }

    if (isLaserOn)
    {
        if (prevLaserState == false)
        {
            if (isParrying)
            {
                laserColor = COLOR_YELLOW;
                Engine::GetLogger().LogEvent("Laser turned YELLOW");
            }
            else
            {
                laserColor = COLOR_RED;
                Engine::GetLogger().LogEvent("Laser turned RED");
            }
        }
    }
    else
    {
        if (prevLaserState == true)
        {
            laserColor = COLOR_RED;
            isParrying = false;
            Engine::GetLogger().LogEvent("Laser turned OFF, reset color to RED");
        }
    }

    const double moveSpeed = 200.0;
    Math::vec2   moveDir{ 0.0, 0.0 };

    if (input.KeyDown(CS230::Input::Keys::W))
        moveDir.y += 1.0;
    if (input.KeyDown(CS230::Input::Keys::S))
        moveDir.y -= 1.0;
    if (input.KeyDown(CS230::Input::Keys::A))
        moveDir.x -= 1.0;
    if (input.KeyDown(CS230::Input::Keys::D))
        moveDir.x += 1.0;

    double moveLen = moveDir.Length();
    if (moveLen > std::numeric_limits<double>::epsilon())
    {
        Math::vec2 nextPos    = characterPos + moveDir.Normalize() * moveSpeed * dt;
        const auto windowSize = Engine::GetWindow().GetSize();
        nextPos.x             = std::clamp(nextPos.x, 0.0, static_cast<double>(windowSize.x));
        nextPos.y             = std::clamp(nextPos.y, 0.0, static_cast<double>(windowSize.y));
        characterPos          = nextPos;
    }

    const double rotateSpeed = 2.0 * PI / 2.0;

    if (input.KeyDown(CS230::Input::Keys::Left))
    {
        shieldAngle += rotateSpeed * dt;
    }

    if (input.KeyDown(CS230::Input::Keys::Right))
    {
        shieldAngle -= rotateSpeed * dt;
    }

    shieldAngle = fmod(shieldAngle, 2.0 * PI);
    if (shieldAngle < 0)
        shieldAngle += 2.0 * PI;

    UpdateShield();
    if (isLaserOn)
    {
        CalculateLaser();
        if (laserColor == COLOR_YELLOW && !targetHitByParriedLaser)
        {
            if (CheckCollision())
            {
                targetColor             = COLOR_GREEN;
                targetHitByParriedLaser = true;
                Engine::GetLogger().LogEvent("Target hit by parried laser!");
            }
        }
    }
    else
    {
        laserPath.clear();
    }
}

void DemoLaserReflection::UpdateShield()
{
    double dx = (shieldLength / 2.0) * std::cos(shieldAngle);
    double dy = (shieldLength / 2.0) * std::sin(shieldAngle);

    shieldStart = characterPos + Math::vec2{ dx, dy };
    shieldEnd   = characterPos - Math::vec2{ dx, dy };
}

void DemoLaserReflection::CalculateLaser()
{
    std::vector<std::pair<Math::vec2, Math::vec2>> reflectionSegments;
    reflectionSegments.push_back({ shieldStart, shieldEnd });

    const auto windowSize = Engine::GetWindow().GetSize();
    Math::vec2 center     = { windowSize.x / 2.0, windowSize.y / 2.0 };
    Math::vec2 initialDir = (center - laserOrigin).Normalize();

    if (initialDir.Length() < std::numeric_limits<double>::epsilon())
    {
        initialDir = { 1.0, -1.0 };
        initialDir = initialDir.Normalize();
    }

    laserPath = Physics::CalculateLaserPath(laserOrigin, initialDir, reflectionSegments, 5);
}

bool DemoLaserReflection::CheckCollision() const
{
    Math::vec2 intersectionPoint;
    for (const auto& segment : laserPath)
    {
        if (LineCircleIntersection(segment.first, segment.second, targetPos, targetRadius, intersectionPoint))
        {
            return true;
        }
    }
    return false;
}

void DemoLaserReflection::Draw() const
{
    CS200::RenderingAPI::Clear();
    auto&      renderer   = Engine::GetRenderer2D();
    const auto windowSize = Engine::GetWindow().GetSize();

    renderer.BeginScene(CS200::build_ndc_matrix(windowSize));
    renderer.DrawCircle(Math::TranslationMatrix(characterPos) * Math::ScaleMatrix(10.0), COLOR_WHITE);
    renderer.DrawLine(shieldStart, shieldEnd, COLOR_CYAN, 3.0);
    renderer.DrawCircle(Math::TranslationMatrix(targetPos) * Math::ScaleMatrix(targetRadius), targetColor);

    if (isLaserOn)
    {
        for (const auto& segment : laserPath)
        {
            if (std::isfinite(segment.first.x) && std::isfinite(segment.first.y) && std::isfinite(segment.second.x) && std::isfinite(segment.second.y))
            {
                renderer.DrawLine(segment.first, segment.second, laserColor, 2.0);
            }
        }
    }

    renderer.EndScene();
}

void DemoLaserReflection::DrawImGui()
{
    ImGui::Begin("Laser Reflection Info");
    ImGui::Text("Character Position: (%.1f, %.1f)", characterPos.x, characterPos.y);
    ImGui::Text("Shield Angle (Degrees): %.1f", shieldAngle * 180.0 / PI);
    ImGui::Text("Shield Start: (%.1f, %.1f)", shieldStart.x, shieldStart.y);
    ImGui::Text("Shield End: (%.1f, %.1f)", shieldEnd.x, shieldEnd.y);
    ImGui::Separator();
    ImGui::Text("Laser State: %s", isLaserOn ? "ON" : "OFF");
    ImGui::Text("Laser Timer: %.2f / %.2f", fmod(laserTimer, laserCycleTime), laserCycleTime);
    const char* colorName = (laserColor == COLOR_RED) ? "RED" : (laserColor == COLOR_YELLOW ? "YELLOW" : "UNKNOWN");
    ImGui::Text("Laser Color: %s", colorName);
    ImGui::Text("Parry Window: %s", parryWindowActive ? "ACTIVE" : "INACTIVE");
    ImGui::Text("Parrying: %s", isParrying ? "YES" : "NO");
    ImGui::Separator();
    const char* targetColorName = (targetColor == COLOR_RED) ? "RED" : (targetColor == COLOR_GREEN ? "GREEN" : "UNKNOWN");
    ImGui::Text("Target Position: (%.1f, %.1f)", targetPos.x, targetPos.y);
    ImGui::Text("Target Color: %s", targetColorName);
    ImGui::Text("Target Hit by Parried Laser: %s", targetHitByParriedLaser ? "YES" : "NO");

    if (ImGui::Button("Reset Target"))
    {
        targetColor             = COLOR_RED;
        targetHitByParriedLaser = false;
        Engine::GetLogger().LogEvent("Target Reset!");
    }

    ImGui::Separator();
    ImGui::Text("Laser Path Segments: %zu", laserPath.size());
    if (ImGui::TreeNode("Path Details"))
    {
        for (size_t i = 0; i < laserPath.size(); ++i)
        {
            ImGui::Text("Seg %zu: (%.1f, %.1f) -> (%.1f, %.1f)", i, laserPath[i].first.x, laserPath[i].first.y, laserPath[i].second.x, laserPath[i].second.y);
        }
        ImGui::TreePop();
    }
    ImGui::End();
}
