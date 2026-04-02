//
// Created by fran on 11/03/2026.
//

#include "WorldController.hpp"

WorldController::WorldController()
{
}

void WorldController::init(const std::string &mapName, 
        const CameraSettings& cameraSettings, const ViewSettings& viewSettings)
{
    m_worldModel.loadMap(mapName);
    m_worldView.init(viewSettings.center, viewSettings.size);
    m_worldView.initCamera(cameraSettings.tileSizeX, cameraSettings.tileSizeY, cameraSettings.heightScale,
        cameraSettings.projectionAngleX, cameraSettings.projectionAngleY, m_worldModel.getCenter());
    m_worldView.initTileMap(m_worldModel.getTiles());
    m_worldView.initEnvironment(viewSettings.windowSize);
    m_worldView.zoom(10);
}

void WorldController::handleEvents(sf::RenderWindow &window)
{
    sf::Event event;
    m_navigationController.resetKeyPressedEvents();
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed 
            || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
            window.close();
        m_editionController.handleEvents(window, event, m_worldModel, m_worldView);
        // navigation events like zoom and rotate will only happen when we're not in edition mode
        m_navigationController.handleEvents(window, event, m_worldModel, m_worldView, m_editionController.isEditing());
    }
    m_navigationController.handleContinuousEvents(m_worldView);
    m_editionController.handleContinuousEvents(window, m_worldModel, m_worldView);
}

void WorldController::update(const float deltaTime, sf::RenderWindow &window)
{
    m_worldView.update(deltaTime, m_worldModel.getTiles(), window);
    m_editionController.update(deltaTime, window, m_worldModel, m_worldView);
}

void WorldController::draw(sf::RenderWindow &window)
{
    m_worldView.draw(window);
    m_editionController.draw(window, m_worldView.getCamera());
}