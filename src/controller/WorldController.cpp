//
// Created by fran on 11/03/2026.
//

#include "WorldController.hpp"

WorldController::WorldController()
    : m_movementStep(5.f)
    , m_zoomStep(1)
{
}

void WorldController::init(const std::string &mapName, 
        const CameraSettings& cameraSettings, const ViewSettings& viewSettings)
{
    m_worldModel.loadMap(mapName);
    m_worldView.init(viewSettings.center, viewSettings.size);
    m_worldView.initCamera(cameraSettings.tileSizeX, cameraSettings.tileSizeY, cameraSettings.heightScale, cameraSettings.projectionAngleX, cameraSettings.projectionAngleY);
    m_worldView.initTileMap(m_worldModel.getTiles());
    m_worldView.zoom(10);
}

void WorldController::handleEvents(sf::RenderWindow &window)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed 
            || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
            window.close();
        handlePanEvents(window, event);
        handleRotationEvents(window, event);
        handleZoomEvents(window, event);
        handleMapEditingEvents(window, event);
    }
}

void WorldController::update(float deltaTime)
{
    m_worldView.update(deltaTime);
}

void WorldController::draw(sf::RenderWindow &window)
{
    m_worldView.draw(window);
}

void WorldController::handlePanEvents(sf::RenderWindow& window, const sf::Event &event)
{
    // mouse
    //  drag and drop with middle mouse button
    constexpr sf::Mouse::Button mouseButton = sf::Mouse::Middle;
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == mouseButton)
            m_worldView.startDragging(window);
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == mouseButton)
            m_worldView.stopDragging();
    if (event.type == sf::Event::MouseMoved)
        m_worldView.updateDragging(window);

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
        moveVector = MathUtils::normalize(moveVector);
        // adapt movement speed based on zoom level to maintain a consistent feel
        // float currentZoom = m_worldView->getTargetZoom();
        // float adjustedSpeed = m_movementStep * currentZoom;
        m_worldView.moveTarget(moveVector * m_movementStep);
    }
}

void WorldController::handleRotationEvents(sf::RenderWindow& window, const sf::Event &event)
{
}

void WorldController::handleZoomEvents(sf::RenderWindow& window, const sf::Event &event)
{
    // mouse
    // zoom with mouse wheel at mouse position
    // Note: We handle this separately from the keyboard zoom to allow for zooming at the mouse position.
    const bool isCtrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
    if (event.type == sf::Event::MouseWheelScrolled)
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel && !isCtrlPressed)
            m_worldView.zoomAtMouse(window, event.mouseWheelScroll.delta);

    // keyboard
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::I)
        m_worldView.zoom(-m_zoomStep);
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::O)
        m_worldView.zoom(m_zoomStep);
}

void WorldController::handleMapEditingEvents(sf::RenderWindow& window, const sf::Event &event)
{
}
