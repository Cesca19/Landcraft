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
    , m_yawRotationStep(22.5)
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
        drawBackground();
        drawWireframe();
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

        sf::Vector2f moveVector(0.f, 0.f);
        // scren space movement input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) moveVector.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) moveVector.y += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) moveVector.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveVector.x += 1.f;

        if (moveVector.x != 0.f || moveVector.y != 0.f)
        {
            // normalize the movement vector to ensure consistent speed in all directions
            // TODO : add it to isometric projection
            float length = std::hypot(moveVector.x, moveVector.y);
            moveVector /= length;

            // adapt movement speed based on zoom level to maintain a consistent feel
            // float currentZoom = m_worldView->getTargetZoom(); 
            // float adjustedSpeed = m_movementStep * currentZoom; 
            
            m_worldView->moveTarget(moveVector * (float)m_movementStep);
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

void WorldManager::drawBackground()
{
    sf::View previousView = m_window.getView();
    m_window.setView(m_window.getDefaultView());
    drawSkyBox();
    drawGizmo();
    m_window.setView(previousView);
}

void WorldManager::drawWireframe()
{
    // TO do get isometic projection instance out of the scrren map
    m_screenMap->drawWorldReference(m_window,  m_worldView->getCenter(), m_worldView->getSize());
}

void WorldManager::drawSkyBox()
{
    // may be create a shader and add some particles for night or day
    sf::VertexArray background(sf::Quads, 4);
    // sf::Color bottomColor(120, 72, 153);   // purple
    // sf::Color topColor(255, 179, 193);  // pink
    sf::Color bottomColor(255, 179, 193);  // pink
    sf::Color topColor(196, 218, 242);
    sf::Vector2u windowSize = m_window.getSize();

    background[0].position = sf::Vector2f(0, 0);
    background[0].color = topColor;
    background[1].position = sf::Vector2f(windowSize.x, 0);
    background[1].color = topColor;
    background[2].position = sf::Vector2f(windowSize.x, windowSize.y);
    background[2].color = bottomColor;
    background[3].position = sf::Vector2f(0, windowSize.y);
    background[3].color = bottomColor;
    m_window.draw(background);
}

void WorldManager::drawGizmo()
{
    sf::Vector2f gizmoPos(m_window.getSize().x - 50.0f, 100.0f);
    m_screenMap->drawGizmo(m_window, gizmoPos, 40.0f);
}