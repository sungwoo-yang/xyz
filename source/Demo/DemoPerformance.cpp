/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "DemoPerformance.hpp"
#include "CS200/BatchRenderer2D.hpp"
#include "CS200/ImmediateRenderer2D.hpp"
#include "CS200/InstancedRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RenderingAPI.hpp"
#include "DemoCreativeScene.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Random.hpp"
#include "Engine/TextureManager.hpp"
#include "Engine/Window.hpp"
#include <imgui.h>

void DemoPerformance::InitializeSprite(Sprite& sprite)
{
    m_ScreenSize = Engine::GetWindow().GetSize();

    sprite.position = { util::random(0.0, static_cast<double>(m_ScreenSize.x)), util::random(0.0, static_cast<double>(m_ScreenSize.y)) };
    sprite.velocity = { util::random(-100.0, 100.0), util::random(-100.0, 100.0) };
    sprite.rotation = util::random(0.0, 3.14159 * 2.0);
    sprite.tint     = CS200::pack_color({ static_cast<float>(util::random(0.5, 1.0)), static_cast<float>(util::random(0.5, 1.0)), static_cast<float>(util::random(0.5, 1.0)), 1.0f });

    sprite.textureIndex = util::random(0, static_cast<int>(m_SpriteTextures.size())); // <-- 수정

    if (sprite.textureIndex % 2 == 0)
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

void DemoPerformance::ChangeSpriteCount(int new_count)
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

void DemoPerformance::SwitchRenderer(RendererType type)
{
    if (m_Renderer && m_CurrentRenderer == type)
        return;

    if (m_Renderer)
    {
        m_Renderer->Shutdown();
        m_Renderer.reset();
    }

    m_CurrentRenderer = type;
    switch (type)
    {
        case RendererType::Immediate: m_Renderer = std::make_unique<CS200::ImmediateRenderer2D>(); break;
        case RendererType::Batch: m_Renderer = std::make_unique<CS200::BatchRenderer2D>(); break;
        case RendererType::Instanced: m_Renderer = std::make_unique<CS200::InstancedRenderer2D>(); break;
    }

    if (m_Renderer)
    {
        m_Renderer->Init();
    }
}

void DemoPerformance::Load()
{
    CS200::RenderingAPI::SetClearColor(0x1a1a1aff);

    for (int i = 1; i <= 64; ++i)
    {
        std::string num = std::to_string(i);
        if (i < 10)
            num = "0" + num;
        std::string path = "Assets/images/variations/robot_var_" + num + ".png";
        m_SpriteTextures.push_back(Engine::GetTextureManager().Load(path));
    }

    m_ScreenSize = Engine::GetWindow().GetSize();

    m_Sprites.resize(static_cast<size_t>(m_SpriteCount));
    for (int i = 0; i < m_SpriteCount; ++i)
    {
        InitializeSprite(m_Sprites[static_cast<size_t>(i)]);
    }

    SwitchRenderer(m_CurrentRenderer);
}

void DemoPerformance::Update()
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

void DemoPerformance::Unload()
{
    m_Renderer->Shutdown();
}

void DemoPerformance::Draw() const
{
    CS200::RenderingAPI::Clear();
    m_ScreenSize = Engine::GetWindow().GetSize();

    auto& renderer = *m_Renderer;

    renderer.BeginScene(CS200::build_ndc_matrix(m_ScreenSize));

    for (int i = 0; i < m_SpriteCount; ++i)
    {
        const auto& sprite  = m_Sprites[static_cast<size_t>(i)];
        const auto& texture = m_SpriteTextures[sprite.textureIndex];

        Math::TransformationMatrix transform = Math::TranslationMatrix(sprite.position) * Math::RotationMatrix(sprite.rotation) *
                                               Math::ScaleMatrix({ static_cast<double>(sprite.frame_size.x), static_cast<double>(sprite.frame_size.y) });

        renderer.DrawQuad(transform, texture->GetHandle(), sprite.uv_bl, sprite.uv_tr, sprite.tint);
    }

    renderer.EndScene();
}

void DemoPerformance::DrawImGui()
{
    if (ImGui::Begin("Demo Controls"))
    {
        ImGui::Text("Renderer: ");
        if (ImGui::RadioButton("Immediate", m_CurrentRenderer == RendererType::Immediate))
        {
            SwitchRenderer(RendererType::Immediate);
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Batch", m_CurrentRenderer == RendererType::Batch))
        {
            SwitchRenderer(RendererType::Batch);
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Instanced", m_CurrentRenderer == RendererType::Instanced))
        {
            SwitchRenderer(RendererType::Instanced);
        }

        ImGui::Text("FPS: %d", Engine::GetWindowEnvironment().FPS);
        ImGui::Text("Sprite Count: %d", m_SpriteCount);
        ImGui::Text("Draw Calls: %u", m_Renderer->GetDrawCallCount());
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

        if (ImGui::Button("Switch to Creative Scene"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<DemoCreativeScene>();
        }
    }
    ImGui::End();
}

gsl::czstring DemoPerformance::GetName() const
{
    return "Demo Batch Rendering";
}