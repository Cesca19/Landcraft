//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_WORLDCONTROLLER_HPP
#define LANDCRAFT_WORLDCONTROLLER_HPP

#include "../model/WorldModel.hpp"
#include "../view/WorldView.hpp"
#include "EditionController.hpp"
#include "NavigationController.hpp"


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
    sf::Vector2u windowSize;
};

class WorldController 
{
public:
    WorldController();
    void init(const std::string &mapName, 
        const CameraSettings& cameraSettings, const ViewSettings& viewSettings);
    void handleEvents(sf::RenderWindow& window);
    void update(float deltaTime, sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
private:
    // add event for turn on/off wireframe, shaded mode,

    WorldModel m_worldModel;
    WorldView m_worldView;
    EditionController m_editionController;
    NavigationController m_navigationController;
};


#endif //LANDCRAFT_WORLDCONTROLLER_HPP