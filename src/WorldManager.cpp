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
    , m_movementStep(5)
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
    m_worldView->zoom(m_zoomStep * 10); // zoom out a bit to see more of the map at the start
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
            if (event.key.code == sf::Keyboard::Add)
                m_screenMap->setSelectedCornersHeight(m_heightOffset);
            if (event.key.code == sf::Keyboard::Subtract)
                m_screenMap->setSelectedCornersHeight(- m_heightOffset);

            // cam events
            // zoom
            if (event.key.code == sf::Keyboard::I)
                m_worldView->zoom(-m_zoomStep);
            if (event.key.code == sf::Keyboard::O)
                m_worldView->zoom(m_zoomStep);
            // yaw
            if ( event.key.code == sf::Keyboard::Right)
                m_screenMap->rotateAroundZAxis(m_yawRotationStep);
            if (event.key.code == sf::Keyboard::Left)
                m_screenMap->rotateAroundZAxis(-m_yawRotationStep);
            // pitch
            if (event.key.code == sf::Keyboard::Up)
                m_screenMap->rotateAroundXAxis(m_pitchRotationStep);
            if (event.key.code == sf::Keyboard::Down)
                m_screenMap->rotateAroundXAxis(-m_pitchRotationStep);
        }

        /* camera events */

        // zoom with mouse wheel at mouse position
        // Note: We handle this separately from the keyboard zoom to allow for zooming at the mouse position.
        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                {
                    // m_worldView->zoom(event.mouseWheelScroll.delta);
                    m_worldView->zoomAtMouse(event.mouseWheelScroll.delta, sf::Mouse::getPosition(m_window));
                }
        }

        //  drag and drop with middle mouse button 
        if (event.type == sf::Event::MouseButtonPressed)
            if (event.mouseButton.button == sf::Mouse::Left)
                m_worldView->startDragging(sf::Mouse::getPosition(m_window));
        if (event.type == sf::Event::MouseButtonReleased)
            if (event.mouseButton.button == sf::Mouse::Left)
                m_worldView->stopDragging();
        if (event.type == sf::Event::MouseMoved)
            m_worldView->updateDragging(sf::Mouse::getPosition(m_window));
    }
}