/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Junwoo Lee
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute
 */

#include "DemoAstar.hpp"

#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RenderingAPI.hpp"
#include "DemoShapes.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Texture.hpp"
#include "Engine/TextureManager.hpp"
#include "Engine/Window.hpp"
#include "OpenGL/GL.hpp"

#include <cmath>
#include <imgui.h>

namespace
{
    constexpr Math::ivec2 OLIM_FRAME_SIZE{ 64, 64 };
    constexpr Math::ivec2 OLIM_HOT_SPOT{ 32, 32 };
    constexpr int         OLIM_NUM_FRAMES = 4;
} // namespace

void DemoAstar::Load()
{
    auto& texture_manager = Engine::GetTextureManager();
    olimTexture           = texture_manager.Load("Assets/images/DemoAstar/Robot.png"); // 실제 파일 필요

    initializeOlimAnimations();

    CS200::RenderingAPI::SetClearColor(0x223344FF);

    idleOlim.animation  = OlimAnimation::None;
    idleOlim.frameIndex = 0;
    idleOlim.timer      = 0.0;
    idleOlim.position   = Math::vec2{ 400.0, 300.0 };
    idleOlim.faceRight  = true;
}

void DemoAstar::Update()
{
    const double delta_time = Engine::GetWindowEnvironment().DeltaTime;
    auto&        input      = Engine::GetInput();

    Math::vec2 move{ 0.0, 0.0 };
    double     speed = 200.0; // pixels/sec

    if (input.KeyDown(CS230::Input::Keys::W))
        move.y += 1.0;
    if (input.KeyDown(CS230::Input::Keys::S))
        move.y -= 1.0;
    if (input.KeyDown(CS230::Input::Keys::A))
    {
        move.x -= 1.0;
        // don't set faceRight until we decide final animation
    }
    if (input.KeyDown(CS230::Input::Keys::D))
    {
        move.x += 1.0;
    }

    // Determine desired animation and facing based on move
    const OlimAnimation prevAnim = idleOlim.animation;
    OlimAnimation       newAnim  = OlimAnimation::None;
    if (move.x != 0.0 || move.y != 0.0)
    {
        newAnim = OlimAnimation::OlimWalking;
        // set facing based on horizontal movement (prefer horizontal)
        if (move.x > 0.0)
            idleOlim.faceRight = true;
        else if (move.x < 0.0)
            idleOlim.faceRight = false;
    }
    else
    {
        newAnim = OlimAnimation::None;
    }

    // If animation changed, reset frameIndex/timer to avoid out-of-range indexing
    if (newAnim != prevAnim)
    {
        idleOlim.animation  = newAnim;
        idleOlim.frameIndex = 0;
        idleOlim.timer      = 0.0;
    }
    else
    {
        // keep same animation
        idleOlim.animation = newAnim;
    }

    // Normalize move vector correctly using member Normalize()
    if (move.x != 0.0 || move.y != 0.0)
    {
        move = move.Normalize();
    }

    // Update position
    idleOlim.position += move * speed * delta_time;

    // Update animation (safe: updateOlimAnimation does extra bounds checks)
    updateOlimAnimation(idleOlim, delta_time);
}

void DemoAstar::Draw() const
{
    CS200::RenderingAPI::Clear();
    auto& renderer_2d = Engine::GetRenderer2D();

    renderer_2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));

    drawOlim(idleOlim);

    renderer_2d.EndScene();
}

