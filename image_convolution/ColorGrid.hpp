// Copyright (C) 2018 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the prior
// written consent of DigiPen Institute of Technology is prohibited.
//
// File Name: ColorGrid.hpp
// Purpose:   Represent a collection of RGB values
// Language:  C++
// Platform:  Visual Studio 15.8.2 w/ C++17, g++-8 (Ubuntu 8.2.0-1ubuntu2~18.04) 8.2.0
// Project:   image convolution thread
// Author:    Rudy Castan <rudy.castan@digipen.edu>
// Creation date: 2018/10/29
#pragma once

#include "Color3D.hpp"
#include <string>
#include <vector>

namespace CS180
{
    struct ColorGrid
    {
        std::vector<Color3D> colors{};
        unsigned width{0};
        unsigned height{0};

        ColorGrid() = default;
        ColorGrid(unsigned the_width, unsigned the_height);

        Color3D& operator()(int row, int column);
        Color3D operator()(int row, int column) const;
    };

    ColorGrid load_color_grid_from_png_file_path(const std::string& file_path);
    void save_color_grid_to_png_file(const ColorGrid& grid, const std::string& file_path);
}