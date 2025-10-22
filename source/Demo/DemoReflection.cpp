#include "DemoReflection.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Physics/Reflection.hpp" // Physics 네임스페이스 함수 선언 포함
#include "Engine/Window.hpp"

#include <algorithm>
#include <cmath>
#include <imgui.h>
#include <limits>
#include <numbers> // std::numbers 사용 가능 시 (C++20)
#include <string>  // std::to_string 사용 위해 추가

// --- 보조 함수 정의 ---
namespace // Anonymous namespace for helper functions
{
    // 내적 계산
    inline double dot(const Math::vec2& a, const Math::vec2& b)
    {
        return a.x * b.x + a.y * b.y;
    }

    // 벡터에 수직인 벡터 계산 (시계 반대 방향 90도 회전)
    inline Math::vec2 perpendicular(const Math::vec2& v)
    {
        return Math::vec2{ -v.y, v.x };
    }

    // Helper function: Line segment-circle intersection
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
            discriminant       = std::sqrt(discriminant);
            double t1          = (-b - discriminant) / (2.0 * a);
            double t2          = (-b + discriminant) / (2.0 * a);
            bool   intersected = false;
            // Intersection point calculation adjusted slightly for clarity
            if (t1 >= -std::numeric_limits<double>::epsilon() && t1 <= 1.0 + std::numeric_limits<double>::epsilon())
            {
                intersection = p1 + d * t1;
                intersected  = true;
            }
            if (t2 >= -std::numeric_limits<double>::epsilon() && t2 <= 1.0 + std::numeric_limits<double>::epsilon())
            {
                // If t1 wasn't valid or t2 is also valid (we just need one valid point on the segment)
                if (!intersected)
                {
                    intersection = p1 + d * t2;
                }
                intersected = true;
            }
            return intersected;
        }
    }

    // --- 색상 보간 관련 함수들 ---
    CS200::RGBA LerpColor(CS200::RGBA start, CS200::RGBA end, double t)
    {
        t                      = std::clamp(t, 0.0, 1.0);
        auto                 s = CS200::unpack_color(start);
        auto                 e = CS200::unpack_color(end);
        std::array<float, 4> result;
        for (int i = 0; i < 4; ++i)
        {
            result[i] = s[i] + static_cast<float>(t) * (e[i] - s[i]);
        }
        return CS200::pack_color(result);
    }

    std::array<float, 4> operator-(const std::array<float, 4>& a, const std::array<float, 4>& b)
    {
        return { a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3] };
    }

    std::array<float, 4> operator*(double scalar, const std::array<float, 4>& arr)
    {
        const auto s = static_cast<float>(scalar);
        return { s * arr[0], s * arr[1], s * arr[2], s * arr[3] };
    }

    std::array<float, 4>& operator+=(std::array<float, 4>& a, const std::array<float, 4>& b)
    {
        a[0] += b[0];
        a[1] += b[1];
        a[2] += b[2];
        a[3] += b[3];
        return a;
    }

    template <typename FLOAT = double>
    void ease_color_to_target(std::array<float, 4>& current, const std::array<float, 4>& target, FLOAT delta_time, FLOAT weight = 1.0)
    {
        const auto easing = std::min(delta_time * weight, static_cast<FLOAT>(1.0));
        current += easing * (target - current);
    }

} // end anonymous namespace

// --- 보조 함수 정의 끝 ---


void DemoLaserReflection::Load()
{
    CS200::RenderingAPI::SetClearColor(COLOR_BLACK);
    const auto windowSize = Engine::GetWindow().GetSize();
    characterPos          = { windowSize.x / 2.0, windowSize.y / 2.0 };
    laserOrigin           = { 0.0, static_cast<double>(windowSize.y) };
    shieldAngle           = PI / 2.0;
    isLaserOn             = false;
    laserTimer            = 0.0;
    laserColor            = COLOR_RED;
    showingWarningLaser   = false;
    warningLaserColor     = COLOR_WARNING; // 초기 예고선 색상 설정
    isParrying            = false;
    parryWindowActive     = false;
    parryTimer            = 0.0;
    shieldColor           = COLOR_CYAN;
    currentShieldColor    = CS200::unpack_color(COLOR_CYAN); // float 배열 초기화
    targetShieldColor     = CS200::unpack_color(COLOR_CYAN); // float 배열 초기화
    shieldHitTimer        = shieldColorRecoveryTime;         // 바로 복구 가능하도록 초기화
    isShieldFrozen        = false;
    shieldFrozenTimer     = 0.0;

    targets.clear();
    targets.push_back({}); // 오른쪽 위
    targets.back().position = { windowSize.x - 50.0, windowSize.y - 50.0 };
    targets.push_back({}); // 왼쪽 아래
    targets.back().position = { 50.0, 50.0 };

    UpdateShield();
}

