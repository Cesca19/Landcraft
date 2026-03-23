//
// Created by fran on 17/03/2026.
//

#include "NavigationController.hpp"

NavigationController::NavigationController()
    : m_movementStep(2)
    , m_zoomStep(1)
    , m_pitchRotationStep(5)
    , m_yawRotationStep(22.5)
    , m_isRotating(false)
    , m_isMovementKeyPressed(false)
{
}

void NavigationController::handleEvents(const sf::RenderWindow &window, const sf::Event &event, WorldModel &model,
    WorldView &view)
{
    handlePanEvents(window, event, view);
    handleZoomEvents(window, event, view);
    handleRotationEvents(window, event, model, view);
}

void NavigationController::handleContinuousEvents(WorldView &view)
{
    handleContinuousPanEvents(view);
}

bool NavigationController::isNavigating(const WorldView &view) const
{
    return m_isMovementKeyPressed || m_isRotating || view.isMoving();
}

void NavigationController::resetKeyPressedEvents()
{
    m_isMovementKeyPressed = false;
}

void NavigationController::handlePanEvents(const sf::RenderWindow &window, const sf::Event &event, WorldView& view)
{
    //  drag and drop with middle mouse button
    constexpr sf::Mouse::Button mouseButton = sf::Mouse::Middle;
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == mouseButton)
        view.startDragging(window);
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == mouseButton)
        view.stopDragging();
    if (event.type == sf::Event::MouseMoved)
        view.updateDragging(window);
}

void NavigationController::handleContinuousPanEvents(WorldView& view)
{
    if (const bool isCtrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) return;
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
        // float currentZoom = view->getTargetZoom();
        // float adjustedSpeed = m_movementStep * currentZoom;
        view.moveTarget(moveVector * m_movementStep);
        m_isMovementKeyPressed = true;
    }
}

void NavigationController::handleRotationEvents(const sf::RenderWindow &window, const sf::Event &event, WorldModel& model, const WorldView& view)
{
    // mouse
    // right button + vertical / horizontal scroll
    // this might cause problems  when selecting objects in the future
    constexpr sf::Mouse::Button mouseButton = sf::Mouse::Right;
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == mouseButton) {   
        view.startContinuousRotation(window);
        m_isRotating = true;
    }
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == mouseButton) {
        view.stopContinuousRotation(); 
        m_isRotating = false;
    }
    if (event.type == sf::Event::MouseMoved)
        view.updateContinuousRotation(window, model.getTiles());

    // keyboard
    // yaw
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
        view.rotateYaw(m_yawRotationStep);
        m_isMovementKeyPressed = true;
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
        view.rotateYaw(-m_yawRotationStep);
        m_isMovementKeyPressed = true;
    }
    // pitch
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
        view.rotatePitch(m_pitchRotationStep);
        m_isMovementKeyPressed = true;
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F) {
        view.rotatePitch(-m_pitchRotationStep);
        m_isMovementKeyPressed = true;
    }
    
    // add gizmo axes click like blender
}

void NavigationController::handleZoomEvents(const sf::RenderWindow &window, const sf::Event &event, WorldView &view) const
{
    // mouse
    // zoom with mouse wheel at mouse position
    // Note: We handle this separately from the keyboard zoom to allow for zooming at the mouse position.
    const bool isCtrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
    if (event.type == sf::Event::MouseWheelScrolled)
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel && !isCtrlPressed)
            view.zoomAtMouse(window, event.mouseWheelScroll.delta);

    // keyboard
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::I)
        view.zoom(-m_zoomStep);
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::O)
        view.zoom(m_zoomStep);
}
