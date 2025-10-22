/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Junwoo Lee
 * \author [Your Name/Modified By]
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute
 */

#include "DemoAstar.hpp"

#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RenderingAPI.hpp"
#include "DemoReflection.hpp" // 데모 전환용
#include "DemoShapes.hpp"
#include "Engine/Dash.hpp" // DashComponent 사용
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Texture.hpp"
#include "Engine/TextureManager.hpp"
#include "Engine/Window.hpp"
#include "OpenGL/GL.hpp"

#include <algorithm> // std::clamp, std::max
#include <cmath>
#include <imgui.h>
#include <limits>   // numeric_limits
#include <optional> // optional

namespace
{
    constexpr Math::ivec2 OLIM_FRAME_SIZE{ 63, 127 };
    constexpr Math::ivec2 OLIM_HOT_SPOT{ 32, 63 };
    constexpr int         OLIM_NUM_FRAMES = 2;
} // namespace

void DemoAstar::Load()
{
    auto& texture_manager = Engine::GetTextureManager();
    olimTexture           = texture_manager.Load("Assets/images/DemoAstar/Robot.png"); // Ensure this file exists

    initializeOlimAnimations();

    CS200::RenderingAPI::SetClearColor(0x223344FF);

    // 캐릭터 초기 상태 설정
    idleOlim.animation            = OlimAnimation::None;
    idleOlim.frameIndex           = 0;
    idleOlim.timer                = 0.0;
    idleOlim.position             = Math::vec2{ 400.0, groundLevel + OLIM_HOT_SPOT.y + 100.0 }; // Start above ground
    idleOlim.faceRight            = true;
    idleOlim.velocityY            = 0.0;
    idleOlim.isJumping            = true; // Start falling
    idleOlim.currentPlatformIndex = std::nullopt;

    // 플랫폼 생성
    platforms.clear();
    const auto windowSize = Engine::GetWindow().GetSize();

    // 1. 바닥 플랫폼 추가
    platforms.push_back(
        {
            {                50.0, groundLevel },
            { windowSize.x - 50.0, groundLevel },
            platformThickness,
            false  // Cannot drop down
    });

    // 2. 공중 얇은 플랫폼 추가
    platforms.push_back(
        {
            { windowSize.x * 0.3, groundLevel + 150.0 },
            { windowSize.x * 0.7, groundLevel + 150.0 },
            thinPlatformThickness,
            true  // Can drop down
    });

    // DashComponent 초기화 (기본값 사용 또는 여기서 설정 변경)
    olimDash         = CS230::DashComponent{};
    olimDash.dashKey = CS230::Input::Keys::LShift; // Use Left Shift for dash
}

