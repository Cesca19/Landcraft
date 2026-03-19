//
// Created by fran on 11/03/2026.
//

#include "WorldView.hpp"

WorldView::WorldView()
    : m_minZoom(0.2f)
    , m_maxZoom(3.0f)
    , m_currentZoom(1.0f)
    , m_targetZoom(1.0f)
    , m_zoomOffset(0.1f)
    , m_zoomSpeed(10.0f)
    , m_movementSpeed(10.0f)
    , m_isDragging(false)
    , m_dragStartWorldPos({0, 0})
    , m_isMoving(false)
{
}

void WorldView::init(const sf::Vector2f center, const sf::Vector2f size)
{
    m_view.setCenter(center);
    m_view.setSize(size);

    m_baseSize = size;
    m_currentCenter = center;
    m_targetCenter = center;
}

void WorldView::initCamera(float tileSizeX, float tileSizeY, float heightScale, float projectionAngleX, float projectionAngleY, const sf::Vector2f worldPivot)
{
    m_camera = std::make_unique<Camera>(tileSizeX, tileSizeY, heightScale, projectionAngleX, projectionAngleY);
    m_camera->setWorldPivotWithWorldPosition(worldPivot);
}

void WorldView::initTileMap(const std::vector<std::vector<Tile>> &tiles)
{
    m_tileMap = std::make_unique<TileMap>("assets/textures/tileset.png", sf::Vector2u{256, 256});
    m_tileMap->init(tiles, *m_camera);
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

    bool hasCameraMoved = false;
    m_camera->update(deltaTime, hasCameraMoved);
    if (hasCameraMoved)
        m_tileMap->updatePositions(tiles, *m_camera);
    m_isMoving = m_isMoving || hasCameraMoved;
    m_environmentView->update(*m_camera, m_view.getCenter(), m_view.getSize(),
        {window.getSize().x - 50.0f, 100.0f}, 40, isMoving());
}

void WorldView::draw(sf::RenderWindow &window) const
{
    m_environmentView->drawSkyBox(window);
    m_environmentView->drawWorldReference(window);
    window.setView(m_view);
    window.draw(*m_tileMap);
    m_environmentView->drawWorldGizmo(window);
}

bool WorldView::isMoving() const
{
    return m_isMoving || m_isDragging;
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

void WorldView::updateContinuousRotation(const sf::RenderWindow &window, const std::vector<std::vector<Tile>>& tiles) const
{
    m_camera->updateContinuousRotation(sf::Mouse::getPosition(window));
    m_tileMap->updatePositions(tiles, *m_camera);
}

void WorldView::updateTileCorners(const std::vector<std::vector<Tile>>& worldTiles, const std::vector<TileCorner *> &selectedCorners) const
{
    m_tileMap->updatePositions(worldTiles, selectedCorners, *m_camera);
}

void WorldView::paintTiles(const std::vector<std::vector<Tile>> &worldTiles, const std::vector<Tile *> &tilesToPaint,
    const int textureId) const
{
    m_tileMap->paintTiles(worldTiles, tilesToPaint, textureId);
}

void WorldView::paintTile(const std::vector<std::vector<Tile>> &worldTiles, Tile *tileToPaint, int textureId) const
{
    m_tileMap->paintTile(worldTiles, tileToPaint, textureId);
}

void WorldView::updateViewCenter(const sf::Vector2f center)
{
    m_view.setCenter(center);
}
