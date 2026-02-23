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

void WorldManager::init(const std::string &worldMapFilePath, float tileSizeX, float tileSizeY, float heightScale, float projectionAngleX, float projectionAngleY)
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
        if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            m_window.close();
        handlePanEvents(event);
        handleRotationEvents(event);
        handleZoomEvents(event);
        handleMapEditingEvents(event);
    }
}

void WorldManager::handlePanEvents(const sf::Event &event) const
{
    // mouse
    //  drag and drop with middle mouse button
    constexpr sf::Mouse::Button mouseButton = sf::Mouse::Middle;
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == mouseButton)
            m_worldView->startDragging(sf::Mouse::getPosition(m_window));
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == mouseButton)
            m_worldView->stopDragging();
    if (event.type == sf::Event::MouseMoved)
        m_worldView->updateDragging(sf::Mouse::getPosition(m_window));

    // keyboard
    sf::Vector2f moveVector(0.f, 0.f);
    // screen space movement input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) moveVector.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) moveVector.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) moveVector.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveVector.x += 1.f;

    if (moveVector.x != 0.f || moveVector.y != 0.f)
    {
        // normalize the movement vector to ensure consistent speed in all directions
        moveVector = IsometricProjection::normalize(moveVector);
        // adapt movement speed based on zoom level to maintain a consistent feel
        // float currentZoom = m_worldView->getTargetZoom();
        // float adjustedSpeed = m_movementStep * currentZoom;
        m_worldView->moveTarget(moveVector * m_movementStep);
    }
}

void WorldManager::handleRotationEvents(const sf::Event &event)
{
    // mouse
    // left button + vertical / horizontal scroll
    // this might cause problems  when selecting objects in the future
    constexpr sf::Mouse::Button mouseButton = sf::Mouse::Left;
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == mouseButton)
        m_screenMap->startContinuousRotation(m_window, sf::Mouse::getPosition(m_window));
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == mouseButton)
        m_screenMap->stopContinuousRotation();
    if (event.type == sf::Event::MouseMoved)
        m_screenMap->updateContinuousRotation(m_window, sf::Mouse::getPosition(m_window));

    // keyboard
    // yaw
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A)
        m_screenMap->rotateAroundZAxis(m_yawRotationStep);
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E)
        m_screenMap->rotateAroundZAxis(-m_yawRotationStep);
    // pitch
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R)
        m_screenMap->rotateAroundXAxis(m_pitchRotationStep);
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F)
        m_screenMap->rotateAroundXAxis(-m_pitchRotationStep);

    // gizmo axes click like blender
}

void WorldManager::handleZoomEvents(const sf::Event &event) const
{
    // mouse
    // zoom with mouse wheel at mouse position
    // Note: We handle this separately from the keyboard zoom to allow for zooming at the mouse position.
    const bool isCtrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
    if (event.type == sf::Event::MouseWheelScrolled)
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel && !isCtrlPressed)
            m_worldView->zoomAtMouse(event.mouseWheelScroll.delta, sf::Mouse::getPosition(m_window));

    // keyboard
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::I)
        m_worldView->zoom(-m_zoomStep);
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::O)
        m_worldView->zoom(m_zoomStep);
}

void WorldManager::handleMapEditingEvents(const sf::Event &event)
{
    // keyboard
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
        m_currentSelectionMode = (m_currentSelectionMode == SelectionMode::TILE)
                        ? SelectionMode::TILE_CORNER
                        : SelectionMode::TILE;
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Add)
        m_screenMap->setSelectedCornersHeight(m_heightOffset);
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Subtract)
        m_screenMap->setSelectedCornersHeight(- m_heightOffset);

    // mouse
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
        && event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
        m_screenMap->setSelectedCornersHeight(m_heightOffset * event.mouseWheelScroll.delta);
}

void WorldManager::drawBackground()
{
    const sf::View previousView = m_window.getView();
    m_window.setView(m_window.getDefaultView());
    drawSkyBox();
    drawGizmo();
    m_window.setView(previousView);
    // draw the wireframe on the world view
    drawWireframe();
}

void WorldManager::drawWireframe()
{
    // TO do get isometric projection instance out of the screen map
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
    const sf::Vector2f gizmoPos(m_window.getSize().x - 50.0f, 100.0f);
    m_screenMap->drawGizmo(m_window, gizmoPos, 40.0f);
}