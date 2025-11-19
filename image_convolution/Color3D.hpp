// Copyright (C) 2018 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the prior
// written consent of DigiPen Institute of Technology is prohibited.
//
// File Name: Color3D.hpp
// Purpose:   Represent RGB color with doubles
// Language:  C++
// Platform:  Visual Studio 15.8.2 w/ C++17, g++-8 (Ubuntu 8.2.0-1ubuntu2~18.04) 8.2.0
// Project:   image convolution thread
// Author:    Rudy Castan <rudy.castan@digipen.edu>
// Creation date: 2018/10/29
#pragma once

#include <algorithm>

namespace CS180
{
    struct Color3D
    {
        double red{0.0f}, green{0.0f}, blue{0.0f};

        Color3D() = default;
        Color3D(double r, double g, double b);

        void ClampColorChannelsFromZeroToOne();
    };

    inline Color3D::Color3D(double r, double g, double b) : red(r), green(g), blue(b) {}

    inline void Color3D::ClampColorChannelsFromZeroToOne()
    {
        red = std::max(std::min(red, 1.0), 0.0);
        green = std::max(std::min(green, 1.0), 0.0);
        blue = std::max(std::min(blue, 1.0), 0.0);
    }

    inline Color3D& operator*=(Color3D& color, double s)
    {
        color.red *= s;
        color.green *= s;
        color.blue *= s;
        return color;
    }

    inline Color3D operator*(const Color3D& color, double s)
    {
        auto result(color);
        result *= s;
        return result;
    }

    inline Color3D& operator+=(Color3D& color, const Color3D& rhs)
    {
        color.red += rhs.red;
        color.green += rhs.green;
        color.blue += rhs.blue;
        return color;
    }

    inline Color3D operator+(const Color3D& color, const Color3D& rhs)
    {
        auto result(color);
        result += rhs;
        return result;
    }

    inline Color3D& operator/=(Color3D& color, double s)
    {
        color.red /= s;
        color.green /= s;
        color.blue /= s;
        return color;
    }

    inline Color3D operator/(const Color3D& color, double s)
    {
        auto result(color);
        result /= s;
        return result;
    }
}
