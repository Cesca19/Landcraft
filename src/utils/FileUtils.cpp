//
// Created by fran on 06/05/2026.
//

#include "FileUtils.hpp"

namespace FileUtils {
    std::string getFileToOpenPathFromFileDialog(const std::vector<std::string> &filters)
    {
        auto result = pfd::open_file("Select a file", ".", filters).result();
        if (!result.empty()) {
            std::cout << "Selected file: " << result[0] << std::endl;
            return result[0];
        }
        return "";
    }

    std::string getFileToSavePathFromFileDialog(const std::vector<std::string> &filters)
    {
        auto destination = pfd::save_file("Save file as", ".", filters).result();
        if (!destination.empty()) {
            std::cout << "Selected save location: " << destination << std::endl;
            return destination;
        }
        return "";
    }
}