/**
 * \file Camera.cpp
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Camera.hpp"
#include <cmath>

namespace CS200
{
    Math::TransformationMatrix Camera::GetViewMatrix() const
    {
        Math::ScaleMatrix scaleM(zoom);
        Math::TranslationMatrix transM(-position);

        if (mode == CameraMode::FirstPerson_View)
        {
            Math::RotationMatrix rotM(-rotation);
            return scaleM * rotM * transM;
        }
        else
        {
            return scaleM * transM;
        }
    }
}
