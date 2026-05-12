//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_WORLDCONTROLLER_HPP
#define LANDCRAFT_WORLDCONTROLLER_HPP

#include "../model/WorldModel.hpp"
#include "../view/world/WorldView.hpp"
#include "../view/menu/WorldMenu.hpp"
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
    WorldController(sf::Vector2u minWindowSize, sf::Vector2u maxWindowSize);
    void init(const std::string &mapName, 
        const CameraSettings& cameraSettings, const ViewSettings& viewSettings);
    void handleEvents(const sf::Event &event, sf::RenderWindow& window);
    void handleContinuousEvents(float deltaTime, const sf::RenderWindow& window);
    void update(float deltaTime, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;
    void onWindowResized(sf::Vector2u windowSize, sf::RenderWindow& window);
    void setSaveMapButtonOnClickCallback(const std::function<void()> &callback) const;
    void setDontSaveButtonOnClickCallback(const std::function<void()> &callback) const;
    void setCancelButtonOnClickCallback(const std::function<void()> &callback) const;
    void setQuitMenuVisibility(bool isVisible) const;
    bool isQuitMenuVisible() const;
    void saveMapToFile() const;
private:
    void onDrawModeButtonClicked(DrawMode mode);

    sf::Vector2u m_minWindowSize;
    sf::Vector2u m_maxWindowSize;
    DrawMode m_currentDrawMode;
    WorldView m_worldView;
    WorldModel m_worldModel;
    std::unique_ptr<WorldMenu> m_worldMenu;
    std::unique_ptr<EditionController> m_editionController;
    std::unique_ptr<BrushController> m_brushController;
    std::unique_ptr<NavigationController> m_navigationController;
    std::unique_ptr<MapLoadSaveController> m_mapLoadSaveController;
};


#endif //LANDCRAFT_WORLDCONTROLLER_HPP