//
// Created by fran on 11/03/2026.
//

#include "WorldView.hpp"

WorldView::WorldView()
    : m_minZoom(0.05f)
    , m_maxZoom(5.0f)
    , m_currentZoom(1.0f)
    , m_targetZoom(1.0f)
    , m_zoomOffset(0.1f)
    , m_zoomSpeed(10.0f)
    , m_movementSpeed(10.0f)
    , m_dragStartWorldPos({0, 0})
    , m_isDragging(false)
    , m_isMoving(false)
    , m_waterView(std::make_unique<WaterView>())
    , m_tileMap(nullptr)
{
}

void WorldView::init(const sf::Vector2f center, const sf::Vector2f size, const int defaultZoom)
{
    m_view.setCenter(center);
    m_view.setSize(size);
    zoom(defaultZoom);

    m_baseSize = size;
    m_currentCenter = center;
    m_targetCenter = center;

    m_defaultCenter = center;
    m_defaultZoom = defaultZoom;
    m_defaultTargetZoom = m_targetZoom;
}

void WorldView::initCamera(float tileSizeX, float tileSizeY, float heightScale, float projectionAngleX, float projectionAngleY, const sf::Vector2f worldPivot)
{
    m_camera = std::make_unique<Camera>(tileSizeX, tileSizeY, heightScale, projectionAngleX, projectionAngleY);
    m_camera->setWorldPivotWithWorldPosition(worldPivot);
}

void WorldView::initTileMap(const std::vector<std::vector<Tile>> &tiles
    , float minElevation, float maxElevation, float waterHeight)
{
    m_tileMap = std::make_unique<TileMap>(minElevation, maxElevation, waterHeight);
    m_waterHeight = waterHeight;
    resetTileMap(tiles);
}

void WorldView::initBrushes(const std::vector<std::string> &brushesImagePaths)
{
    m_tileMap->initBrushes(brushesImagePaths);
}

void WorldView::clearTileMap()
{
    m_tileMap->clear();
}

void WorldView::resetTileMap(const std::vector<std::vector<Tile>>& tiles)
{
    m_tileMap->init(tiles, *m_camera);
}

void WorldView::initSplatMap(const std::string &filepath, const sf::Vector2i &tileSize, int nbCols, int nbRows)
{
    m_tileMap->initSplatMap(filepath, tileSize, nbCols, nbRows);
}

void WorldView::initWaterView(int nbCols, int nbRows, const sf::Vector2i &tileSize)
{
    m_waterView->init(nbCols, nbRows, tileSize, m_waterHeight, *m_camera);
}

void WorldView::initEnvironment(const sf::Vector2u windowSize)
{
    m_environmentView = std::make_unique<EnvironmentView>();
    m_environmentView->init(windowSize);
}

void WorldView::update(const float deltaTime, const std::vector<std::vector<Tile>>& tiles, const sf::RenderWindow &window)
{
    // if (m_isDragging) don't know who should do the update
    //     updateDragging(sf::Mouse::getPosition(*m_window));

    // zoom lerping
    m_isMoving = false;
    if (std::abs(m_targetZoom - m_currentZoom) > m_zoomEpsilon) {
        float t = std::min(deltaTime * m_zoomSpeed, 1.0f);
        m_currentZoom += (m_targetZoom - m_currentZoom) * t;
        m_view.setSize(m_baseSize * m_currentZoom);
        m_isMoving = true;
    } else
        if (m_currentZoom != m_targetZoom) {
            m_currentZoom = m_targetZoom;
            m_view.setSize(m_baseSize * m_currentZoom);
            m_isMoving = true;
        }

    // translation lerping
    // To do : add drag speed multiplier to make the drag more responsive, and normal movement smoother
    if (MathUtils::distanceBetweenPoints(m_targetCenter, m_currentCenter) > m_movementEpsilon) {
        const float speed = m_isDragging ? 50.0f : m_movementSpeed;
        float t = std::min(deltaTime * speed, 1.0f);
        m_currentCenter += (m_targetCenter - m_currentCenter) * t;
        // m_currentCenter += (m_targetCenter - m_currentCenter) * deltaTime * speed;
        updateViewCenter(m_currentCenter);
        m_isMoving = true;
    } else
        if (m_currentCenter != m_targetCenter) {
            m_currentCenter = m_targetCenter;
            updateViewCenter(m_currentCenter);
            m_isMoving = true;
        }

    m_camera->update(deltaTime);
    m_waterView->update(deltaTime);
    if (m_camera->isRotating() || m_camera->isContinuousRotationActive())
        updatePositions(tiles, *m_camera);
        // m_tileMap->updatePositions(tiles, *m_camera);
    m_environmentView->update(*m_camera, m_view.getCenter(), m_view.getSize(),
        {window.getSize().x - 50.0f, 100.0f}, 40, isMoving() || isRotating());
}

