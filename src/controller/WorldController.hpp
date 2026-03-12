//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_WORLDCONTROLLER_HPP
#define LANDCRAFT_WORLDCONTROLLER_HPP

#include "../model/WorldModel.hpp"
#include "../view/WorldView.hpp"

struct CameraSettings {
    float tileSizeX;
    float tileSizeY;
    float heightScale;
    float projectionAngleX;
    float projectionAngleY;
};

struct ViewSettings {
    sf::Vector2f center;
    sf::Vector2f size;
};

class WorldController 
{
public:
    WorldController();
    void init(const std::string &mapName, 
        const CameraSettings& cameraSettings, const ViewSettings& viewSettings);
    void handleEvents(sf::RenderWindow& window);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
private:
    void handlePanEvents(sf::RenderWindow& window, const sf::Event &event);
    void handleRotationEvents(sf::RenderWindow& window, const sf::Event &event);
    void handleZoomEvents(sf::RenderWindow& window, const sf::Event &event);
    void handleMapEditingEvents(sf::RenderWindow& window, const sf::Event &event);
    // add event for turn on/off wireframe, shaded mode,


    WorldModel m_worldModel;
    WorldView m_worldView;

    float m_movementStep;
    int m_zoomStep;
};


#endif //LANDCRAFT_WORLDCONTROLLER_HPP