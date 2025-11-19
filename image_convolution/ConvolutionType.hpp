// Copyright (C) 2018 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the prior
// written consent of DigiPen Institute of Technology is prohibited.
//
// File Name: ConvolutionType.hpp
// Purpose:   Defines the different types of convolutions
// Language:  C++
// Platform:  Visual Studio 15.8.2 w/ C++17, g++-8 (Ubuntu 8.2.0-1ubuntu2~18.04) 8.2.0
// Project:   image convolution thread
// Author:    Rudy Castan <rudy.castan@digipen.edu>
// Creation date: 2018/10/29

#pragma once

namespace CS180
{
    // http://beej.us/blog/data/convolution-image-processing/
    enum ConvolutionType : int
    {
        Identity = 0,
        Blur,
        Sharpen,
        Emboss,
        Lighten,
        Darken,
        EdgeDetect,
        Length
    };

    ConvolutionType& operator++(ConvolutionType& mode);

    using ConvolutionMatrix = double[3][3];
    extern const ConvolutionMatrix CONVOLUTIONS[ConvolutionType::Length];
    extern const char* const CONVOLUTION_NAMES[ConvolutionType::Length];
}