void DemoAstar::Update()
{
    const double delta_time = Engine::GetWindowEnvironment().DeltaTime;
    auto&        input      = Engine::GetInput();

    // --- 대시 상태 업데이트 (타이머) ---
    olimDash.UpdateTimers(delta_time);

    // --- 대시 시작 시도 ---
    // bool justDashed = olimDash.TryStartDash(input, idleOlim.faceRight);

    olimDash.TryStartDash(input, idleOlim.faceRight);

    // 1. 중력 적용
    bool applyGravity = true;
    if (olimDash.IsDashing() && olimDash.disableGravityOnDash)
    {
        applyGravity = false;
    }
    else if (!idleOlim.isJumping && idleOlim.currentPlatformIndex.has_value()) // 플랫폼 위에 있을 때
    {
        applyGravity       = false;
        idleOlim.velocityY = 0.0;
    }

    if (applyGravity)
    {
        idleOlim.velocityY -= gravity * delta_time;
    }

    // 2. 입력 및 이동 처리
    Math::vec2 move{ 0.0, 0.0 };
    double     currentSpeedX = 0.0;   // X축 최종 속도
    double     speed         = 300.0; // 일반 이동 속도

    if (olimDash.IsDashing()) // 대시 중일 때
    {
        currentSpeedX = olimDash.GetDashVelocityX();
    }
    else // 대시 중이 아닐 때 일반 이동
    {
        if (input.KeyDown(CS230::Input::Keys::A))
        {
            move.x -= 1.0;
            idleOlim.faceRight = false;
        }
        idleOlim.faceRight = false;
        if (input.KeyDown(CS230::Input::Keys::D))
        {
            move.x += 1.0;
            idleOlim.faceRight = false;
        }
        currentSpeedX = move.x * speed;
    }

    // 점프 처리 (대시 중에는 점프 불가)
    bool justJumped = false;
    if (!olimDash.IsDashing() && !idleOlim.isJumping && (input.KeyJustPressed(CS230::Input::Keys::Space) || input.KeyJustPressed(CS230::Input::Keys::W)))
    {
        idleOlim.velocityY            = jumpStrength;
        idleOlim.isJumping            = true;
        idleOlim.currentPlatformIndex = std::nullopt;
        justJumped                    = true;
    }

    // 아래로 내려가기 처리 (S 키) (대시 중에는 불가)
    bool tryingToDropDown = false;
    if (!olimDash.IsDashing() && idleOlim.currentPlatformIndex.has_value() && platforms[*idleOlim.currentPlatformIndex].canDropDown && input.KeyDown(CS230::Input::Keys::S) && !idleOlim.isJumping)
    {
        tryingToDropDown              = true;
        idleOlim.isJumping            = true;
        idleOlim.currentPlatformIndex = std::nullopt;
        idleOlim.position.y -= 50.0; // 살짝 아래로 이동
        // Optional: give small downward velocity
        // idleOlim.velocityY = -50.0;
    }


    // 3. 위치 업데이트 (예측)
    double     previousFeetY     = idleOlim.position.y - OLIM_HOT_SPOT.y;
    Math::vec2 predictedPosition = idleOlim.position;
    predictedPosition.x += currentSpeedX * delta_time; // 계산된 X 속도 적용

    // Y 위치 업데이트 (대시 중 중력 무시 옵션 적용)
    if (!olimDash.IsDashing() || !olimDash.disableGravityOnDash)
    {
        predictedPosition.y += idleOlim.velocityY * delta_time;
    }

    double predictedFeetY = predictedPosition.y - OLIM_HOT_SPOT.y;

    // 4. 플랫폼 충돌 처리
    bool                  landedOnPlatform    = false;
    double                landingPlatformY    = -std::numeric_limits<double>::infinity();
    std::optional<size_t> landedPlatformIndex = std::nullopt;
    const double          landingEpsilon      = 1.0;

    // 착지 검사 조건: 아래로 떨어지는 중이고, 방금 점프 X, S키 X, 대시 X
    if (idleOlim.velocityY <= 0 && !justJumped && !tryingToDropDown && !olimDash.IsDashing())
    {
        for (size_t i = 0; i < platforms.size(); ++i)
        {
            const auto& platform  = platforms[i];
            double      platformY = platform.start.y;

            // 충돌 조건 검사
            if (previousFeetY >= platformY - landingEpsilon && predictedFeetY <= platformY + landingEpsilon && predictedPosition.x >= platform.start.x - (OLIM_FRAME_SIZE.x / 2.0 - OLIM_HOT_SPOT.x) &&
                predictedPosition.x <= platform.end.x + (OLIM_FRAME_SIZE.x / 2.0 - OLIM_HOT_SPOT.x))
            {
                if (platformY > landingPlatformY) // 가장 높은 플랫폼에 착지
                {
                    landingPlatformY    = platformY;
                    landedPlatformIndex = i;
                    landedOnPlatform    = true;
                }
            }
        }
    }

    // 착지 처리
    if (landedOnPlatform)
    {
        predictedPosition.y           = landingPlatformY + OLIM_HOT_SPOT.y;
        idleOlim.velocityY            = 0.0;
        idleOlim.isJumping            = false;
        idleOlim.currentPlatformIndex = landedPlatformIndex;
    }
    else // 착지하지 않았을 경우 (떨어지는 중, 점프 중, 대시 중, S키 누름)
    {
        // 플랫폼 참조가 있는데 착지 판정이 안 났다면 (가장자리에서 떨어짐 등)
        // 또는 S키를 눌러서 참조를 없앴다면 공중 상태로 전환
        if (idleOlim.currentPlatformIndex.has_value() || tryingToDropDown)
        {
            idleOlim.currentPlatformIndex = std::nullopt; // 플랫폼 참조 제거
        }
        // 플랫폼 참조가 없거나 방금 없어졌다면 무조건 공중(점프/낙하) 상태
        idleOlim.isJumping = true;
    }

    // 최종 위치 업데이트
    idleOlim.position = predictedPosition;

    // 화면 경계 처리 (좌우)
    const auto windowSize = Engine::GetWindow().GetSize();
    idleOlim.position.x   = std::clamp(idleOlim.position.x, 0.0 + OLIM_HOT_SPOT.x, static_cast<double>(windowSize.x - OLIM_HOT_SPOT.x));


    // 5. 애니메이션 상태 업데이트
    const OlimAnimation prevAnim = idleOlim.animation;
    OlimAnimation       newAnim  = idleOlim.animation;

    if (olimDash.IsDashing()) // 대시 애니메이션
    {
        newAnim = OlimAnimation::OlimJump; // 점프 애니메이션 재활용
    }
    else if (idleOlim.isJumping) // 점프 애니메이션
    {
        newAnim = OlimAnimation::OlimJump;
        if (currentSpeedX > 0.0)
            idleOlim.faceRight = true;
        else if (currentSpeedX < 0.0)
            idleOlim.faceRight = false;
    }
    // 대시 중 아닐 때 좌우 이동 애니메이션
    else if (std::abs(currentSpeedX) > 0.01)
    {
        newAnim = OlimAnimation::OlimWalking;
        if (currentSpeedX > 0.0)
            idleOlim.faceRight = true;
        else if (currentSpeedX < 0.0)
            idleOlim.faceRight = false;
    }
    else // 정지 애니메이션
    {
        newAnim = OlimAnimation::None;
    }

    if (newAnim != prevAnim)
    {
        idleOlim.animation  = newAnim;
        idleOlim.frameIndex = 0;
        idleOlim.timer      = 0.0;
    }
    updateOlimAnimation(idleOlim, delta_time);
}

