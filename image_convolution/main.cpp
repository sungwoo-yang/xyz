// Copyright (C) 2018 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the prior
// written consent of DigiPen Institute of Technology is prohibited.
//
// File Name: main.cpp
// Purpose:   entry point for image manipulation
// Language:  C++
// Platform:  Visual Studio 15.8.2 w/ C++17, g++-8 (Ubuntu 8.2.0-1ubuntu2~18.04) 8.2.0
// Project:   image convolution thread
// Author:    Rudy Castan <rudy.castan@digipen.edu>
// Creation date: 2018/10/29

#include <filesystem>
#include <iostream>
#include <vector>
#include "ConvolutePNG.hpp"

namespace
{
    struct InputParameters
    {
        int statusCode = 0;
        std::vector<std::filesystem::path> pngPaths{};
        std::filesystem::path destinationPath;
    };
    InputParameters extract_input_from_command_line(int argc, char* argv[]);
}

int main(int argc, char* argv[])
{
    // https://en.cppreference.com/w/cpp/language/structured_binding
    auto [status_code, png_paths, destination_path] = extract_input_from_command_line(argc, argv);
    if (status_code != 0)
        return status_code;

    //TODO: for each png run in it's own process
    for (const auto& path : png_paths )
    {
        CS180::generate_convolutions_and_their_png_files(path, destination_path);
    }

    return 0;
}

namespace
{
    void print_usage(char* argv[]) { std::cerr << "usage: " << argv[0] << " file_1.png file_2.png file_n.png [destination_folder]\n"; }

    InputParameters extract_input_from_command_line(int argc, char* argv[])
    {
        InputParameters input_values;
        input_values.destinationPath = std::filesystem::current_path();
        if (argc == 1)
        {
            print_usage(argv);
            input_values.statusCode = 1;
            return input_values;
        }

        for (int i = 1; i < argc; ++i)
        {
            auto current_input_path = std::filesystem::path{argv[i]};
            if (!current_input_path.has_extension())
            {
                if (i + 1 < argc)
                {
                    print_usage(argv);
                    input_values.statusCode = 1;
                    return input_values;
                }
                else
                {
                    if (!exists(current_input_path))
                    {
                        std::cerr << "Error: The following path does not exist:\n\t" << argv[i] << "\n";
                        input_values.statusCode = 2;
                        return input_values;
                    }
                    input_values.destinationPath = std::move(current_input_path);
                }
            }
            else
            {
                if (current_input_path.extension() != ".png")
                {
                    std::cerr << "Error: The following file name does not end with png:\n\t" << argv[i] << "\n";
                    input_values.statusCode = 3;
                    return input_values;
                }
                input_values.pngPaths.emplace_back(std::move(current_input_path));
            }
        }

        return input_values;
    }
}
