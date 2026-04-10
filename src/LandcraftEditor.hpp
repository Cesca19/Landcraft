//
// Created by fran on 02/04/2026.
//

#ifndef LANDCRAFT_LANDCRAFTEDITOR_HPP
#define LANDCRAFT_LANDCRAFTEDITOR_HPP

#include "controller/WorldController.hpp"
#include "ui/UIController.hpp"
#include "ui/UIFactory.hpp"

class LandcraftEditor {
public:
    LandcraftEditor();
    void run();
private:
    void handleEvents();
    void handleContinuousEvents(float deltaTime);

    bool m_hasFocus;
    sf::Vector2u m_windowSize;
    sf::Vector2u m_viewSize;
    float m_tileSizeX;
    float m_tileSizeY;
    float m_heightScale;
    float m_projectionAngleX;
    float m_projectionAngleY;
    sf::Clock m_clock;
    sf::RenderWindow m_window;
    std::unique_ptr<WorldController> m_worldController;
    std::unique_ptr<UIController> m_uiController;
};



#endif //LANDCRAFT_LANDCRAFTEDITOR_HPP
