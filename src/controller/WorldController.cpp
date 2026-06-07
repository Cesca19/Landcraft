//
// Created by fran on 11/03/2026.
//

#include "WorldController.hpp"

WorldController::WorldController(sf::Vector2u minWindowSize, sf::Vector2u maxWindowSize)
    : m_minWindowSize(minWindowSize)
    , m_maxWindowSize(maxWindowSize)
    , m_currentDrawMode(DrawMode::WIREFRAME_SHADED)
    , waterLevelIncrementStep(0.25f)
{
}

void WorldController::init(const std::string &mapName, 
        const CameraSettings& cameraSettings, const ViewSettings& viewSettings)
{
    constexpr float globalToolBoxOffset = 800;
    const sf::Vector2f globalUIPosition{static_cast<float>(viewSettings.windowSize.x) / 2.f - globalToolBoxOffset, 10};
    sf::Vector2f quitMenuPosition = sf::Vector2f(viewSettings.windowSize.x / 2.f, viewSettings.windowSize.y / 2.f) - sf::Vector2f(200, 150);
    sf::Vector2f mapNamePosition = sf::Vector2f(static_cast<float>(viewSettings.windowSize.x) / 2.f, 
        static_cast<float>(viewSettings.windowSize.y) - 150);
    sf::Vector2f brushMenuPosition = globalUIPosition + sf::Vector2f(245, 0); 
    sf::Vector2f drawModesMenuPosition = brushMenuPosition + sf::Vector2f(300, 0);
    sf::Vector2f mapSaveMenuPosition = drawModesMenuPosition + sf::Vector2f(320, 0);
    sf::Vector2f terrainGenerationMenuPosition = mapSaveMenuPosition + sf::Vector2f(225, 0);
    sf::Vector2f waterLevelMenuPosition = terrainGenerationMenuPosition + sf::Vector2f(220, 0);

    m_worldMenu = std::make_unique<WorldMenu>(globalUIPosition, drawModesMenuPosition, mapNamePosition);
    m_worldMenu->setDrawModeButtonOnClickCallback(DrawMode::SHADED, [this] () { this->onDrawModeButtonClicked(DrawMode::SHADED); } );
    m_worldMenu->setDrawModeButtonOnClickCallback(DrawMode::WIREFRAME, [this] () { this->onDrawModeButtonClicked(DrawMode::WIREFRAME); } );
    m_worldMenu->setDrawModeButtonOnClickCallback(DrawMode::WIREFRAME_SHADED, [this] () { this->onDrawModeButtonClicked(DrawMode::WIREFRAME_SHADED); } );
    m_worldMenu->selectDrawModeButton(DrawMode::WIREFRAME_SHADED);

    m_editionController = std::make_unique<EditionController>(m_worldModel, m_worldView, 
        globalUIPosition + sf::Vector2f(5, 5), terrainGenerationMenuPosition, viewSettings.windowSize);
    m_brushController = std::make_unique<BrushController>(brushMenuPosition);
    m_navigationController = std::make_unique<NavigationController>(m_worldModel, m_worldView, globalUIPosition + sf::Vector2f(5, 5));
    m_mapLoadSaveController = std::make_unique<MapLoadSaveController>(&m_worldModel, &m_worldView, 
        m_editionController.get(), mapSaveMenuPosition, [this] () { this->onMapLoaded(); });

    m_worldMenu->initWaterLevelMenu(waterLevelMenuPosition);
    m_worldMenu->initQuitMenu(quitMenuPosition);
    m_worldMenu->setQuitMenuVisibility(false);
    m_worldMenu->initOnWaterLevelIncrementButtonClickCallback([this] () { this->onWaterLevelButtonClicked(1); });
    m_worldMenu->initOnWaterLevelDecrementButtonClickCallback([this] () { this->onWaterLevelButtonClicked(-1); });

    m_worldModel.loadMap(mapName);
    onMapLoaded();
    sf::Vector2i tilesSize = m_worldModel.getTilesSize();
    m_worldView.init(viewSettings.center, viewSettings.size, 15);
    m_worldView.initCamera(tilesSize.x, tilesSize.y, cameraSettings.heightScale,
        cameraSettings.projectionAngleX, cameraSettings.projectionAngleY, m_worldModel.getCenter());
    m_worldView.initTileMap(m_worldModel.getTiles(), m_worldModel.getMinElevation(), m_worldModel.getMaxElevation(), m_worldModel.getWaterHeight());
    m_worldView.initBrushes(m_brushController->getBrushesImagePaths());
    m_worldView.initSplatMap(m_worldModel.getSplatMapFilepath(), sf::Vector2i(tilesSize.x, tilesSize.y), m_worldModel.getMapSize().x, m_worldModel.getMapSize().y);
    m_worldView.initWaterView(m_worldModel.getMapSize().x, m_worldModel.getMapSize().y, m_worldModel.getTilesSize());
    m_worldView.initEnvironment(viewSettings.windowSize);
    setWaterLevelValueText(m_worldModel.getWaterHeight());
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

void WorldController::onMapLoaded()
{
    m_worldMenu->setMapName(m_worldModel.getMapName());
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
            m_worldView.setIsWaterVisible(true);
            break;
        case DrawMode::WIREFRAME:
            m_worldView.setAreShadedTilesVisible(false);
            m_worldView.setIsWireframeVisible(true);
            m_worldView.setIsWaterVisible(false);
            break;
        case DrawMode::SHADED:
            m_worldView.setAreShadedTilesVisible(true);
            m_worldView.setIsWireframeVisible(false);
            m_worldView.setIsWaterVisible(true);
            break;
    }
    m_currentDrawMode = mode;
    m_worldMenu->selectDrawModeButton(m_currentDrawMode);
}

void WorldController::onWaterLevelButtonClicked(int factor)
{
    float newLevel = static_cast<float>(m_worldModel.getWaterHeight()) + (static_cast<float>(factor) * waterLevelIncrementStep);
    newLevel = std::clamp(newLevel, -10.0f, 10.0f);
    m_worldModel.setWaterHeight(newLevel);
    m_worldView.setWaterHeight(m_worldModel.getWaterHeight());
    setWaterLevelValueText(m_worldModel.getWaterHeight());
    m_worldView.updatePositions(m_worldModel.getTiles(), m_worldView.getCamera());
}

void WorldController::setWaterLevelValueText(float waterHeight) const
{
    m_worldMenu->setWaterLevelValueText(MathUtils::toString(waterHeight));
}