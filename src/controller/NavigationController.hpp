//
// Created by fran on 17/03/2026.
//

#ifndef LANDCRAFT_NAVIGATIONCONTROLLER_HPP
#define LANDCRAFT_NAVIGATIONCONTROLLER_HPP

#include "../view/WorldView.hpp"
#include "../model/WorldModel.hpp"
#include "../ui/UIFactory.hpp"

class NavigationController {
public:
    NavigationController(WorldModel &model, WorldView &view, sf::Vector2f globalUIPosition);
    ~NavigationController();
    void handleEvents(const sf::RenderWindow& window, const sf::Event& event, WorldModel& model, WorldView& view, bool isEditing);
    void handleContinuousEvents(float deltaTime, WorldView& view) const;
private:
    void handlePanEvents(const sf::RenderWindow& window, const sf::Event &event, WorldView& view);
    void handleContinuousPanEvents(float deltaTime, WorldView& view) const;
    void handleRotationEvents(const sf::RenderWindow& window, const sf::Event &event,  WorldModel& model, const WorldView& view);
    void handleZoomEvents(const sf::RenderWindow& window, const sf::Event &event, WorldView& view) const;

    float m_movementStep;
    int m_zoomStep;
    float m_pitchRotationStep;
    float m_yawRotationStep;
    SpriteButton *m_recenterViewButton;
};


#endif //LANDCRAFT_NAVIGATIONCONTROLLER_HPP