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

void WorldController::handleEvents(const sf::Event &event, sf::RenderWindow &window)
{
    // navigation events like zoom and rotate will only happen when we're not in edition mode
    m_navigationController.handleEvents(window, event, m_worldModel, m_worldView, m_editionController.isEditing());
    m_editionController.handleEvents(window, event, m_worldModel, m_worldView, m_selectionController);
}

void WorldController::handleContinuousEvents(float deltaTime, const sf::RenderWindow &window)
{
    m_navigationController.handleContinuousEvents(deltaTime, m_worldView);
    m_editionController.handleContinuousEvents(window, m_worldModel, m_worldView, m_selectionController);
}

void WorldController::update(const float deltaTime, const sf::RenderWindow &window)
{
    m_worldView.update(deltaTime, m_worldModel.getTiles(), window);
    if (!m_editionController.isSelectionLocked())
        m_selectionController.update(deltaTime, window, m_editionController.getSelectionMode(),
            m_worldModel, m_worldView.getCamera());
}

void WorldController::draw(sf::RenderWindow &window)
{
    m_worldView.draw(window);
    if (!m_worldView.getCamera().isRotating()) // only draw selection when not rotating to avoid visual clutter
        m_selectionController.draw(window, m_worldView.getCamera());
}