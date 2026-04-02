//
// Created by fran on 17/03/2026.
//

#ifndef LANDCRAFT_NAVIGATIONCONTROLLER_HPP
#define LANDCRAFT_NAVIGATIONCONTROLLER_HPP

#include "../view/WorldView.hpp"
#include "../model/WorldModel.hpp"

class NavigationController {
public:
    NavigationController();
    void handleEvents(const sf::RenderWindow& window, const sf::Event& event, WorldModel& model, WorldView& view, bool isEditing);
    void handleContinuousEvents(WorldView& view);
    void resetKeyPressedEvents();
private:
    void handlePanEvents(const sf::RenderWindow& window, const sf::Event &event, WorldView& view);
    void handleContinuousPanEvents(WorldView& view);
    void handleRotationEvents(const sf::RenderWindow& window, const sf::Event &event,  WorldModel& model, const WorldView& view);
    void handleZoomEvents(const sf::RenderWindow& window, const sf::Event &event, WorldView& view) const;

    float m_movementStep;
    int m_zoomStep;
    float m_pitchRotationStep;
    float m_yawRotationStep;
    bool m_isRotating;
    bool m_isNavigationKeyPressed;
};


#endif //LANDCRAFT_NAVIGATIONCONTROLLER_HPP