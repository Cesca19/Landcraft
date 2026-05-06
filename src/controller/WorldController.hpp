//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_WORLDCONTROLLER_HPP
#define LANDCRAFT_WORLDCONTROLLER_HPP

#include "../model/WorldModel.hpp"
#include "../view/world/WorldView.hpp"
#include "EditionController.hpp"
#include "BrushController.hpp"
#include "NavigationController.hpp"
#include "MapLoadSaveController.hpp"


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
    void handleEvents(const sf::Event &event, sf::RenderWindow& window);
    void handleContinuousEvents(float deltaTime, const sf::RenderWindow& window);
    void update(float deltaTime, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;
    void onWindowResized(sf::Vector2u windowSize);
private:
    // add event for turn on/off wireframe, shaded mode,

    WorldView m_worldView;
    WorldModel m_worldModel;
    std::unique_ptr<EditionController> m_editionController;
    std::unique_ptr<BrushController> m_brushController;
    std::unique_ptr<NavigationController> m_navigationController;
    std::unique_ptr<MapLoadSaveController> m_mapLoadSaveController;
};


#endif //LANDCRAFT_WORLDCONTROLLER_HPP