void WorldView::draw(sf::RenderWindow &window) const
{
    m_environmentView->drawSkyBox(window);
    // m_environmentView->drawWorldReference(window);

    window.setView(m_view);
    m_waterView->draw(window);
    window.draw(*m_tileMap);

    m_environmentView->drawWorldGizmo(window);
}

void WorldView::onWindowResized(const sf::Vector2u windowSize)
{
    setSize(sf::Vector2f(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)));
    m_environmentView->onWindowResized(windowSize);
}

bool WorldView::isMoving() const
{
    return m_isMoving || m_isDragging;
}

bool WorldView::isRotating() const
{
    return m_camera->isRotating();
}

void WorldView::setSize(const sf::Vector2f size)
{
    m_baseSize = size;
    m_view.setSize(size * m_currentZoom);
}

void WorldView::setCenter(const sf::Vector2f center)
{
    m_currentCenter = center;
    m_targetCenter = center;
    updateViewCenter(center);
}

sf::Vector2f WorldView::getCenter() const
{
    return m_view.getCenter();
}

sf::Vector2f WorldView::getSize() const
{
    return m_view.getSize();
}

void WorldView::zoom(const int zoomDelta)
{
    // simple zoom towards the center of the view (not mouse position)
    m_targetZoom += m_zoomOffset * static_cast<float>(zoomDelta);
    m_targetZoom = std::clamp(m_targetZoom, m_minZoom, m_maxZoom);
}

const Camera &WorldView::getCamera() const
{
    return *m_camera;
}

void WorldView::recenter()
{
    setCenter(m_defaultCenter);
    m_targetZoom = m_defaultTargetZoom;
    if (m_camera)
        m_camera->resetRotation();
}

void WorldView::zoomAtMouse(const sf::RenderWindow &window, const float zoomDelta)
{
    // This method allows zooming towards the mouse position, keeping the point under the mouse stable.
    // --- CRUCIAL STEP: PREDICTIVE CALCULATION ---
    // We don't work with the current position (which is moving),
    // but with the TARGET position (where the camera will end up).
    // This guarantees perfect mathematical stability even if the camera is moving.

    // 1. Calculate the world point under the mouse based on the CURRENT TARGET
    // We simulate a view that would already have reached its destination
    const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::View targetView = m_view;
    targetView.setSize(m_baseSize * m_targetZoom);
    targetView.setCenter(m_targetCenter);

    const sf::Vector2f mouseWorldPosBefore = window.mapPixelToCoords(mousePos, targetView);

    // 2. Apply the new zoom to the target
    float oldTargetZoom = m_targetZoom;
    m_targetZoom -= m_zoomOffset * zoomDelta; 
    m_targetZoom = std::clamp(m_targetZoom, m_minZoom, m_maxZoom);
    if (std::abs(m_targetZoom - oldTargetZoom) < 0.0001f) return;

    // 3. Calculate where that same world point would be with the NEW target zoom
    targetView.setSize(m_baseSize * m_targetZoom); 
    // (The center of targetView is still the old m_targetCenter for now)
    sf::Vector2f mouseWorldPosAfter = window.mapPixelToCoords(mousePos, targetView);

    // 4. Calculate the necessary correction
    // "The mouse aims at point X, after zoom it aims at point Y. 
    // We need to move the camera by (X - Y) to bring the point back under the mouse."
    const sf::Vector2f correction = mouseWorldPosBefore - mouseWorldPosAfter;
    // 5. Apply the correction to the TARGET only
    m_targetCenter += correction;
    // The update() will do the rest: it will interpolate both zoom and position towards these new targets.
}