void DemoLaserReflection::Unload()
{
    // 리소스 해제 코드
}

gsl::czstring DemoLaserReflection::GetName() const
{
    return "Demo: Laser Reflection Parry";
}

void DemoLaserReflection::Update()
{
    const auto&  input = Engine::GetInput();
    const double dt    = Engine::GetWindowEnvironment().DeltaTime;

    // --- 레이저 타이머 및 상태 업데이트 ---
    laserTimer += dt;
    double cycleTime = fmod(laserTimer, laserCycleTime);

    bool prevLaserState = isLaserOn;
    // bool prevWarningState = showingWarningLaser;

    showingWarningLaser = !isLaserOn && (cycleTime >= laserCycleTime - warningLaserLeadTime);
    isLaserOn           = (cycleTime < laserOnDuration);

    // --- 패링 윈도우 관리 및 예고선 색상 결정 ---
    parryWindowActive = false; // 기본값은 비활성
    if (showingWarningLaser)
    {
        // 예고 시간이 시작된 후 얼마나 지났는지 계산
        double timeIntoWarning = cycleTime - (laserCycleTime - warningLaserLeadTime);

        // 예고 시간의 후반부 0.5초가 패리 윈도우
        if (timeIntoWarning >= parryWindowStartTimeOffset)
        {
            parryWindowActive = true;
            warningLaserColor = COLOR_PARRY_WARNING; // 하늘색 예고선
            parryTimer += dt;
            if (input.KeyJustPressed(CS230::Input::Keys::Space))
            {
                isParrying = true; // 패링 성공 플래그 설정
                Engine::GetLogger().LogEvent("Parry Input Success!");
            }
        }
        else // 예고 시간의 전반부
        {
            warningLaserColor = COLOR_WARNING; // 빨간색 예고선
            parryTimer        = 0.0;           // 패리 윈도우 시작 전이므로 타이머 리셋
        }
    }
    else // 예고 시간이 아닐 때
    {
        parryTimer        = 0.0;
        warningLaserColor = COLOR_WARNING; // 기본값으로 리셋
    }


    // --- 쉴드 고정 타이머 업데이트 ---
    if (isShieldFrozen)
    {
        shieldFrozenTimer += dt;
        if (shieldFrozenTimer >= shieldFreezeDuration)
        {
            isShieldFrozen    = false; // 고정 해제
            shieldFrozenTimer = 0.0;
            Engine::GetLogger().LogEvent("Shield Unfrozen.");
        }
    }

    // --- 레이저 색상 및 상태 업데이트 ---
    if (isLaserOn)
    {
        if (prevLaserState == false) // 레이저가 방금 켜졌을 때
        {
            if (isParrying)
            {
                laserColor = COLOR_YELLOW;
                Engine::GetLogger().LogEvent("Laser turned YELLOW (Parry)");
                isShieldFrozen    = true;
                shieldFrozenTimer = 0.0;
                Engine::GetLogger().LogEvent("Shield Frozen!");
            }
            else
            {
                laserColor = COLOR_RED;
                Engine::GetLogger().LogEvent("Laser turned RED (Parry Failed)");
                if (CheckShieldCollision())
                {
                    targetShieldColor = CS200::unpack_color(COLOR_RED);
                    shieldHitTimer    = 0.0;
                    Engine::GetLogger().LogEvent("Shield hit by RED laser (Initial)!");
                }
            }
            isParrying = false; // 패링 상태는 레이저 켜지면 리셋
        }
        else if (laserColor == COLOR_RED) // 레이저가 이미 켜져 있고 빨간색일 때
        {
            if (CheckShieldCollision()) // 매 프레임 충돌 검사
            {
                // 아직 빨간색으로 변하는 중이 아니거나 이미 복구 중이었다면 다시 타이머 리셋
                if (shieldHitTimer >= shieldColorRecoveryTime || !(targetShieldColor[0] > 0.9f && targetShieldColor[1] < 0.1f && targetShieldColor[2] < 0.1f))
                {
                    targetShieldColor = CS200::unpack_color(COLOR_RED);
                    shieldHitTimer    = 0.0;
                }
            }
        }
    }
    else // 레이저가 꺼져 있을 때
    {
        if (prevLaserState == true) // 레이저가 방금 꺼졌을 때
        {
            laserColor = COLOR_RED; // 색상 리셋
            Engine::GetLogger().LogEvent("Laser turned OFF");
        }
        // --- 예고선 경로 계산 로직 (매 프레임) ---
        if (showingWarningLaser)
        {
            // 예고선은 항상 반사되어야 하므로, 쉴드를 포함하여 경로 계산
            std::vector<std::pair<Math::vec2, Math::vec2>> reflectionSegments;
            reflectionSegments.push_back({ shieldStart, shieldEnd }); // 현재 쉴드 위치 사용

            const auto windowSize = Engine::GetWindow().GetSize();
            Math::vec2 center     = { windowSize.x / 2.0, windowSize.y / 2.0 };
            Math::vec2 initialDir = (center - laserOrigin).Normalize();
            if (initialDir.Length() < std::numeric_limits<double>::epsilon())
            {
                initialDir = { 1.0, -1.0 };
                initialDir = initialDir.Normalize();
            }
            // Physics::CalculateLaserPath 직접 호출 (쉴드 포함)
            warningLaserPath = Physics::CalculateLaserPath(laserOrigin, initialDir, reflectionSegments);
            laserPath.clear(); // 주 레이저 경로는 비움 (레이저 꺼진 상태이므로)
        }
        else
        {
            warningLaserPath.clear();
        } // 예고 시간 아니면 경로 비움
    }

    // --- 쉴드 색상 복구 로직 ---
    UpdateShieldColor(dt);


    // --- 캐릭터 이동 및 실드 회전 (고정 상태 아닐 때만) ---
    if (!isShieldFrozen)
    {
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

        if (moveDir.Length() > std::numeric_limits<double>::epsilon())
        {
            Math::vec2 nextPos    = characterPos + moveDir.Normalize() * moveSpeed * dt;
            const auto windowSize = Engine::GetWindow().GetSize();
            nextPos.x             = std::clamp(nextPos.x, 0.0, static_cast<double>(windowSize.x));
            nextPos.y             = std::clamp(nextPos.y, 0.0, static_cast<double>(windowSize.y));
            characterPos          = nextPos;
        }

        const double rotateSpeed = PI;
        if (input.KeyDown(CS230::Input::Keys::Left))
            shieldAngle += rotateSpeed * dt;
        if (input.KeyDown(CS230::Input::Keys::Right))
            shieldAngle -= rotateSpeed * dt;
        shieldAngle = fmod(shieldAngle, 2.0 * PI);
        if (shieldAngle < 0)
            shieldAngle += 2.0 * PI;
    }


    // --- 상태 업데이트 및 계산 ---
    UpdateShield(); // 쉴드 위치는 매 프레임 업데이트
    if (isLaserOn)
    {
        CalculateLaser(laserColor == COLOR_YELLOW); // 주 레이저 경로 계산
        if (laserColor == COLOR_YELLOW)
        {
            for (TargetState& target : targets)
            {
                if (!target.hitByParriedLaser && CheckCollision(target))
                {
                    target.color             = COLOR_GREEN;
                    target.hitByParriedLaser = true;
                    Engine::GetLogger().LogEvent("Target hit by parried laser!");
                }
            }
        }
    }
    else if (!showingWarningLaser) // 레이저 꺼져있고 예고선도 없으면 주 레이저 경로 비움
    {
        laserPath.clear();
    }
}