void DemoAstar::Draw() const
{
    CS200::RenderingAPI::Clear();
    auto& renderer_2d = Engine::GetRenderer2D();

    renderer_2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));

    // 모든 플랫폼 그리기
    for (const auto& platform : platforms)
    {
        renderer_2d.DrawLine(platform.start, platform.end, CS200::WHITE, platform.thickness);
    }

    // 캐릭터 그리기
    drawOlim(idleOlim);

    if (olimDash.dashCooldownTimer > 0.0 && olimDash.dashCooldown > 0.0) // 쿨다운 중일 때만 그리기
    {
        // 게이지 비율 계산 (0.0 ~ 1.0)
        double cooldownRatio = std::clamp(olimDash.dashCooldownTimer / olimDash.dashCooldown, 0.0, 1.0);

        // 게이지 바 위치 계산 (캐릭터 발 아래)
        Math::vec2 gaugeCenterPos = {
            idleOlim.position.x,                                     // 캐릭터 X 위치 중앙
            idleOlim.position.y - OLIM_HOT_SPOT.y - dashGaugeOffsetY // 캐릭터 발 아래 + 오프셋
        };

        // 1. 게이지 바 배경 그리기 (전체 너비)
        Math::TransformationMatrix bgTransform = Math::TranslationMatrix(gaugeCenterPos) * Math::ScaleMatrix({ dashGaugeWidth, dashGaugeHeight });
        renderer_2d.DrawRectangle(bgTransform, dashGaugeBgColor); // 배경색으로 채움

        // 2. 게이지 바 전경(남은 쿨다운) 그리기 (비율에 따른 너비)
        double                     fgWidth     = dashGaugeWidth * cooldownRatio;
        // 전경 바는 왼쪽 정렬되어야 하므로 중앙 위치에서 왼쪽으로 (전체 너비 - 현재 너비)/2 만큼 이동
        Math::vec2                 fgCenterPos = { gaugeCenterPos.x - (dashGaugeWidth - fgWidth) / 2.0, gaugeCenterPos.y };
        Math::TransformationMatrix fgTransform = Math::TranslationMatrix(fgCenterPos) * Math::ScaleMatrix({ fgWidth, dashGaugeHeight });
        renderer_2d.DrawRectangle(fgTransform, dashGaugeFgColor); // 전경색으로 채움
    }

    renderer_2d.EndScene();
}

