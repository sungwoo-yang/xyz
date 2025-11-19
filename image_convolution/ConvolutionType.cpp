// Copyright (C) 2018 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the prior
// written consent of DigiPen Institute of Technology is prohibited.
//
// File Name: ConvolutionType.cpp
// Purpose:   Defines the different types of convolutions
// Language:  C++
// Platform:  Visual Studio 15.8.2 w/ C++17, g++-8 (Ubuntu 8.2.0-1ubuntu2~18.04) 8.2.0
// Project:   image convolution thread
// Author:    Rudy Castan <rudy.castan@digipen.edu>
// Creation date: 2018/10/29

#include "ConvolutionType.hpp"

namespace CS180
{
    ConvolutionType& operator++(ConvolutionType& mode)
    {
        mode = static_cast<ConvolutionType>(mode + 1);
        return mode;
    }

    const ConvolutionMatrix CONVOLUTIONS[] = {
        {{0.000, 0.000, 0.000}, {0.000, 9.000, 0.000}, {0.000, 0.000, 0.000}}, {{1.000, 1.000, 1.000}, {1.000, 1.000, 1.000}, {1.000, 1.000, 1.000}},
        {{0.000, -3.00, 0.000}, {-3.00, 21.00, -3.00}, {0.000, -3.00, 0.000}}, {{-18.0, -9.00, 0.000}, {-9.00, 9.000, 9.000}, {0.000, 9.000, 18.00}},
        {{0.000, 0.000, 0.000}, {0.000, 12.00, 0.000}, {0.000, 0.000, 0.000}}, {{0.000, 0.000, 0.000}, {0.000, 6.000, 0.000}, {0.000, 0.000, 0.000}},
        {{0.000, 9.000, 0.000}, {9.000, -36.0, 9.000}, {0.000, 9.000, 0.000}}};

    const char* const CONVOLUTION_NAMES[] = {"Identity", "Blur", "Sharpen", "Emboss", "Lighten", "Darken", "EdgeDetect"};
}