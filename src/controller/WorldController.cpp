//
// Created by fran on 11/03/2026.
//

#include "WorldController.hpp"

WorldController::WorldController(sf::Vector2u minWindowSize, sf::Vector2u maxWindowSize)
    : m_minWindowSize(minWindowSize)
    , m_maxWindowSize(maxWindowSize)
    , m_currentDrawMode(DrawMode::WIREFRAME_SHADED)
{
}

void WorldController::init(const std::string &mapName, 
        const CameraSettings& cameraSettings, const ViewSettings& viewSettings)
{
    const float globalToolBoxOffset = 550;
    const sf::Vector2f globalUIPosition{static_cast<float>(viewSettings.windowSize.x) / 2.f - globalToolBoxOffset, 10};
    sf::Vector2f quitMenuPosition = sf::Vector2f(viewSettings.windowSize.x / 2.f, viewSettings.windowSize.y / 2.f) - sf::Vector2f(200, 150);
    m_worldMenu = std::make_unique<WorldMenu>(globalUIPosition, quitMenuPosition);
    m_worldMenu->setQuitMenuVisibility(false);
    m_worldMenu->setDrawModeButtonOnClickCallback(DrawMode::SHADED, [this] () { this->onDrawModeButtonClicked(DrawMode::SHADED); } );
    m_worldMenu->setDrawModeButtonOnClickCallback(DrawMode::WIREFRAME, [this] () { this->onDrawModeButtonClicked(DrawMode::WIREFRAME); } );
    m_worldMenu->setDrawModeButtonOnClickCallback(DrawMode::WIREFRAME_SHADED, [this] () { this->onDrawModeButtonClicked(DrawMode::WIREFRAME_SHADED); } );
    m_worldMenu->selectDrawModeButton(DrawMode::WIREFRAME_SHADED);

    m_editionController = std::make_unique<EditionController>(m_worldModel, m_worldView, globalUIPosition + sf::Vector2f(5, 5));
    sf::Vector2f brushMenuPosition = globalUIPosition + sf::Vector2f(245, 0); 
    m_brushController = std::make_unique<BrushController>(brushMenuPosition);
    m_navigationController = std::make_unique<NavigationController>(m_worldModel, m_worldView, globalUIPosition + sf::Vector2f(5, 5));
    m_mapLoadSaveController = std::make_unique<MapLoadSaveController>(&m_worldModel, &m_worldView, m_editionController.get(), brushMenuPosition + sf::Vector2f(300, 0));

    m_worldModel.loadMap(mapName);
    sf::Vector2i tilesSize = m_worldModel.getTilesSize();
    m_worldView.init(viewSettings.center, viewSettings.size, 10);
    m_worldView.initCamera(tilesSize.x, tilesSize.y, cameraSettings.heightScale,
        cameraSettings.projectionAngleX, cameraSettings.projectionAngleY, m_worldModel.getCenter());
    m_worldView.initTileMap(m_worldModel.getTiles());
    m_worldView.initBrushes(m_brushController->getBrushesImagePaths());
    m_worldView.initSplatmap(m_worldModel.getSplatmapFilepath(), sf::Vector2i(tilesSize.x, tilesSize.y), m_worldModel.getMapSize().x, m_worldModel.getMapSize().y);
    m_worldView.initEnvironment(viewSettings.windowSize);
}

void WorldController::handleEvents(const sf::Event &event, sf::RenderWindow &window)
{
    // navigation events like zoom and rotate will only happen when we're not in edition mode
    m_navigationController->handleEvents(window, event, m_worldModel, m_worldView, m_editionController->isEditing());
    m_brushController->handleEvents(window, event);
    m_editionController->handleEvents(window, event, m_worldModel, m_worldView, *m_brushController);
    m_mapLoadSaveController->handleEvents(event, window);
}

void WorldController::handleContinuousEvents(const float deltaTime, const sf::RenderWindow &window)
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
        m_brushController->draw(window, m_worldView.getCamera(), m_editionController->areEditableTilesVisible());
}

void WorldController::onWindowResized(const sf::Vector2u windowSize, sf::RenderWindow& window)
{
    unsigned int clampedWidth = std::clamp(windowSize.x, m_minWindowSize.x, m_maxWindowSize.x);
    unsigned int clampedHeight = std::clamp(windowSize.y, m_minWindowSize.y, m_maxWindowSize.y);
    
    if (clampedWidth != windowSize.x || clampedHeight != windowSize.y)
        window.setSize(sf::Vector2u(clampedWidth, clampedHeight));
    m_worldView.onWindowResized(windowSize);
}

void WorldController::setSaveMapButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_worldMenu->setSaveMapButtonOnClickCallback(callback);
}

void WorldController::setDontSaveButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_worldMenu->setDontSaveButtonOnClickCallback(callback);
}

void WorldController::setCancelButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_worldMenu->setCancelButtonOnClickCallback(callback);
}

void WorldController::setQuitMenuVisibility(const bool isVisible) const
{
    m_worldMenu->setQuitMenuVisibility(isVisible);
}

bool WorldController::isQuitMenuVisible() const
{
    return m_worldMenu->isQuitMenuVisible();
}

void WorldController::saveMapToFile() const
{
    m_mapLoadSaveController->saveMapToFile();
}

void WorldController::onDrawModeButtonClicked(const DrawMode mode)
{
    if (mode == m_currentDrawMode)
        return;
    m_worldMenu->unselectDrawModeButton(m_currentDrawMode);
    switch (mode) {
        case DrawMode::WIREFRAME_SHADED:
            m_worldView.setAreShadedTilesVisible(true);
            m_worldView.setIsWireframeVisible(true);
            break;
        case DrawMode::WIREFRAME:
            m_worldView.setAreShadedTilesVisible(false);
            m_worldView.setIsWireframeVisible(true);
            break;
        case DrawMode::SHADED:
            m_worldView.setAreShadedTilesVisible(true);
            m_worldView.setIsWireframeVisible(false);
            break;
    }
    m_currentDrawMode = mode;
    m_worldMenu->selectDrawModeButton(m_currentDrawMode);
}