// --- 쉴드 색상 업데이트 함수 ---
void DemoLaserReflection::UpdateShieldColor(double dt)
{
    bool isTargetRed = (targetShieldColor[0] > 0.9f && targetShieldColor[1] < 0.1f && targetShieldColor[2] < 0.1f);
    if (isTargetRed)
    {
        shieldHitTimer += dt;
        if (shieldHitTimer >= shieldColorRecoveryTime)
        {
            targetShieldColor = CS200::unpack_color(COLOR_CYAN);
        }
    }
    else
    {
        shieldHitTimer = shieldColorRecoveryTime;
        if (!(targetShieldColor[0] < 0.1f && targetShieldColor[1] > 0.9f && targetShieldColor[2] > 0.9f))
        {
            targetShieldColor = CS200::unpack_color(COLOR_CYAN);
        }
    }
    ease_color_to_target(currentShieldColor, targetShieldColor, dt, 5.0);
    shieldColor = CS200::pack_color(currentShieldColor);
}

// --- 쉴드 위치 업데이트 함수 ---
void DemoLaserReflection::UpdateShield()
{
    double dx   = (shieldLength / 2.0) * std::cos(shieldAngle);
    double dy   = (shieldLength / 2.0) * std::sin(shieldAngle);
    shieldStart = characterPos + Math::vec2{ dx, dy };
    shieldEnd   = characterPos - Math::vec2{ dx, dy };
}

