//
// Created by fran on 06/05/2026.
//

#ifndef LANDCRAFT_FILEUTILS_HPP
#define LANDCRAFT_FILEUTILS_HPP

#include "../../lib/portable-file-dialogs.h"

namespace FileUtils {
    std::string getFileToOpenPathFromFileDialog(const std::vector<std::string> &filters = {
        "All Files", "*" 
        "Legend Files", "*.legend"});

    std::string getFileToSavePathFromFileDialog(const std::vector<std::string> &filters = {
        "All Files", "*",
        "Legend Files", "*.legend"
    });
};


#endif //LANDCRAFT_FILEUTILS_HPP