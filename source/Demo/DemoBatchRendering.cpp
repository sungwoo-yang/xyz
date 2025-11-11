/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "DemoBatchRendering.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RenderingAPI.hpp"
#include "DemoInstancedRendering.hpp"
#include "DemoShapes.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Random.hpp"
#include "Engine/TextureManager.hpp"
#include "Engine/Window.hpp"
#include <imgui.h>

void DemoBatchRendering::Load()
{
    m_Renderer.Init();
    CS200::RenderingAPI::SetClearColor(0x1a1a1aff);

    m_TextureCat   = Engine::GetTextureManager().Load("Assets/images/DemoFramebuffer/Cat.png");
    m_TextureRobot = Engine::GetTextureManager().Load("Assets/images/DemoFramebuffer/Robot.png");

    m_ScreenSize = Engine::GetWindow().GetSize();

    m_Sprites.resize(m_SpriteCount);
    for (int i = 0; i < m_SpriteCount; ++i)
    {
        auto& sprite    = m_Sprites[i];
        sprite.position = { util::random(0.0, static_cast<double>(m_ScreenSize.x)), util::random(0.0, static_cast<double>(m_ScreenSize.y)) };
        sprite.velocity = { util::random(-100.0, 100.0), util::random(-100.0, 100.0) };
        sprite.rotation = util::random(0.0, 3.14159 * 2.0);
        sprite.tint     = CS200::pack_color({ (float)util::random(0.5, 1.0), (float)util::random(0.5, 1.0), (float)util::random(0.5, 1.0), 1.0f });
    }
}

void DemoBatchRendering::Update()
{
    if (!m_Animate)
        return;

    const double dt = Engine::GetWindowEnvironment().DeltaTime;
    m_ScreenSize    = Engine::GetWindow().GetSize();

    for (auto& sprite : m_Sprites)
    {
        sprite.position += sprite.velocity * dt;
        sprite.rotation += dt * 0.5;

        if (sprite.position.x < 0 || sprite.position.x > m_ScreenSize.x)
        {
            sprite.velocity.x *= -1;
            sprite.position.x = std::clamp(sprite.position.x, 0.0, static_cast<double>(m_ScreenSize.x));
        }
        if (sprite.position.y < 0 || sprite.position.y > m_ScreenSize.y)
        {
            sprite.velocity.y *= -1;
            sprite.position.y = std::clamp(sprite.position.y, 0.0, static_cast<double>(m_ScreenSize.y));
        }
    }
}

void DemoBatchRendering::Unload()
{
    m_Renderer.Shutdown();
}

void DemoBatchRendering::Draw() const
{
    CS200::RenderingAPI::Clear();
    m_ScreenSize = Engine::GetWindow().GetSize();

    auto& renderer = const_cast<CS200::BatchRenderer2D&>(m_Renderer);

    renderer.BeginScene(CS200::build_ndc_matrix(m_ScreenSize));

    const Math::ivec2 catFrameSize   = { 128, 128 };
    const Math::ivec2 robotFrameSize = { 63, 127 };
    const Math::ivec2 catTexelPos    = { 0, 0 };
    const Math::ivec2 robotTexelPos  = { 0, 0 };

    for (int i = 0; i < m_SpriteCount; ++i)
    {
        const auto& sprite = m_Sprites[i];

        if (i % 2 == 0)
        {
            Math::TransformationMatrix transform =
                Math::TranslationMatrix(sprite.position) * Math::RotationMatrix(sprite.rotation) * Math::ScaleMatrix({ (double)catFrameSize.x, (double)catFrameSize.y });

            renderer.DrawQuad(transform, m_TextureCat->GetHandle(), { 0.0, 0.0 }, { 128.0 / 640.0, 128.0 / 256.0 }, sprite.tint);
        }
        else
        {
            Math::TransformationMatrix transform =
                Math::TranslationMatrix(sprite.position) * Math::RotationMatrix(sprite.rotation) * Math::ScaleMatrix({ (double)robotFrameSize.x, (double)robotFrameSize.y });

            renderer.DrawQuad(transform, m_TextureRobot->GetHandle(), { 0.0, 0.0 }, { 63.0 / 315.0, 1.0 }, sprite.tint);
        }
    }

    renderer.EndScene();
}

void DemoBatchRendering::DrawImGui()
{
    if (ImGui::Begin("Demo Controls"))
    {
        ImGui::Text("Renderer: BatchRenderer2D");
        ImGui::Text("FPS: %d", Engine::GetWindowEnvironment().FPS);
        ImGui::Text("Sprite Count: %d", m_SpriteCount);
        ImGui::Checkbox("Animate", &m_Animate);

        ImGui::SeparatorText("Switch Demo");
        if (ImGui::Button("Switch to Immediate Mode (Shapes)"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<DemoShapes>();
        }
        if (ImGui::Button("Switch to Instanced Rendering"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<DemoInstancedRendering>();
        }
    }
    ImGui::End();
}

gsl::czstring DemoBatchRendering::GetName() const
{
    return "Demo Batch Rendering";
}