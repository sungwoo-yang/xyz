/**
 * \file
 * \author [Your Name/Modified By]
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 * \brief Implements the DashComponent logic.
 */
#include "Dash.hpp"

namespace CS230
{
    void DashComponent::UpdateTimers(double delta_time)
    {
        // 쿨다운 타이머 감소
        if (dashCooldownTimer > 0.0)
        {
            dashCooldownTimer -= delta_time;
        }

        // 대시 지속 시간 타이머 감소 및 상태 종료
        if (isDashing)
        {
            dashTimer -= delta_time;
            if (dashTimer <= 0.0)
            {
                isDashing = false;
            }
        }
    }

    bool DashComponent::TryStartDash(const Input& input, bool currentFaceRight)
    {
        if (input.KeyJustPressed(dashKey) && // 대시 키를 방금 눌렀고
            dashCooldownTimer <= 0.0 &&      // 쿨다운이 끝났으며
            !isDashing)                      // 현재 대시 중이 아닐 때
        {
            isDashing         = true;
            dashTimer         = dashDuration;
            dashCooldownTimer = dashCooldown;              // 쿨다운 시작
            dashDirection     = currentFaceRight ? 1 : -1; // 현재 바라보는 방향으로 설정
            return true;
        }
        return false;
    }

    double DashComponent::GetDashVelocityX() const
    {
        if (isDashing)
        {
            return dashSpeed * dashDirection;
        }
        return 0.0;
    }

    bool DashComponent::IsDashing() const
    {
        return isDashing;
    }

} // namespace CS230