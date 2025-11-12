/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "DemoCreativeScene.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RenderingAPI.hpp"
#include "DemoPerformance.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Random.hpp"
#include "Engine/TextureManager.hpp"
#include "Engine/Window.hpp"
#include <imgui.h>

void DemoCreativeScene::Load()
{
    m_renderer.Init();
    CS200::RenderingAPI::SetClearColor(0x050510FF);

    m_screen_size = Engine::GetWindow().GetSize();

    m_tex_planets     = Engine::GetTextureManager().Load("Assets/images/DemoFramebuffer/Planets.png");
    m_tex_foreground  = Engine::GetTextureManager().Load("Assets/images/DemoFramebuffer/Foreground.png");
    m_tex_white_pixel = Engine::GetTextureManager().Load("Assets/fonts/Font_Outlined.png");

    m_stars.resize(2000);
    for (auto& star : m_stars)
    {
        star.position     = { util::random(0.0, static_cast<double>(m_screen_size.x)), util::random(0.0, static_cast<double>(m_screen_size.y)) };
        star.scroll_speed = util::random(50.0, 150.0);
        star.tint         = CS200::pack_color({ 1.0f, 1.0f, static_cast<float>(util::random(0.5, 1.0)), 1.0f });
    }
}

void DemoCreativeScene::Update()
{
    const double dt = Engine::GetWindowEnvironment().DeltaTime;
    m_screen_size   = Engine::GetWindow().GetSize();

    double fast_speed = 400.0;
    double slow_speed = 30.0;

    m_fg_offset_platforms -= fast_speed * dt;
    m_bg_offset_planets -= slow_speed * dt;

    for (auto& star : m_stars)
    {
        star.position.x -= star.scroll_speed * dt;

        if (star.position.x < 0)
        {
            star.position.x = static_cast<double>(m_screen_size.x);
            star.position.y = util::random(0.0, static_cast<double>(m_screen_size.y));
        }
    }
}

void DemoCreativeScene::Unload()
{
    m_renderer.Shutdown();
}

void DemoCreativeScene::Draw() const
{
    CS200::RenderingAPI::Clear();
    m_screen_size  = Engine::GetWindow().GetSize();
    auto& renderer = const_cast<CS200::BatchRenderer2D&>(m_renderer);

    renderer.BeginScene(CS200::build_ndc_matrix(m_screen_size));

    Math::TransformationMatrix bg_transform = Math::TranslationMatrix(Math::vec2{ m_bg_offset_planets, static_cast<double>(m_screen_size.y) * 0.5 }) *
                                              Math::ScaleMatrix({ static_cast<double>(m_tex_planets->GetSize().x), static_cast<double>(m_tex_planets->GetSize().y) });
    renderer.DrawQuad(bg_transform, m_tex_planets->GetHandle(), { 0.0, 0.0 }, { 1.0, 1.0 }, CS200::WHITE);

    const double tex_w      = static_cast<double>(m_tex_white_pixel->GetSize().x);
    const double tex_h      = static_cast<double>(m_tex_white_pixel->GetSize().y);
    Math::vec2   star_uv_bl = { 0.0, 0.0 };
    Math::vec2   star_uv_tr = { 1.0 / tex_w, 1.0 / tex_h };

    for (const auto& star : m_stars)
    {
        double                     size           = star.scroll_speed < 100.0 ? 2.0 : 3.0;
        Math::TransformationMatrix star_transform = Math::TranslationMatrix(star.position) * Math::ScaleMatrix({ size, size });
        renderer.DrawQuad(star_transform, m_tex_white_pixel->GetHandle(), star_uv_bl, star_uv_tr, star.tint);
    }

    Math::ivec2 fg_size  = m_tex_foreground->GetSize();
    double      fg_x_pos = m_fg_offset_platforms;

    if (fg_x_pos < -fg_size.x)
    {
        m_fg_offset_platforms = 0.0;
        fg_x_pos              = 0.0;
    }

    Math::TransformationMatrix fg1_transform =
        Math::TranslationMatrix(Math::vec2{ fg_x_pos, static_cast<double>(fg_size.y) * 0.5 }) * Math::ScaleMatrix({ static_cast<double>(fg_size.x), static_cast<double>(fg_size.y) });

    Math::TransformationMatrix fg2_transform =
        Math::TranslationMatrix(Math::vec2{ fg_x_pos + fg_size.x, static_cast<double>(fg_size.y) * 0.5 }) * Math::ScaleMatrix({ static_cast<double>(fg_size.x), static_cast<double>(fg_size.y) });

    renderer.DrawQuad(fg1_transform, m_tex_foreground->GetHandle(), { 0.0, 0.0 }, { 1.0, 1.0 }, CS200::WHITE);
    renderer.DrawQuad(fg2_transform, m_tex_foreground->GetHandle(), { 0.0, 0.0 }, { 1.0, 1.0 }, CS200::WHITE);


    renderer.EndScene();
}

void DemoCreativeScene::DrawImGui()
{
    if (ImGui::Begin("Demo Controls"))
    {
        ImGui::Text("Renderer: Creative Scene (BatchRenderer2D)");
        ImGui::Text("FPS: %d", Engine::GetWindowEnvironment().FPS);
        ImGui::Text("Draw Calls: %u", m_renderer.GetDrawCallCount());
        ImGui::Text("Star Count: %zu", m_stars.size());

        ImGui::SeparatorText("Switch Demo");
        if (ImGui::Button("Switch to Performance Demo"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<DemoPerformance>();
        }
    }
    ImGui::End();
}

gsl::czstring DemoCreativeScene::GetName() const
{
    return "Demo Creative Scene";
}