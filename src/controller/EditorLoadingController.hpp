//
// Created by fran on 30/04/2026.
//

#ifndef LANDCRAFT_EDITORLOADINGCONTROLLER_HPP
#define LANDCRAFT_EDITORLOADINGCONTROLLER_HPP

#include "WorldController.hpp"
#include "../utils/ResourceManager.hpp"

class EditorLoadingController {
public:
    bool initializeWorld(sf::RenderWindow& window,
        const sf::Vector2u& windowSize,
        std::string mapName,
        WorldController& worldController,
        const CameraSettings& cameraSettings,
        const ViewSettings& viewSettings) const;

private:
    bool drawLoadingScreen(sf::RenderWindow& window, const sf::Vector2u& windowSize, const std::string& message) const;
};

#endif //LANDCRAFT_EDITORLOADINGCONTROLLER_HPP
