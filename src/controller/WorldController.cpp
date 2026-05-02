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
    float globalToolBoxOffset = 450;
    const sf::Vector2f globalUIPosition{static_cast<float>(viewSettings.windowSize.x) / 2.f - globalToolBoxOffset, 10};
    Box *globalActions = UIFactory::createBox(globalUIPosition, {225, 90});
    UIFactory::applyDefaultBoxStyle(globalActions);

    m_editionController = std::make_unique<EditionController>(m_worldModel, m_worldView, globalUIPosition + sf::Vector2f(5, 5));
    float selectionMenuOffset = 185;
    m_brushController = std::make_unique<BrushController>(sf::Vector2f{static_cast<float>(viewSettings.windowSize.x) / 2.f - selectionMenuOffset, 10});
    m_navigationController = std::make_unique<NavigationController>(m_worldModel, m_worldView, globalUIPosition + sf::Vector2f(5, 5));

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
    m_brushController->handleEvents(window, event);
    m_editionController->handleEvents(window, event, m_worldModel, m_worldView, *m_brushController);
}

void WorldController::handleContinuousEvents(float deltaTime, const sf::RenderWindow &window)
{
    m_navigationController->handleContinuousEvents(deltaTime, m_worldView);
    m_editionController->handleContinuousEvents(window, m_worldModel, m_worldView, *m_brushController);
}

void WorldController::update(const float deltaTime, const sf::RenderWindow &window)
{
    m_worldView.update(deltaTime, m_worldModel.getTiles(), window);
    if (!m_editionController->isSelectionLocked())
        m_brushController->update(deltaTime, window, m_editionController->getSelectionMode(),
            m_worldModel, m_worldView.getCamera());
}

void WorldController::draw(sf::RenderWindow &window) const
{
    m_worldView.draw(window);
    if (!m_worldView.getCamera().isRotating()) // only draw selection when not rotating to avoid visual clutter
        m_brushController->draw(window, m_worldView.getCamera());
}

void WorldController::onWindowResized(const sf::Vector2u windowSize)
{
    m_worldView.onWindowResized(windowSize);
}
