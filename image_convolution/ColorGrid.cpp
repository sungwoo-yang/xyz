// Copyright (C) 2018 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the prior
// written consent of DigiPen Institute of Technology is prohibited.
//
// File Name: ColorGrid.cpp
// Purpose:   Represent a collection of RGB values
// Language:  C++
// Platform:  Visual Studio 15.8.2 w/ C++17, g++-8 (Ubuntu 8.2.0-1ubuntu2~18.04) 8.2.0
// Project:   image convolution thread
// Author:    Rudy Castan <rudy.castan@digipen.edu>
// Creation date: 2018/10/29

#include "ColorGrid.hpp"
#include "lodepng.h"
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace CS180
{
    ColorGrid::ColorGrid(unsigned the_width, unsigned the_height) : colors(the_width * the_height), width(the_width), height(the_height) {}

    Color3D& ColorGrid::operator()(int row, int column)
    {
        assert(column >= 0);
        assert(column < int(width));
        assert(row >= 0);
        assert(row < int(height));
        return colors[row * width + column];
    }

    Color3D ColorGrid::operator()(int row, int column) const
    {
        assert(column >= 0);
        assert(column < int(width));
        assert(row >= 0);
        assert(row < int(height));
        return colors[row * width + column];
    }

    ColorGrid load_color_grid_from_png_file_path(const std::string& file_path)
    {
        ColorGrid grid;
        std::vector<unsigned char> image;
        const auto error = lodepng::decode(image, grid.width, grid.height, file_path);
        if (error)
        {
            const auto error_msg = lodepng_error_text(error);
            std::cerr << "decoder error with " << file_path << "\n" << error << ": " << error_msg << std::endl;
            throw std::runtime_error(error_msg);
        }

        grid.colors.reserve(grid.width * grid.height);
        for (unsigned i = 0; i < image.size(); i += 4) // note using i+=4 because lodepng returns rgba chunks
        {
            grid.colors.emplace_back(image[i + 0] / 255.0, image[i + 1] / 255.0, image[i + 2] / 255.0);
        }

        return grid;
    }

    void save_color_grid_to_png_file(const ColorGrid& grid, const std::string& file_path)
    {
        std::vector<unsigned char> image;
        image.reserve(grid.width * grid.height * 4); // 4, because lodepng expects rgba chunks
        for (const auto color : grid.colors)
        {
            const auto red = static_cast<unsigned char>(color.red * 255);
            const auto green = static_cast<unsigned char>(color.green * 255);
            const auto blue = static_cast<unsigned char>(color.blue * 255);
            constexpr unsigned char alpha = 255;
            image.push_back(red);
            image.push_back(green);
            image.push_back(blue);
            image.push_back(alpha);
        }

        const auto error = lodepng::encode(file_path, image, grid.width, grid.height);
        if (error)
        {
            const auto error_msg = lodepng_error_text(error);
            std::cerr << "encoder error with " << file_path << "\n" << error << ": " << error_msg << std::endl;
            throw std::runtime_error(error_msg);
        }
    }
}
