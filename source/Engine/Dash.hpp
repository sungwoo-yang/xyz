#pragma once

#include "Input.hpp" // Input 키 사용을 위해 포함
#include "Vec2.hpp"

namespace CS230
{
    struct DashComponent
    {
        // Dash 상태
        bool   isDashing         = false; // 현재 대시 중인지 여부
        double dashTimer         = 0.0;   // 남은 대시 시간
        double dashCooldownTimer = 0.0;   // 남은 쿨다운 시간
        int    dashDirection     = 1;     // 대시 방향 (1: 오른쪽, -1: 왼쪽)

        // Dash 설정 값
        double      dashSpeed            = 800.0;          // 대시 속도 (pixels/sec)
        double      dashDuration         = 0.15;           // 대시 지속 시간 (sec)
        double      dashCooldown         = 0.5;            // 대시 쿨다운 시간 (sec)
        bool        disableGravityOnDash = true;           // 대시 중 중력 무시 여부
        Input::Keys dashKey              = Input::Keys::K; // 대시 키 (TODO: Shift로 변경 필요)

        /**
         * @brief 매 프레임 대시 상태를 업데이트합니다 (쿨다운, 지속 시간).
         * @param delta_time 프레임 간 시간 간격
         */
        void UpdateTimers(double delta_time);

        /**
         * @brief 대시 시작 가능 여부를 확인하고, 가능하다면 대시 상태를 시작합니다.
         * @param input 현재 입력 상태
         * @param currentFaceRight 캐릭터가 현재 오른쪽을 보는지 여부
         * @return 대시를 시작했으면 true, 아니면 false
         */
        bool TryStartDash(const Input& input, bool currentFaceRight);

        /**
         * @brief 현재 대시 상태에 따른 X축 이동 속도를 반환합니다.
         * @return 대시 중이면 대시 속도 * 방향, 아니면 0.0
         */
        double GetDashVelocityX() const;

        /**
         * @brief 현재 대시 중인지 여부를 반환합니다.
         * @return 대시 중이면 true, 아니면 false
         */
        bool IsDashing() const;
    };

} // namespace CS230