// --- 레이저 경로 계산 함수 ---
void DemoLaserReflection::CalculateLaser(bool parrySuccess)
{
    std::vector<std::pair<Math::vec2, Math::vec2>> reflectionSegments;
    // 패링 성공 시(또는 예고선 계산 시)에만 쉴드 세그먼트를 반사 세그먼트 목록에 추가
    if (parrySuccess)
    {
        reflectionSegments.push_back({ shieldStart, shieldEnd });
    }

    const auto windowSize = Engine::GetWindow().GetSize();
    Math::vec2 center     = { windowSize.x / 2.0, windowSize.y / 2.0 };
    Math::vec2 initialDir = (center - laserOrigin).Normalize();

    if (initialDir.Length() < std::numeric_limits<double>::epsilon())
    {
        initialDir = { 1.0, -1.0 };
        initialDir = initialDir.Normalize();
    }
    // Physics::CalculateLaserPath 호출
    laserPath = Physics::CalculateLaserPath(laserOrigin, initialDir, reflectionSegments);
}

// --- 쉴드 충돌 검사 함수 ---
bool DemoLaserReflection::CheckShieldCollision() const
{
    const auto windowSize = Engine::GetWindow().GetSize();
    Math::vec2 center     = { windowSize.x / 2.0, windowSize.y / 2.0 };
    Math::vec2 initialDir = (center - laserOrigin).Normalize();
    if (initialDir.Length() < std::numeric_limits<double>::epsilon())
    {
        initialDir = { 1.0, -1.0 };
        initialDir = initialDir.Normalize();
    }

    Math::vec2 intersectionPoint;
    double     t;
    // 쉴드 세그먼트와 레이저의 첫 번째 충돌만 검사 (관통하기 전 경로 기준)
    return Physics::RaySegmentIntersection(laserOrigin, initialDir, shieldStart, shieldEnd, intersectionPoint, t);
}

// --- 타겟 충돌 검사 함수 ---
bool DemoLaserReflection::CheckCollision(const TargetState& target) const
{
    Math::vec2 intersectionPoint;
    for (const auto& segment : laserPath)
    {
        if (LineCircleIntersection(segment.first, segment.second, target.position, target.radius, intersectionPoint))
        {
            return true;
        }
    }
    return false;
}

// --- 그리기 함수 ---
void DemoLaserReflection::Draw() const
{
    CS200::RenderingAPI::Clear();
    auto&      renderer   = Engine::GetRenderer2D();
    const auto windowSize = Engine::GetWindow().GetSize();

    renderer.BeginScene(CS200::build_ndc_matrix(windowSize));

    renderer.DrawCircle(Math::TranslationMatrix(characterPos) * Math::ScaleMatrix(10.0), COLOR_WHITE);
    renderer.DrawLine(shieldStart, shieldEnd, shieldColor, 3.0); // shieldColor 사용
    for (const auto& target : targets)
    {
        renderer.DrawCircle(Math::TranslationMatrix(target.position) * Math::ScaleMatrix(target.radius), target.color);
    }

    // --- 예고선 그릴 때 warningLaserColor 사용 ---
    if (showingWarningLaser)
    {
        for (const auto& segment : warningLaserPath)
        {
            if (std::isfinite(segment.first.x) && std::isfinite(segment.first.y) && std::isfinite(segment.second.x) && std::isfinite(segment.second.y))
            {
                renderer.DrawLine(segment.first, segment.second, warningLaserColor, warningLaserWidth); // 수정됨
            }
        }
    }
    // --- 수정 끝 ---

    if (isLaserOn)
    {
        for (const auto& segment : laserPath)
        {
            if (std::isfinite(segment.first.x) && std::isfinite(segment.first.y) && std::isfinite(segment.second.x) && std::isfinite(segment.second.y))
            {
                renderer.DrawLine(segment.first, segment.second, laserColor, mainLaserWidth);
            }
        }
    }

    renderer.EndScene();
}

