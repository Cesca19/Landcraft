//
// Created by fran on 03/02/2026.
//

#include "WorldManager.hpp"

WorldManager::WorldManager(const int width, const int height, const std::string &windowTitle)
    : m_window(sf::VideoMode(width, height), windowTitle)
    , m_worldView(std::make_unique<WorldView>(sf::Vector2f({0, 0}),
                                              sf::Vector2f({static_cast<float>(width), static_cast<float>(height)})))
    , m_screenMap(nullptr)
    , m_currentSelectionMode(SelectionMode::TILE_CORNER)
    , m_heightOffset(1)
    , m_zoomStep(1)
    , m_yawRotationStep(90)
    , m_pitchRotationStep(5)
{
}

WorldManager::~WorldManager()
{
}

void WorldManager::init(const std::string worldMapFilePath, int tileSizeX, int tileSizeY, int heightScale, int projectionAngleX, int projectionAngleY)
{
    m_screenMap = std::make_unique<ScreenMap>(tileSizeX, tileSizeY, heightScale, projectionAngleX, projectionAngleY);
    m_screenMap->init(worldMapFilePath);
    m_worldView->init(m_window);
    m_worldView->setOrigin(m_screenMap->getScreenMapCenter());
}

void WorldManager::update()
{
    sf::Clock clock;
    float deltaTime = 0;
    while (m_window.isOpen())
    {
        deltaTime = clock.restart().asSeconds();
        handleEvents();
        m_window.clear();
        m_worldView->update(deltaTime);
        m_screenMap->update(deltaTime, m_window, m_currentSelectionMode);
        m_worldView->setOrigin(m_screenMap->getScreenMapCenter());
        m_screenMap->draw(m_window);
        m_window.display();
    }
}

void WorldManager::handleEvents()
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

            // map editing event
            if (event.key.code == sf::Keyboard::Space)
                m_currentSelectionMode = (m_currentSelectionMode == SelectionMode::TILE)
                                ? SelectionMode::TILE_CORNER
                                : SelectionMode::TILE;
            if (event.key.code == sf::Keyboard::Z)
                m_screenMap->setSelectedCornersHeight(m_heightOffset);
            if (event.key.code == sf::Keyboard::S)
                m_screenMap->setSelectedCornersHeight(- m_heightOffset);

            // cam events
            if (event.key.code == sf::Keyboard::I)
                m_worldView->zoom(-m_zoomStep);
            if (event.key.code == sf::Keyboard::O)
                m_worldView->zoom(m_zoomStep);

            if ( event.key.code == sf::Keyboard::Right)
                m_screenMap->rotateAroundZAxis(m_yawRotationStep);
            if (event.key.code == sf::Keyboard::Left)
                m_screenMap->rotateAroundZAxis(-m_yawRotationStep);

            if (event.key.code == sf::Keyboard::Up)
                m_screenMap->rotateAroundXAxis(m_pitchRotationStep);
            if (event.key.code == sf::Keyboard::Down)
                m_screenMap->rotateAroundXAxis(-m_pitchRotationStep);
        }
        // camera events
        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                {m_worldView->zoom(event.mouseWheelScroll.delta);}
        }
    }
}