void WorldView::startDragging(const sf::RenderWindow& window)
{
    m_isDragging = true;
    m_dragStartWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), m_view);
}

void WorldView::updateDragging(const sf::RenderWindow& window)
{
    if (!m_isDragging) return;

    const sf::Vector2f currentWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), m_view);
    const sf::Vector2f delta = m_dragStartWorldPos - currentWorldPos;

    m_targetCenter += delta;
    m_currentCenter += delta; 
    updateViewCenter(m_currentCenter);
}

void WorldView::stopDragging()
{
    m_isDragging = false;
}

void WorldView::moveTarget(const sf::Vector2f& offset) 
{  
    m_targetCenter += offset;
}

sf::Vector2f WorldView::getTargetOrigin() const 
{
    return m_targetCenter;
}

void WorldView::rotatePitch(const float angle) const
{
    m_camera->rotatePitch(angle);
}

void WorldView::rotateYaw(const float angle) const
{
    m_camera->rotateYaw(angle);
}

void WorldView::startContinuousRotation(const sf::RenderWindow &window) const
{
    m_camera->startContinuousRotation(sf::Mouse::getPosition(window));
}

void WorldView::stopContinuousRotation() const
{
    m_camera->stopContinuousRotation();
}

void WorldView::updateContinuousRotation(const sf::RenderWindow &window) const
{
    m_camera->updateContinuousRotation(sf::Mouse::getPosition(window));
}

void WorldView::updatePositions(const std::vector<std::vector<Tile>> &worldTiles, const Camera &camera) const
{
    m_tileMap->updatePositions(worldTiles, camera);
    m_waterView->updatePositions(camera);
}

void WorldView::updateTileCorners(const std::vector<std::vector<Tile>> &worldTiles, const std::vector<TileCorner *> &selectedCorners) const
{
    m_tileMap->updatePositions(worldTiles, selectedCorners, *m_camera);
    m_waterView->updatePositions(*m_camera);
}

void WorldView::setIsWireframeVisible(bool enabled) const
{
    m_tileMap->setIsWireframeVisible(enabled);
}

void WorldView::setAreShadedTilesVisible(bool enabled) const
{
    m_tileMap->setAreShadedTilesVisible(enabled);
}

void WorldView::setIsWaterVisible(bool enabled) const
{
    m_waterView->setIsVisible(enabled);
    if (enabled)
        m_waterView->updatePositions(*m_camera);
}

void WorldView::drawStrokeOnSplatmap(const PaintStroke &stroke, const sf::Vector2i &tileSize, int nbCols, int nbRows)
{
    m_tileMap->drawStrokeOnSplatMap(stroke, tileSize, nbCols, nbRows);
}

sf::Image WorldView::getSplatmapArea(const sf::IntRect &area) const
{
    return m_tileMap->getSplatMapArea(area);
}

void WorldView::restoreSplatmapArea(const sf::IntRect &area, const sf::Image &pixels)
{
    m_tileMap->restoreSplatMapArea(area, pixels);
}

void WorldView::updateSplatmapImage()
{
    m_tileMap->updateSplatMapImage();
}

const sf::Image &WorldView::getSplatmapImage() const
{
    return m_tileMap->getSplatMapImage();
}

void WorldView::updateViewCenter(const sf::Vector2f center)
{
    m_view.setCenter(center);
}