void DemoAstar::DrawImGui()
{
    if (ImGui::Begin("DemoAstar Controls"))
    {
        ImGui::Text("Use A/D to move, W/Space to Jump, S to Drop, LeftShift to Dash"); // 설명 수정
        ImGui::Text("Current position: (%.1f, %.1f)", idleOlim.position.x, idleOlim.position.y);
        ImGui::Text("Current Y Velocity: %.1f", idleOlim.velocityY);
        ImGui::Text("Is Jumping/Falling: %s", idleOlim.isJumping ? "Yes" : "No");
        if (idleOlim.currentPlatformIndex.has_value())
        {
            ImGui::Text("On Platform Index: %zu (Can Drop: %s)", *idleOlim.currentPlatformIndex, platforms[*idleOlim.currentPlatformIndex].canDropDown ? "Yes" : "No");
        }
        else
        {
            ImGui::Text("On Platform Index: None");
        }

        // 대시 상태 표시 (DashComponent 사용)
        ImGui::Text("Is Dashing: %s", olimDash.IsDashing() ? "Yes" : "No");
        ImGui::Text("Dash Cooldown: %.2f", std::max(0.0, olimDash.dashCooldownTimer)); // 남은 시간 표시


        ImGui::SeparatorText("Physics Controls");
        static double min_gravity = 100.0, max_gravity = 3000.0;
        ImGui::DragScalar("Gravity", ImGuiDataType_Double, &gravity, 10.0f, &min_gravity, &max_gravity, "%.1f");
        static double min_jump = 100.0, max_jump = 1500.0;
        ImGui::DragScalar("Jump Strength", ImGuiDataType_Double, &jumpStrength, 10.0f, &min_jump, &max_jump, "%.1f");

        // 추가: 대시 설정 ImGui 컨트롤
        ImGui::SeparatorText("Dash Controls");
        static double min_dash_speed = 100.0, max_dash_speed = 2000.0;
        static double min_dash_dur = 0.05, max_dash_dur = 0.5;
        static double min_dash_cool = 0.1, max_dash_cool = 2.0;
        ImGui::DragScalar("Dash Speed", ImGuiDataType_Double, &olimDash.dashSpeed, 10.0f, &min_dash_speed, &max_dash_speed, "%.1f");
        ImGui::DragScalar("Dash Duration", ImGuiDataType_Double, &olimDash.dashDuration, 0.01f, &min_dash_dur, &max_dash_dur, "%.2f");
        ImGui::DragScalar("Dash Cooldown", ImGuiDataType_Double, &olimDash.dashCooldown, 0.05f, &min_dash_cool, &max_dash_cool, "%.2f");
        ImGui::Checkbox("Disable Gravity on Dash", &olimDash.disableGravityOnDash);

        ImGui::SeparatorText("Switch Demo");
        if (ImGui::Button("Switch to Demo Reflection"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<DemoLaserReflection>();
        }
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
    return "Demo A* (Platformer)"; // 이름 변경
}

void DemoAstar::initializeOlimAnimations()
{
    olimAnimations.clear();
    olimAnimations.resize(3); // None, OlimWalking, OlimJump

    // None (Idle)
    olimAnimations[static_cast<int>(OlimAnimation::None)] = { "Idle", { { 0, 0.5 } }, 0 };

    // Walking (loop)
    olimAnimations[static_cast<int>(OlimAnimation::OlimWalking)] = {
        "Walk", { { 0, 0.2 }, { 1, 0.2 } },
         0
    };

    // Jump (single frame for now, can be expanded)
    olimAnimations[static_cast<int>(OlimAnimation::OlimJump)] = { "Jump", { { 2, 1.0 } }, 0 }; // Using frame 2 as jump/fall frame
}

void DemoAstar::updateOlimAnimation(OlimState& character, double delta_time)
{
    const size_t anim_index = static_cast<size_t>(character.animation);
    if (anim_index >= olimAnimations.size())
        return; // Safety check

    const auto& anim = olimAnimations[anim_index];
    if (anim.frames.empty())
        return; // Safety check

    // Clamp frameIndex if out of bounds (e.g., after animation switch)
    if (character.frameIndex < 0 || static_cast<size_t>(character.frameIndex) >= anim.frames.size())
    {
        character.frameIndex = anim.loopFrame;
        character.timer      = 0.0; // Reset timer if index was invalid
    }

    character.timer += delta_time;
    const auto& current_frame = anim.frames[static_cast<size_t>(character.frameIndex)];

    // Advance frame if duration exceeded
    if (character.timer >= current_frame.duration)
    {
        character.timer -= current_frame.duration; // Subtract duration instead of resetting to 0
        character.frameIndex++;
        if (static_cast<size_t>(character.frameIndex) >= anim.frames.size())
        {
            character.frameIndex = anim.loopFrame; // Loop back
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

    // Clamp frameIndex before using
    int currentFrameIdx = character.frameIndex;
    if (currentFrameIdx < 0 || static_cast<size_t>(currentFrameIdx) >= anim.frames.size())
    {
        currentFrameIdx = anim.loopFrame;
    }

    const int  sprite_frame = anim.frames[static_cast<size_t>(currentFrameIdx)].frameIndex;
    const auto texel_base   = Math::ivec2{ sprite_frame * OLIM_FRAME_SIZE.x, 0 };
    const auto frame_size   = OLIM_FRAME_SIZE;
    const auto hot_spot     = OLIM_HOT_SPOT;

    const auto to_center = Math::TranslationMatrix(Math::vec2{ static_cast<double>(-hot_spot.x), static_cast<double>(-hot_spot.y) });
    const auto scale     = character.faceRight ? Math::ScaleMatrix({ 1.0, 1.0 }) : Math::ScaleMatrix({ -1.0, 1.0 });
    const auto translate = Math::TranslationMatrix(character.position);
    const auto transform = translate * scale * to_center;

    if (olimTexture)
    { // Safety check for texture pointer
        olimTexture->Draw(transform, texel_base, frame_size);
    }
}

// Dummy implementations for begin/endOffscreenRendering if not used elsewhere
DemoAstar::RenderInfo DemoAstar::beginOffscreenRendering() const
{
    return {};
}

GLuint DemoAstar::endOffscreenRendering([[maybe_unused]] const RenderInfo& render_info) const
{
    return 0;
}