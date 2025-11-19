// Copyright (C) 2018 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the prior
// written consent of DigiPen Institute of Technology is prohibited.
//
// File Name: ConvolutePNG.hpp
// Purpose:   Given a source PNG file, will generate all other convolution patterns and save them to png files.
// Language:  C++
// Platform:  Visual Studio 15.8.2 w/ C++17, g++-8 (Ubuntu 8.2.0-1ubuntu2~18.04) 8.2.0
// Project:   image convolution thread
// Author:    Rudy Castan <rudy.castan@digipen.edu>
// Creation date: 2018/10/29

#pragma once

#include <filesystem>

namespace CS180
{

    void generate_convolutions_and_their_png_files(const std::filesystem::path& source_png_file, const std::filesystem::path& destination_folder = ".");
}