// --- ImGui 그리기 함수 ---
void DemoLaserReflection::DrawImGui()
{
    ImGui::Begin("Laser Reflection Info");
    // ... 캐릭터, 쉴드 정보 ...
    ImGui::Text("Character Position: (%.1f, %.1f)", characterPos.x, characterPos.y);
    ImGui::Text("Shield Angle (Degrees): %.1f", shieldAngle * 180.0 / PI);
    ImGui::Text("Shield Start: (%.1f, %.1f)", shieldStart.x, shieldStart.y);
    ImGui::Text("Shield End: (%.1f, %.1f)", shieldEnd.x, shieldEnd.y);
    ImVec4 currentShieldColorVec4 = ImVec4(currentShieldColor[0], currentShieldColor[1], currentShieldColor[2], currentShieldColor[3]);
    ImGui::ColorEdit4("Current Shield Color", (float*)&currentShieldColorVec4, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker);
    ImGui::Text("Shield Frozen: %s (%.1f / %.1f s)", isShieldFrozen ? "YES" : "NO", isShieldFrozen ? shieldFrozenTimer : 0.0f, shieldFreezeDuration);

    ImGui::Separator();
    // ... 레이저 정보 ...
    ImGui::Text("Laser State: %s", isLaserOn ? "ON" : (showingWarningLaser ? "WARNING" : "OFF"));
    ImGui::Text("Laser Origin: (%.1f, %.1f)", laserOrigin.x, laserOrigin.y);
    ImGui::Text("Laser Timer: %.2f / %.2f", fmod(laserTimer, laserCycleTime), laserCycleTime);
    const char* colorName = (laserColor == COLOR_RED) ? "RED" : (laserColor == COLOR_YELLOW ? "YELLOW" : "UNKNOWN");
    ImGui::Text("Laser Color: %s", colorName);
    // --- 예고선 색상 정보 추가 ---
    const char* warnColorName = (warningLaserColor == COLOR_WARNING) ? "RED" : ((warningLaserColor == COLOR_PARRY_WARNING) ? "CYAN" : "UNKNOWN");
    ImGui::Text("Warning Laser Color: %s", showingWarningLaser ? warnColorName : "N/A");
    ImGui::Text("Parry Window: %s", parryWindowActive ? "ACTIVE" : "INACTIVE");
    ImGui::Text("Parrying: %s", isParrying ? "YES (Input success)" : "NO");

    // ... 타겟 정보 및 리셋 버튼 ...
    ImGui::SeparatorText("Targets Info");
    for (size_t i = 0; i < targets.size(); ++i)
    {
        const auto& target = targets[i];
        ImGui::PushID(static_cast<int>(i)); // 각 타겟 구분을 위한 ID 푸시
        const char* targetColorName = (target.color == COLOR_RED) ? "RED" : (target.color == COLOR_GREEN ? "GREEN" : "UNKNOWN");
        ImGui::Text("Target %zu Position: (%.1f, %.1f)", i + 1, target.position.x, target.position.y);
        ImGui::Text("Target %zu Color: %s", i + 1, targetColorName);
        ImGui::Text("Target %zu Hit: %s", i + 1, target.hitByParriedLaser ? "YES" : "NO");
        ImGui::PopID(); // ID 팝
        if (i < targets.size() - 1)
            ImGui::Separator(); // 타겟 사이에 구분선
    }
    if (ImGui::Button("Reset Targets")) // 버튼 이름 변경
    {
        for (TargetState& target : targets) // 모든 타겟 리셋
        {
            target.color             = COLOR_RED;
            target.hitByParriedLaser = false;
        }
        Engine::GetLogger().LogEvent("All Targets Reset!");
    }

    // ... 레이저 경로 정보 ...
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
    ImGui::Text("Warning Laser Path Segments: %zu", warningLaserPath.size());
    if (ImGui::TreeNode("Warning Path Details"))
    {
        for (size_t i = 0; i < warningLaserPath.size(); ++i)
        {
            ImGui::Text("Seg %zu: (%.1f, %.1f) -> (%.1f, %.1f)", i, warningLaserPath[i].first.x, warningLaserPath[i].first.y, warningLaserPath[i].second.x, warningLaserPath[i].second.y);
        }
        ImGui::TreePop();
    }

    ImGui::End();
}