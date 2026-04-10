//
// Created by fran on 11/03/2026.
//

#include "WorldController.hpp"

WorldController::WorldController()
{
    const sf::Vector2f globalUIPosition(480, 0);
    Box *globalActions = UIFactory::createBox(globalUIPosition, {225, 85});
    globalActions->initColors(sf::Color(sf::Color(205, 185, 220)), sf::Color(255, 255, 255));

    m_editionController = std::make_unique<EditionController>(m_worldModel, m_worldView, globalUIPosition + sf::Vector2f(5, 0));
    m_selectionController = std::make_unique<SelectionController>();
    m_navigationController = std::make_unique<NavigationController>(m_worldModel, m_worldView, globalUIPosition + sf::Vector2f(5, 0));
}

void WorldController::init(const std::string &mapName, 
        const CameraSettings& cameraSettings, const ViewSettings& viewSettings)
{
    m_worldModel.loadMap(mapName);
    m_worldView.init(viewSettings.center, viewSettings.size, 10);
    m_worldView.initCamera(cameraSettings.tileSizeX, cameraSettings.tileSizeY, cameraSettings.heightScale,
        cameraSettings.projectionAngleX, cameraSettings.projectionAngleY, m_worldModel.getCenter());
    m_worldView.initTileMap(m_worldModel.getTiles());
    m_worldView.initEnvironment(viewSettings.windowSize);
}

void WorldController::handleEvents(const sf::Event &event, sf::RenderWindow &window)
{
    // navigation events like zoom and rotate will only happen when we're not in edition mode
    m_navigationController->handleEvents(window, event, m_worldModel, m_worldView, m_editionController->isEditing());
    m_editionController->handleEvents(window, event, m_worldModel, m_worldView, *m_selectionController);
}

void WorldController::handleContinuousEvents(float deltaTime, const sf::RenderWindow &window)
{
    m_navigationController->handleContinuousEvents(deltaTime, m_worldView);
    m_editionController->handleContinuousEvents(window, m_worldModel, m_worldView, *m_selectionController);
}

void WorldController::update(const float deltaTime, const sf::RenderWindow &window)
{
    m_worldView.update(deltaTime, m_worldModel.getTiles(), window);
    if (!m_editionController->isSelectionLocked())
        m_selectionController->update(deltaTime, window, m_editionController->getSelectionMode(),
            m_worldModel, m_worldView.getCamera());
}

void WorldController::draw(sf::RenderWindow &window) const
{
    m_worldView.draw(window);
    if (!m_worldView.getCamera().isRotating()) // only draw selection when not rotating to avoid visual clutter
        m_selectionController->draw(window, m_worldView.getCamera());
}

void WorldController::onWindowResized(const sf::Vector2u windowSize)
{
    m_worldView.onWindowResized(windowSize);
}
