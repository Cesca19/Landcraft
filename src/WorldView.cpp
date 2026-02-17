//
// Created by fran on 03/02/2026.
//

#include "WorldView.hpp"

WorldView::WorldView(const sf::Vector2f origin, const sf::Vector2f size)
    : m_minZoom(0.2f)
    , m_maxZoom(3.0f)
    , m_currentZoom(1.0f)
    , m_targetZoom(1.0f)
    , m_zoomOffset(0.1f)
    , m_zoomSpeed(10.0f)
    , m_movementSpeed(10.0f)
    , m_baseSize(size)
    , m_view(origin, size)
    , m_window(nullptr)
    , m_currentOrigin(origin)
    , m_targetOrigin(origin)
    , m_isDragging(false)
    , m_dragStartWorldPos({0, 0})
{
}

WorldView::~WorldView()
{
}

void WorldView::init(sf::RenderWindow &window)
{
    m_window = &window;
    m_window->setView(m_view);
}

void WorldView::update(const float deltaTime)
{
    bool needsUpdate = false;
    // if (m_isDragging) dont='t know who should do the update
    //     updateDragging(sf::Mouse::getPosition(*m_window));

    // zoom lerping
    if (std::abs(m_targetZoom - m_currentZoom) > m_zoomEpsilon) {
        m_currentZoom += (m_targetZoom - m_currentZoom) * deltaTime * m_zoomSpeed;
        m_view.setSize(m_baseSize * m_currentZoom);
        needsUpdate = true;
    } else {
        if (m_currentZoom != m_targetZoom) {
            m_currentZoom = m_targetZoom;
            m_view.setSize(m_baseSize * m_currentZoom);
            needsUpdate = true;
        }
    }

    // translation lerping
    // To do : add drag speed multiplier to make the drag more responsive, and normal movement smoother
    if (IsometricProjection::distanceBetweenPoints(m_targetOrigin, m_currentOrigin) > m_movementEpsilon) {
        float speed = m_isDragging ? 50.0f : m_movementSpeed;

        m_currentOrigin += (m_targetOrigin - m_currentOrigin) * deltaTime * speed;
        setCenter(m_currentOrigin);
    } else {
        if (m_currentOrigin != m_targetOrigin) {
            m_currentOrigin = m_targetOrigin;
            setCenter(m_currentOrigin);
        }
    }

    if (needsUpdate)
        updateWindowView();
}

void WorldView::setSize(const sf::Vector2f size)
{
    m_baseSize = size;
    m_view.setSize(size * m_currentZoom);
    updateWindowView();
}

void WorldView::setOrigin(const sf::Vector2f origin)
{
    m_currentOrigin = origin;
    m_targetOrigin = origin;
    setCenter(origin);
}

void WorldView::zoom(const int zoomDelta)
{
    // simple zoom towards the center of the view (not mouse position)
    m_targetZoom += m_zoomOffset * zoomDelta;
    m_targetZoom = std::clamp(m_targetZoom, m_minZoom, m_maxZoom);
}

void WorldView::zoomAtMouse(const int zoomDelta, const sf::Vector2i mousePos)
{
    // This method allows zooming towards the mouse position, keeping the point under the mouse stable.
    if (!m_window) return;

    // --- CRUCIAL STEP: PREDICTIVE CALCULATION ---
    // We don't work with the current position (which is moving),
    // but with the TARGET position (where the camera will end up).
    // This guarantees perfect mathematical stability even if the camera is moving.

    // 1. Calculate the world point under the mouse based on the CURRENT TARGET
    // We simulate a view that would already have reached its destination
    sf::View targetView = m_view;
    targetView.setSize(m_baseSize * m_targetZoom);
    targetView.setCenter(m_targetOrigin);

    sf::Vector2f mouseWorldPosBefore = m_window->mapPixelToCoords(mousePos, targetView);

    // 2. Apply the new zoom to the target
    float oldTargetZoom = m_targetZoom;
    m_targetZoom -= m_zoomOffset * zoomDelta; 
    m_targetZoom = std::clamp(m_targetZoom, m_minZoom, m_maxZoom);
    if (std::abs(m_targetZoom - oldTargetZoom) < 0.0001f) return;

    // 3. Calculate where that same world point would be with the NEW target zoom
    targetView.setSize(m_baseSize * m_targetZoom); 
    // (The center of targetView is still the old m_targetOrigin for now)
    sf::Vector2f mouseWorldPosAfter = m_window->mapPixelToCoords(mousePos, targetView);

    // 4. Calculate the necessary correction
    // "The mouse aims at point X, after zoom it aims at point Y. 
    // We need to move the camera by (X - Y) to bring the point back under the mouse."
    sf::Vector2f correction = mouseWorldPosBefore - mouseWorldPosAfter;
    // 5. Apply the correction to the TARGET only
    m_targetOrigin += correction;
    // The update() will do the rest: it will interpolate both zoom and position towards these new targets.
}

void WorldView::startDragging(sf::Vector2i mousePos)
{
    if (!m_window) return;
    m_isDragging = true;
    m_dragStartWorldPos = m_window->mapPixelToCoords(mousePos, m_view);
}

void WorldView::updateDragging(sf::Vector2i mousePos)
{
    if (!m_isDragging || !m_window) return;

    sf::Vector2f currentWorldPos = m_window->mapPixelToCoords(mousePos, m_view);
    sf::Vector2f delta = m_dragStartWorldPos - currentWorldPos;

    // Pour le drag, on applique immédiatement à la cible ET à l'origine
    // pour une réponse 1:1 sans lag ("Marshmallow effect")
    m_targetOrigin += delta;
    m_currentOrigin += delta; 
    setCenter(m_currentOrigin);
}

void WorldView::stopDragging()
{
    m_isDragging = false;
}

void WorldView::setCenter(const sf::Vector2f center)
{
    m_view.setCenter(center);
    updateWindowView();
}

void WorldView::updateWindowView()
{
    if (m_window != nullptr)
        m_window->setView(m_view);
}