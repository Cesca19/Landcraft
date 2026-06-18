//
// Created by fran on 03/03/2026.
//

#include <exception>
#include <iostream>
#include "LandcraftEditor.hpp"

int main(int ac, char** av)
{
    try {
        std::string mapName;
        if (ac > 1)
            mapName = av[1];
        LandcraftEditor m_editor(mapName);
        m_editor.run();
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << std::endl;
        return 1;
    }
    return 0;
}
