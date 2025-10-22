#include "DemoReflection.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RGBA.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Input.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Physics/Reflection.hpp"
#include "Engine/Window.hpp"

#include <algorithm>
#include <cmath>
#include <imgui.h>
#include <limits>
#include <numbers>

constexpr CS200::RGBA CYAN = 0x00FFFFFF;
constexpr CS200::RGBA RED  = 0xFF0000FF;

void DemoLaserReflection::Load()
{
    CS200::RenderingAPI::SetClearColor(CS200::BLACK);
    characterPos = { Engine::GetWindow().GetSize().x / 2.0, Engine::GetWindow().GetSize().y / 2.0 };
    shieldAngle  = PI;
    UpdateShield();
    CalculateLaser();
}

void DemoLaserReflection::Unload()
{
}

gsl::czstring DemoLaserReflection::GetName() const
{
    return "Demo: Laser Reflection";
}

void DemoLaserReflection::Update()
{
    const auto&  input = Engine::GetInput();
    const double dt    = Engine::GetWindowEnvironment().DeltaTime;

    const double moveSpeed = 200.0;
    Math::vec2   moveDir{ 0.0, 0.0 };
    if (input.KeyDown(CS230::Input::Keys::W))
        moveDir.y -= 1.0;
    if (input.KeyDown(CS230::Input::Keys::S))
        moveDir.y += 1.0;
    if (input.KeyDown(CS230::Input::Keys::A))
        moveDir.x -= 1.0;
    if (input.KeyDown(CS230::Input::Keys::D))
        moveDir.x += 1.0;
    double moveLen = moveDir.Length();
    if (moveLen > std::numeric_limits<double>::epsilon())
    {
        characterPos += moveDir.Normalize() * moveSpeed * dt;
    }

    const double rotateSpeed = 2.0;

    if (input.KeyDown(CS230::Input::Keys::Left))
    {
        shieldAngle += rotateSpeed * dt;
    }

    if (input.KeyDown(CS230::Input::Keys::Right))
    {
        shieldAngle -= rotateSpeed * dt;
    }

    shieldAngle = std::clamp(shieldAngle, 0.0, PI);

    UpdateShield();
    CalculateLaser();
}

void DemoLaserReflection::UpdateShield()
{
    double halfLength = shieldLength / 2.0;
    double dx         = halfLength * std::cos(shieldAngle);
    double dy         = halfLength * std::sin(shieldAngle);

    shieldStart = characterPos + Math::vec2{ dx, dy };
    shieldEnd   = characterPos - Math::vec2{ dx, dy };
}

void DemoLaserReflection::CalculateLaser()
{
    std::vector<std::pair<Math::vec2, Math::vec2>> reflectionSegments;
    reflectionSegments.push_back({ shieldStart, shieldEnd });

    Math::vec2 initialDir = (characterPos - laserOrigin).Normalize();

    if (initialDir.Length() < std::numeric_limits<double>::epsilon())
    {
        initialDir = { 1.0, 0.0 };
    }

    laserPath = Physics::CalculateLaserPath(laserOrigin, initialDir, reflectionSegments, 5);
}

void DemoLaserReflection::Draw() const
{
    CS200::RenderingAPI::Clear();
    auto&      renderer   = Engine::GetRenderer2D();
    const auto windowSize = Engine::GetWindow().GetSize();

    const Math::vec2              scale_factors = { 2.0 / static_cast<double>(windowSize.x), -2.0 / static_cast<double>(windowSize.y) };
    const Math::ScaleMatrix       scale_matrix(scale_factors);
    const Math::vec2              tv = { -1.0, 1.0 };
    const Math::TranslationMatrix translate_matrix(tv);
    Math::TransformationMatrix    viewProjection = translate_matrix * scale_matrix;

    renderer.BeginScene(viewProjection);

    renderer.DrawCircle(Math::TranslationMatrix(characterPos) * Math::ScaleMatrix(10.0), CS200::WHITE);
    renderer.DrawLine(shieldStart, shieldEnd, CYAN, 3.0);
    for (const auto& segment : laserPath)
    {
        if (std::isfinite(segment.first.x) && std::isfinite(segment.first.y) && std::isfinite(segment.second.x) && std::isfinite(segment.second.y))
        {
            renderer.DrawLine(segment.first, segment.second, RED, 2.0);
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