void DemoAstar::DrawImGui()
{
    if (ImGui::Begin("DemoAstar Controls"))
    {
        ImGui::Text("Use WASD to move Olim");
        ImGui::Text("Current position: (%.1f, %.1f)", idleOlim.position.x, idleOlim.position.y);

        if (ImGui::Button("Switch to Demo Shapes"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<DemoShapes>();
        }
    }
    ImGui::End();
}

void DemoAstar::Unload()
{
    if (lastFramebufferTexture != 0)
    {
        GL::DeleteTextures(1, &lastFramebufferTexture);
        lastFramebufferTexture = 0;
    }
}

gsl::czstring DemoAstar::GetName() const
{
    return "Demo A* (Olim Movement)";
}

void DemoAstar::initializeOlimAnimations()
{
    olimAnimations.clear();
    olimAnimations.resize(3);

    // None (Idle)
    olimAnimations[static_cast<int>(OlimAnimation::None)] = { "Idle", { { 0, 0.5 } }, 0 };

    // Walking (loop)
    olimAnimations[static_cast<int>(OlimAnimation::OlimWalking)] = {
        "Walk", { { 0, 0.2 }, { 1, 0.2 }, { 2, 0.2 }, { 3, 0.2 } },
         0
    };

    olimAnimations[static_cast<int>(OlimAnimation::OlimJump)] = { "Jump", { { 2, 1.0 } }, 0 };
}

void DemoAstar::updateOlimAnimation(OlimState& character, double delta_time)
{
    // Safety: ensure valid animation index
    const size_t anim_index = static_cast<size_t>(character.animation);
    if (anim_index >= olimAnimations.size())
        return;

    const auto& anim = olimAnimations[anim_index];

    // If there are no frames, nothing to do
    if (anim.frames.empty())
    {
        character.frameIndex = 0;
        character.timer      = 0.0;
        return;
    }

    // Safety: if frameIndex is out of range (e.g. animation switched), clamp to loopFrame
    if (character.frameIndex < 0 || static_cast<size_t>(character.frameIndex) >= anim.frames.size())
    {
        character.frameIndex = anim.loopFrame;
        character.timer      = 0.0;
    }

    character.timer += delta_time;
    const auto& current_frame = anim.frames[static_cast<size_t>(character.frameIndex)];
    if (character.timer >= current_frame.duration)
    {
        character.timer = 0.0;
        ++character.frameIndex;
        if (static_cast<size_t>(character.frameIndex) >= anim.frames.size())
        {
            character.frameIndex = anim.loopFrame;
        }
    }
}

void DemoAstar::drawOlim(const OlimState& character) const
{
    const size_t anim_index = static_cast<size_t>(character.animation);
    if (anim_index >= olimAnimations.size())
        return;

    const auto& anim = olimAnimations[anim_index];

    if (anim.frames.empty())
        return;

    // Safety: make sure frameIndex is valid before using it (defensive)
    int frameIndex = character.frameIndex;
    if (frameIndex < 0 || static_cast<size_t>(frameIndex) >= anim.frames.size())
        frameIndex = anim.loopFrame;

    const int  sprite_frame = anim.frames[static_cast<size_t>(frameIndex)].frameIndex;
    const auto texel_base   = Math::ivec2{ sprite_frame * OLIM_FRAME_SIZE.x, 0 };
    const auto frame_size   = OLIM_FRAME_SIZE;
    const auto hot_spot     = OLIM_HOT_SPOT;

    const auto to_center = Math::TranslationMatrix(Math::vec2{ static_cast<double>(-hot_spot.x), static_cast<double>(-hot_spot.y) });
    const auto scale     = character.faceRight ? Math::ScaleMatrix({ 1.0, 1.0 }) : Math::ScaleMatrix({ -1.0, 1.0 });
    const auto translate = Math::TranslationMatrix(character.position);
    const auto transform = translate * scale * to_center;

    olimTexture->Draw(transform, texel_base, frame_size);
}

DemoAstar::RenderInfo DemoAstar::beginOffscreenRendering() const
{
    RenderInfo render_info;
    const auto [width, height] = Engine::GetWindow().GetSize();
    render_info.Size           = { width / 2, height / 2 };

    GL::GenTextures(1, &render_info.ColorTexture);
    GL::BindTexture(GL_TEXTURE_2D, render_info.ColorTexture);
    GL::TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, render_info.Size.x, render_info.Size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GL::GenFramebuffers(1, &render_info.Framebuffer);
    GL::BindFramebuffer(GL_FRAMEBUFFER, render_info.Framebuffer);
    GL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_info.ColorTexture, 0);

    GL::Viewport(0, 0, render_info.Size.x, render_info.Size.y);
    GL::ClearColor(0, 0, 0, 0);
    GL::Clear(GL_COLOR_BUFFER_BIT);

    return render_info;
}

GLuint DemoAstar::endOffscreenRendering(const RenderInfo& render_info) const
{
    GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
    return render_info.ColorTexture;
}
