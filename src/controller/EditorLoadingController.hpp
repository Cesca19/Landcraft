//
// Created by fran on 30/04/2026.
//

#ifndef LANDCRAFT_APPLOADINGCONTROLLER_HPP
#define LANDCRAFT_APPLOADINGCONTROLLER_HPP

#include "WorldController.hpp"

class EditorLoadingController {
public:
    bool initializeWorld(sf::RenderWindow& window,
        const sf::Vector2u& windowSize,
        WorldController& worldController,
        const CameraSettings& cameraSettings,
        const ViewSettings& viewSettings) const;

private:
    bool drawLoadingScreen(sf::RenderWindow& window, const sf::Vector2u& windowSize, const std::string& message) const;
};

#endif //LANDCRAFT_APPLOADINGCONTROLLER_HPP
