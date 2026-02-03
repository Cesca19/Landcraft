//
// Created by fran on 03/02/2026.
//

#include "WorldView.hpp"

WorldView::WorldView(const sf::Vector2f origin, const sf::Vector2f size)
    : m_minZoom(0.2)
    , m_maxZoom(2.5)
    , m_currentZoom(1)
    , m_zoomOffset(0.1)
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

void WorldView::setSize(sf::Vector2f size)
{
    m_view.setSize(size);
    m_baseSize = size;
    m_currentZoom = 1;
    update();
}

void WorldView::setOrigin(sf::Vector2f origin)
{
    m_view.setCenter(origin);
    update();
}

void WorldView::zoom(const int zoomDelta)
{
    // TO DO : zoom smooth interpolation
    m_currentZoom += m_zoomOffset * zoomDelta;
    m_currentZoom = std::clamp(m_currentZoom, m_minZoom, m_maxZoom);
    m_view.setSize(m_baseSize * m_currentZoom);
    update();
}

void WorldView::update()
{
    if (m_window != nullptr)
        m_window->setView(m_view);
}
