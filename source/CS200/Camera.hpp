/**
 * \file Camera.hpp
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Engine/Matrix.hpp"
#include "Engine/Vec2.hpp"

namespace CS200
{
    enum class CameraMode
    {
        ThirdPerson_Follow,
        FirstPerson_View
    };

    class Camera
    {
    public:
        Math::vec2 position{ 0.0, 0.0 };
        double     rotation{ 0.0 };
        double     zoom{ 1.0 };
        CameraMode mode{ CameraMode::ThirdPerson_Follow };

        Math::TransformationMatrix GetViewMatrix() const;
    };
}