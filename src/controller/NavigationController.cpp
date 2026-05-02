//
// Created by fran on 17/03/2026.
//

#include "NavigationController.hpp"

NavigationController::NavigationController(WorldModel &model, WorldView &view, const sf::Vector2f globalUIPosition)
    : m_movementStep(200)
    , m_zoomStep(1)
    , m_pitchRotationStep(5)
    , m_yawRotationStep(22.5)
{
    m_recenterViewButton = UIFactory::createSpriteButton("assets/textures/ui/recenter_view_512.png",
       globalUIPosition  + sf::Vector2f(155, 16), {28, 28}, "Recenter", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_recenterViewButton, HighlightTextAlign::Down);
    m_recenterViewButton->initOnClickCallback([&view] {
       view.recenter();
    });
}

NavigationController::~NavigationController()
{
    UIFactory::removeWidget(m_recenterViewButton);
}

void NavigationController::handleEvents(const sf::RenderWindow &window, const sf::Event &event, WorldModel &model,
                                        WorldView &view, const bool isEditing)
{
    handlePanEvents(window, event, view);
    if (!isEditing) {
        handleZoomEvents(window, event, view);
        handleRotationEvents(window, event, view);
    }
}

void NavigationController::handleContinuousEvents(const float deltaTime, WorldView &view) const
{
    handleContinuousPanEvents(deltaTime, view);
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

void NavigationController::handleContinuousPanEvents(const float deltaTime, WorldView& view) const
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
        view.moveTarget(moveVector * m_movementStep * deltaTime);
    }
}

void NavigationController::handleRotationEvents(const sf::RenderWindow &window, const sf::Event &event, WorldView& view) const
{
    // mouse
    // right button + vertical / horizontal scroll
    // this might cause problems  when selecting objects in the future
    constexpr sf::Mouse::Button mouseButton = sf::Mouse::Right;
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == mouseButton)
        view.startContinuousRotation(window);
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == mouseButton)
        view.stopContinuousRotation(); 
    if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(mouseButton))
        view.updateContinuousRotation(window);

    // keyboard
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            // yaw
            case sf::Keyboard::K:
                view.rotateYaw(m_yawRotationStep);
                break;
            case sf::Keyboard::M:
                view.rotateYaw(-m_yawRotationStep);
                break;
            // pitch
            case sf::Keyboard::O:
                view.rotatePitch(m_pitchRotationStep);
                break;
            case sf::Keyboard::L:
                view.rotatePitch(-m_pitchRotationStep);
                break;
            // recenter
            case sf::Keyboard::R:
                view.recenter();
                break;
            default:
                break;
        }
    }
    // TO DO : add gizmo axes click like blender
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
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P)
        view.zoom(m_zoomStep);
}
