//
// Created by fran on 03/02/2026.
//

#include "WorldView.hpp"

WorldView::WorldView(const sf::Vector2f origin, const sf::Vector2f size)
    : m_minZoom(0.2)
    , m_maxZoom(2.5)
    , m_currentZoom(1)
    , m_targetZoom(m_currentZoom)
    , m_zoomOffset(0.1)
    , m_zoomSpeed(5)
    , m_rotationOffset(10)
    , m_baseSize(size)
    , m_view(origin, size)
    , m_window(nullptr)
{
}

WorldView::~WorldView()
{
}

void WorldView::init(sf::RenderWindow &window)
{
    window.setView(m_view);
    m_window = &window;
}

void WorldView::update(const float deltaTime)
{
    if (std::abs(m_targetZoom - m_currentZoom) > m_epsilon) {
        m_currentZoom = m_currentZoom + (m_targetZoom - m_currentZoom) * deltaTime * m_zoomSpeed;
        m_view.setSize(m_baseSize * m_currentZoom);
    } else
        m_view.setSize(m_baseSize * m_currentZoom);
    updateWindowView();
}

void WorldView::setSize(const sf::Vector2f size)
{
    m_view.setSize(size);
    m_baseSize = size;
    m_currentZoom = 1;
    m_targetZoom = m_currentZoom;
    updateWindowView();
}

void WorldView::setOrigin(const sf::Vector2f origin)
{
    m_view.setCenter(origin);
    updateWindowView();
}

void WorldView::zoom(const int zoomDelta)
{
    // TO DO : changing the pivot point(origin of the view) while zooming u
    // so when the use is zooming smoothly translate the camera to the mouse pos while zooming there
    m_targetZoom += m_zoomOffset * zoomDelta;
    m_targetZoom = std::clamp(m_targetZoom, m_minZoom, m_maxZoom);
}

void WorldView::rotate(int delta)
{
    m_view.rotate(delta * m_rotationOffset);
    updateWindowView();
}

void WorldView::updateWindowView()
{
    if (m_window != nullptr)
        m_window->setView(m_view);
}
