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

void DemoBatchRendering::InitializeSprite(Sprite& sprite)
{
    sprite.position = { util::random(0.0, static_cast<double>(m_ScreenSize.x)), util::random(0.0, static_cast<double>(m_ScreenSize.y)) };
    sprite.velocity = { util::random(-100.0, 100.0), util::random(-100.0, 100.0) };
    sprite.rotation = util::random(0.0, 3.14159 * 2.0);
    sprite.tint     = CS200::pack_color({ static_cast<float>(util::random(0.5, 1.0)), static_cast<float>(util::random(0.5, 1.0)), static_cast<float>(util::random(0.5, 1.0)), 1.0f });

    sprite.is_cat = util::random(0, 2) == 0;

    if (sprite.is_cat)
    {
        const int    frame_cols   = 5;
        const double sheet_width  = 640.0;
        const double sheet_height = 256.0;
        sprite.frame_size         = { 128, 128 };

        int frame_index = util::random(0, 10);
        int frame_x     = frame_index % frame_cols;
        int frame_y     = frame_index / frame_cols;

        sprite.uv_bl = { (frame_x * 128.0) / sheet_width, (frame_y * 128.0) / sheet_height };
        sprite.uv_tr = { ((frame_x + 1) * 128.0) / sheet_width, ((frame_y + 1) * 128.0) / sheet_height };
    }
    else
    {
        const double sheet_width  = 315.0;
        const double sheet_height = 127.0;
        sprite.frame_size         = { 63, 127 };

        int frame_index = util::random(0, 5);
        int frame_x     = frame_index;
        int frame_y     = 0;

        sprite.uv_bl = { (frame_x * 63.0) / sheet_width, (frame_y * 127.0) / sheet_height };
        sprite.uv_tr = { ((frame_x + 1) * 63.0) / sheet_width, ((frame_y + 1) * 127.0) / sheet_height };
    }
}

void DemoBatchRendering::ChangeSpriteCount(int new_count)
{
    const int max_sprites = 200000;
    new_count             = std::clamp(new_count, 0, max_sprites);

    int current_size = static_cast<int>(m_Sprites.size());
    if (new_count > current_size)
    {
        m_Sprites.resize(new_count);
        for (int i = current_size; i < new_count; ++i)
        {
            InitializeSprite(m_Sprites[i]);
        }
    }
    else if (new_count < current_size)
    {
        m_Sprites.resize(new_count);
    }

    m_SpriteCount = new_count;
}

void DemoBatchRendering::Load()
{
    m_Renderer.Init();
    CS200::RenderingAPI::SetClearColor(0x1a1a1aff);

    m_TextureCat   = Engine::GetTextureManager().Load("Assets/images/DemoFramebuffer/Cat.png");
    m_TextureRobot = Engine::GetTextureManager().Load("Assets/images/DemoFramebuffer/Robot.png");

    m_ScreenSize = Engine::GetWindow().GetSize();

    m_Sprites.resize(static_cast<size_t>(m_SpriteCount));
    for (int i = 0; i < m_SpriteCount; ++i)
    {
        InitializeSprite(m_Sprites[static_cast<size_t>(i)]);
        auto& sprite    = m_Sprites[static_cast<size_t>(i)];
        sprite.position = { util::random(0.0, static_cast<double>(m_ScreenSize.x)), util::random(0.0, static_cast<double>(m_ScreenSize.y)) };
        sprite.velocity = { util::random(-100.0, 100.0), util::random(-100.0, 100.0) };
        sprite.rotation = util::random(0.0, 3.14159 * 2.0);
        sprite.tint     = CS200::pack_color({ static_cast<float>(util::random(0.5, 1.0)), static_cast<float>(util::random(0.5, 1.0)), static_cast<float>(util::random(0.5, 1.0)), 1.0f });
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

    for (int i = 0; i < m_SpriteCount; ++i)
    {
        const auto& sprite = m_Sprites[static_cast<size_t>(i)];

        if (sprite.is_cat)
        {
            Math::TransformationMatrix transform = Math::TranslationMatrix(sprite.position) * Math::RotationMatrix(sprite.rotation) *
                                                   Math::ScaleMatrix({ static_cast<double>(sprite.frame_size.x), static_cast<double>(sprite.frame_size.y) });

            renderer.DrawQuad(transform, m_TextureCat->GetHandle(), sprite.uv_bl, sprite.uv_tr, sprite.tint);
        }
        else
        {
            Math::TransformationMatrix transform = Math::TranslationMatrix(sprite.position) * Math::RotationMatrix(sprite.rotation) *
                                                   Math::ScaleMatrix({ static_cast<double>(sprite.frame_size.x), static_cast<double>(sprite.frame_size.y) });

            renderer.DrawQuad(transform, m_TextureRobot->GetHandle(), sprite.uv_bl, sprite.uv_tr, sprite.tint);
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
        ImGui::Text("Draw Calls: %u", m_Renderer.GetDrawCallCount());
        ImGui::Checkbox("Animate", &m_Animate);

        ImGui::SeparatorText("Sprite Count");
        if (ImGui::Button("+10000"))
        {
            ChangeSpriteCount(m_SpriteCount + 10000);
        }
        ImGui::SameLine();
        if (ImGui::Button("+1000"))
        {
            ChangeSpriteCount(m_SpriteCount + 1000);
        }
        ImGui::SameLine();
        if (ImGui::Button("+100"))
        {
            ChangeSpriteCount(m_SpriteCount + 100);
        }
        ImGui::SameLine();
        if (ImGui::Button("+10"))
        {
            ChangeSpriteCount(m_SpriteCount + 10);
        }
        ImGui::SameLine();
        if (ImGui::Button("+1"))
        {
            ChangeSpriteCount(m_SpriteCount + 1);
        }

        if (ImGui::Button("-10000"))
        {
            ChangeSpriteCount(m_SpriteCount - 10000);
        }
        ImGui::SameLine();
        if (ImGui::Button("-1000"))
        {
            ChangeSpriteCount(m_SpriteCount - 1000);
        }
        ImGui::SameLine();
        if (ImGui::Button("-100"))
        {
            ChangeSpriteCount(m_SpriteCount - 100);
        }
        ImGui::SameLine();
        if (ImGui::Button("-10"))
        {
            ChangeSpriteCount(m_SpriteCount - 10);
        }
        ImGui::SameLine();
        if (ImGui::Button("-1"))
        {
            ChangeSpriteCount(m_SpriteCount - 1);
        }

        if (ImGui::Button("Clear All"))
        {
            ChangeSpriteCount(0);
        }


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