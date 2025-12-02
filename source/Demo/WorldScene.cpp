/**
 * \file WorldScene.cpp
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "WorldScene.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Input.hpp"
#include "Engine/Random.hpp"
#include "Engine/Window.hpp"
#include "OpenGL/GL.hpp"
#include <cmath>
#include <imgui.h>
#include <string>

void WorldScene::Load()
{
    m_batchRenderer.Init();
    m_immediateRenderer.Init();

    m_players.resize(4);
    m_cameras.resize(4);

    m_players[0] = {
        { 0, 0 },
        0.0, 0xFF5555FF
    };
    m_players[1] = {
        { 200, 200 },
        0.0, 0x55FF55FF
    };
    m_players[2] = {
        { -200, 200 },
        0.0, 0x5555FFFF
    };
    m_players[3] = {
        { 200, -200 },
        0.0, 0xFFFF55FF
    };

    for (int i = 0; i < 4; ++i)
    {
        m_cameras[static_cast<size_t>(i)].position = m_players[static_cast<size_t>(i)].position;
        m_cameras[static_cast<size_t>(i)].zoom     = 1.0;
        m_cameras[static_cast<size_t>(i)].mode     = CS200::CameraMode::FirstPerson_View;
    }

    for (int i = 0; i < 50; ++i)
    {
        WorldObject obj;
        obj.position = { util::random(-1500.0, 1500.0), util::random(-1500.0, 1500.0) };
        obj.scale    = { util::random(30.0, 80.0), util::random(30.0, 80.0) };
        obj.rotation = util::random(0.0, 3.14 * 2.0);
        obj.isCircle = (util::random(0, 2) == 0);

        float r   = static_cast<float>(util::random(0.4, 1.0));
        float g   = static_cast<float>(util::random(0.4, 1.0));
        float b   = static_cast<float>(util::random(0.4, 1.0));
        obj.color = CS200::pack_color({ r, g, b, 1.0f });

        m_worldObjects.push_back(obj);
    }

    CS200::RenderingAPI::SetClearColor(0x111111FF);
}

void WorldScene::Unload()
{
    m_batchRenderer.Shutdown();
    m_immediateRenderer.Shutdown();
}

void WorldScene::Update()
{
    const double dt = Engine::GetWindowEnvironment().DeltaTime;

    for (int i = 0; i < m_activeViewportCount; ++i)
    {
        UpdatePlayerInput(i, dt);

        CS200::Camera& cam    = m_cameras[static_cast<size_t>(i)];
        const Player&  player = m_players[static_cast<size_t>(i)];

        cam.position = player.position;

        if (cam.mode == CS200::CameraMode::FirstPerson_View)
        {
            cam.rotation = player.rotation;
        }
        else
        {
            cam.rotation = 0.0;
        }
    }
}

void WorldScene::UpdatePlayerInput(int playerIndex, double dt)
{
    auto&   input = Engine::GetInput();
    Player& p     = m_players[static_cast<size_t>(playerIndex)];

    double moveSpeed = 400.0;
    double rotSpeed  = 3.5;

    Math::vec2 moveDir{ 0.0, 0.0 };
    double     turnDir = 0.0;

    using namespace CS230;

    if (playerIndex == 0)
    {
        if (input.KeyDown(Input::Keys::W))
            moveDir.x += 1.0;
        if (input.KeyDown(Input::Keys::S))
            moveDir.x -= 1.0;
        if (input.KeyDown(Input::Keys::A))
            turnDir += 1.0;
        if (input.KeyDown(Input::Keys::D))
            turnDir -= 1.0;
    }
    else if (playerIndex == 1)
    {
        if (input.KeyDown(Input::Keys::Up))
            moveDir.x += 1.0;
        if (input.KeyDown(Input::Keys::Down))
            moveDir.x -= 1.0;
        if (input.KeyDown(Input::Keys::Left))
            turnDir += 1.0;
        if (input.KeyDown(Input::Keys::Right))
            turnDir -= 1.0;
    }
    else if (playerIndex == 2)
    {
        if (input.KeyDown(Input::Keys::I))
            moveDir.x += 1.0;
        if (input.KeyDown(Input::Keys::K))
            moveDir.x -= 1.0;
        if (input.KeyDown(Input::Keys::J))
            turnDir += 1.0;
        if (input.KeyDown(Input::Keys::L))
            turnDir -= 1.0;
    }
    else if (playerIndex == 3)
    {
        if (input.KeyDown(Input::Keys::Num8))
            moveDir.x += 1.0;
        if (input.KeyDown(Input::Keys::Num5))
            moveDir.x -= 1.0;
        if (input.KeyDown(Input::Keys::Num4))
            turnDir += 1.0;
        if (input.KeyDown(Input::Keys::Num6))
            turnDir -= 1.0;
    }

    p.rotation += turnDir * rotSpeed * dt;
    Math::vec2 forwardVector = { std::cos(p.rotation), std::sin(p.rotation) };
    p.position += forwardVector * (moveDir.x * moveSpeed * dt);
}

void WorldScene::Draw() const
{
    CS200::RenderingAPI::Clear();
    auto windowSize = Engine::GetWindow().GetSize();

    struct ViewportRect
    {
        int x, y, w, h;
    };

    std::vector<ViewportRect> viewports;

    int halfW = windowSize.x / 2;
    int halfH = windowSize.y / 2;

    if (m_activeViewportCount == 1)
        viewports.push_back({ 0, 0, windowSize.x, windowSize.y });
    else if (m_activeViewportCount == 2)
    {
        viewports.push_back({ 0, 0, halfW, windowSize.y });
        viewports.push_back({ halfW, 0, windowSize.x - halfW, windowSize.y });
    }
    else if (m_activeViewportCount == 3)
    {
        viewports.push_back({ 0, halfH, windowSize.x, windowSize.y - halfH });
        viewports.push_back({ 0, 0, halfW, halfH });
        viewports.push_back({ halfW, 0, windowSize.x - halfW, halfH });
    }
    else
    {
        viewports.push_back({ 0, halfH, halfW, windowSize.y - halfH });
        viewports.push_back({ halfW, halfH, windowSize.x - halfW, windowSize.y - halfH });
        viewports.push_back({ 0, 0, halfW, halfH });
        viewports.push_back({ halfW, 0, windowSize.x - halfW, halfH });
    }

    for (int i = 0; i < m_activeViewportCount; ++i)
    {
        const auto& vp = viewports[static_cast<size_t>(i)];

        CS200::RenderingAPI::SetViewport({ vp.w, vp.h }, { vp.x, vp.y });

        Math::TransformationMatrix ndc = CS200::build_ndc_matrix({ vp.w, vp.h });

        Math::TranslationMatrix screenCenterOffset(Math::vec2{ static_cast<double>(vp.w) * 0.5, static_cast<double>(vp.h) * 0.5 });

        Math::TransformationMatrix view = m_cameras[static_cast<size_t>(i)].GetViewMatrix();

        Math::TransformationMatrix cameraMatrix = ndc * screenCenterOffset * view;

        DrawWorld(cameraMatrix);
    }

    CS200::RenderingAPI::SetViewport(windowSize, { 0, 0 });
    DrawViewportSeparators(windowSize);
}

void WorldScene::DrawWorld(const Math::TransformationMatrix& viewProj) const
{
    auto& immRenderer = const_cast<CS200::ImmediateRenderer2D&>(m_immediateRenderer);
    immRenderer.BeginScene(viewProj);

    DrawGrid(immRenderer);

    for (const auto& obj : m_worldObjects)
    {
        Math::TransformationMatrix transform = Math::TranslationMatrix(obj.position) * Math::RotationMatrix(obj.rotation) * Math::ScaleMatrix(obj.scale);

        if (obj.isCircle)
            immRenderer.DrawCircle(transform, obj.color, CS200::WHITE, 2.0);
        else
            immRenderer.DrawRectangle(transform, obj.color, CS200::WHITE, 2.0);
    }

    for (int i = 0; i < 4; ++i)
    {
        DrawPlayerShape(immRenderer, m_players[static_cast<size_t>(i)]);
    }

    immRenderer.EndScene();
}

void WorldScene::DrawGrid(CS200::ImmediateRenderer2D& renderer) const
{
    const int   gridSize  = 2000;
    const int   step      = 100;
    CS200::RGBA gridColor = 0xFFFFFF33;

    for (int x = -gridSize; x <= gridSize; x += step)
    {
        renderer.DrawLine(Math::vec2{ static_cast<double>(x), static_cast<double>(-gridSize) }, Math::vec2{ static_cast<double>(x), static_cast<double>(gridSize) }, gridColor, 1.0);
    }
    for (int y = -gridSize; y <= gridSize; y += step)
    {
        renderer.DrawLine(Math::vec2{ static_cast<double>(-gridSize), static_cast<double>(y) }, Math::vec2{ static_cast<double>(gridSize), static_cast<double>(y) }, gridColor, 1.0);
    }

    renderer.DrawLine(Math::vec2{ 0, 0 }, Math::vec2{ 100, 0 }, 0xFF0000FF, 3.0);
    renderer.DrawLine(Math::vec2{ 0, 0 }, Math::vec2{ 0, 100 }, 0x00FF00FF, 3.0);
}

void WorldScene::DrawPlayerShape(CS200::ImmediateRenderer2D& renderer, const Player& p) const
{
    Math::TransformationMatrix transform = Math::TranslationMatrix(p.position) * Math::RotationMatrix(p.rotation);

    Math::vec2 tip{ 40.0, 0.0 };
    Math::vec2 backTop{ -30.0, 20.0 };
    Math::vec2 backBot{ -30.0, -20.0 };
    Math::vec2 backIndent{ -20.0, 0.0 };

    double thickness = 3.0;
    renderer.DrawLine(transform, tip, backTop, p.color, thickness);
    renderer.DrawLine(transform, backTop, backIndent, p.color, thickness);
    renderer.DrawLine(transform, backIndent, backBot, p.color, thickness);
    renderer.DrawLine(transform, backBot, tip, p.color, thickness);
}

void WorldScene::DrawViewportSeparators(Math::ivec2 windowSize) const
{
    if (m_activeViewportCount <= 1)
        return;

    auto&                      immRenderer = const_cast<CS200::ImmediateRenderer2D&>(m_immediateRenderer);
    Math::TransformationMatrix ndc         = CS200::build_ndc_matrix(windowSize);
    immRenderer.BeginScene(ndc);

    CS200::RGBA lineColor = CS200::WHITE;
    double      thickness = 4.0;

    double w  = static_cast<double>(windowSize.x);
    double h  = static_cast<double>(windowSize.y);
    double hw = w * 0.5;
    double hh = h * 0.5;

    if (m_activeViewportCount == 2)
    {
        immRenderer.DrawLine(Math::vec2{ hw, 0 }, Math::vec2{ hw, h }, lineColor, thickness);
    }
    else if (m_activeViewportCount == 3)
    {
        immRenderer.DrawLine(Math::vec2{ 0, hh }, Math::vec2{ w, hh }, lineColor, thickness);
        immRenderer.DrawLine(Math::vec2{ hw, 0 }, Math::vec2{ hw, hh }, lineColor, thickness);
    }
    else if (m_activeViewportCount == 4)
    {
        immRenderer.DrawLine(Math::vec2{ hw, 0 }, Math::vec2{ hw, h }, lineColor, thickness);
        immRenderer.DrawLine(Math::vec2{ 0, hh }, Math::vec2{ w, hh }, lineColor, thickness);
    }

    immRenderer.EndScene();
}

void WorldScene::DrawImGui()
{
    if (ImGui::Begin("Program Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        const auto&    env = Engine::GetWindowEnvironment();
        CS200::Camera& cam = m_cameras[0];
        const Player&  p   = m_players[0];

        ImGui::Text("%d", env.FPS);
        ImGui::SameLine(200);
        ImGui::Text("FPS");
        ImGui::Text("%.3f seconds", env.DeltaTime);
        ImGui::SameLine(200);
        ImGui::Text("Delta time");

        ImGui::Text("Device( %.0f, %.0f)", env.DisplaySize.x, env.DisplaySize.y);
        ImGui::SameLine(200);
        ImGui::Text("Screen Size");

        ImGui::Text("Camera( %.0f, %.0f)", cam.position.x, cam.position.y);
        ImGui::Text("World ( %.0f, %.0f)", p.position.x, p.position.y);

        double degrees = cam.rotation * 180.0 / 3.14159265;
        while (degrees < 0)
            degrees += 360.0;
        while (degrees >= 360.0)
            degrees -= 360.0;
        ImGui::Text("%.0f degrees", degrees);

        ImGui::Separator();

        double zoom        = cam.zoom;
        int    zoomPercent = static_cast<int>(zoom * 100.0);

        if (ImGui::SliderInt("##Zoom", &zoomPercent, 10, 500, "%d%%"))
        {
            cam.zoom = static_cast<double>(zoomPercent) / 100.0;
        }
        ImGui::SameLine();
        ImGui::Text("Zoom");

        bool isFirstPerson = (cam.mode == CS200::CameraMode::FirstPerson_View);
        if (ImGui::Checkbox("First Person Camera", &isFirstPerson))
        {
            cam.mode = isFirstPerson ? CS200::CameraMode::FirstPerson_View : CS200::CameraMode::ThirdPerson_Follow;
        }

        ImGui::Separator();
        if (ImGui::TreeNode("Viewport Controls"))
        {
            if (ImGui::RadioButton("1 View", m_activeViewportCount == 1))
                m_activeViewportCount = 1;
            ImGui::SameLine();
            if (ImGui::RadioButton("2 Views", m_activeViewportCount == 2))
                m_activeViewportCount = 2;
            if (ImGui::RadioButton("3 Views", m_activeViewportCount == 3))
                m_activeViewportCount = 3;
            ImGui::SameLine();
            if (ImGui::RadioButton("4 Views", m_activeViewportCount == 4))
                m_activeViewportCount = 4;
            ImGui::TreePop();
        }
    }
    ImGui::End();
}
