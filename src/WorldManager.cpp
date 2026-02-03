//
// Created by fran on 03/02/2026.
//

#include "WorldManager.hpp"

WorldManager::WorldManager(int width, int height, std::string windowTitle, sf::Vector2f viewSize)
    : m_window(sf::VideoMode(width, height), windowTitle)
    , m_screenMap(nullptr)
    , m_viewSize(viewSize)
    , m_worldView({0, 0}, m_viewSize)
    , m_heightOffset(1)
    , m_currentSelectionMode(SelectionMode::TILE_CORNER)
{
}

WorldManager::~WorldManager()
{
}

void WorldManager::init(const std::string worldMapFilePath, int tileSizeX, int tileSizeY, int heightScale,
    sf::Vector2f translationOffset, int projectionAngleX, int projectionAngleY)
{
    m_screenMap = std::make_unique<ScreenMap>(tileSizeX, tileSizeY, heightScale, translationOffset, projectionAngleX, projectionAngleY);
    m_screenMap->init(worldMapFilePath);
    m_worldView.setCenter(m_screenMap->getCenter());
    m_window.setView(m_worldView);
}

void WorldManager::update()
{
    while (m_window.isOpen())
    {
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                m_window.close();
            if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Escape)
                    m_window.close();
                if (event.key.code == sf::Keyboard::Space)
                    m_currentSelectionMode = (m_currentSelectionMode == SelectionMode::TILE)
                                    ? SelectionMode::TILE_CORNER
                                    : SelectionMode::TILE;
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Z)
                    m_screenMap->setSelectedCornersHeight(m_heightOffset);
                if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
                    m_screenMap->setSelectedCornersHeight(- m_heightOffset);
            }
        }
        m_window.clear();
        m_screenMap->update(m_window, m_currentSelectionMode);
        m_screenMap->draw(m_window);
        m_window.display();
    }
}
