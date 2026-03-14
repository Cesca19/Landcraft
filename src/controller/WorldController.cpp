//
// Created by fran on 11/03/2026.
//

#include "WorldController.hpp"

WorldController::WorldController()
    : m_movementStep(0.5f)
    , m_zoomStep(1)
    , m_pitchRotationStep(5)
    , m_yawRotationStep(22.5)
    , m_heightStep(1)
    , m_isRotating(false)
    , m_isMovementKeyPressed(false)
    , m_currentSelectionMode(SelectionMode::TILE_CORNER)
{
}

void WorldController::init(const std::string &mapName, 
        const CameraSettings& cameraSettings, const ViewSettings& viewSettings)
{
    m_worldModel.loadMap(mapName);
    m_worldView.init(viewSettings.center, viewSettings.size);
    m_worldView.initCamera(cameraSettings.tileSizeX, cameraSettings.tileSizeY, cameraSettings.heightScale,
        cameraSettings.projectionAngleX, cameraSettings.projectionAngleY, m_worldModel.getCenter());
    m_worldView.initTileMap(m_worldModel.getTiles());
    m_worldView.zoom(10);
}

void WorldController::handleEvents(sf::RenderWindow &window)
{
    sf::Event event;
    m_isMovementKeyPressed = false;

    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed 
            || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
            window.close();
        
        handlePanMouseEvents(window, event);
        handleRotationEvents(window, event);
        handleZoomEvents(window, event);
        handleMapEditingEvents(window, event);
    }
    handlePanKeyboardEvents();
}

void WorldController::update(const float deltaTime, sf::RenderWindow &window)
{
    m_worldView.update(deltaTime, m_worldModel.getTiles());
    bool hasModelChanged = false;
    if (!m_isMovementKeyPressed && !m_isRotating && !m_worldView.isMoving())
        m_selectionController.update(deltaTime, window, m_currentSelectionMode,
            m_worldModel, m_worldView.getCamera(), hasModelChanged);
}

void WorldController::draw(sf::RenderWindow &window)
{
    m_worldView.draw(window);
    if (!m_isMovementKeyPressed && !m_isRotating && !m_worldView.isMoving()) // only draw selection when not panning or rotating to avoid visual clutter
        m_selectionController.draw(window, m_worldView.getCamera());
}

void WorldController::handlePanMouseEvents(const sf::RenderWindow& window, const sf::Event &event)
{
    //  drag and drop with middle mouse button
    constexpr sf::Mouse::Button mouseButton = sf::Mouse::Middle;
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == mouseButton)
            m_worldView.startDragging(window);
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == mouseButton)
            m_worldView.stopDragging();
    if (event.type == sf::Event::MouseMoved)
        m_worldView.updateDragging(window);
}

void WorldController::handlePanKeyboardEvents()
{
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
        m_isMovementKeyPressed = true;
    }

}

void WorldController::handleRotationEvents(sf::RenderWindow& window, const sf::Event &event)
{
    // mouse
    // left button + vertical / horizontal scroll
    // this might cause problems  when selecting objects in the future
    constexpr sf::Mouse::Button mouseButton = sf::Mouse::Left;
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == mouseButton) {   
        m_worldView.startContinuousRotation(window);
        m_isRotating = true;
    }
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == mouseButton) {
        m_worldView.stopContinuousRotation(); 
        m_isRotating = false;
    }
    if (event.type == sf::Event::MouseMoved)
        m_worldView.updateContinuousRotation(window, m_worldModel.getTiles());

    // keyboard
    // yaw
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
        m_worldView.rotateYaw(m_yawRotationStep);
        m_isMovementKeyPressed = true;
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
        m_worldView.rotateYaw(-m_yawRotationStep);
        m_isMovementKeyPressed = true;
    }
    // pitch
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
        m_worldView.rotatePitch(m_pitchRotationStep);
        m_isMovementKeyPressed = true;
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F) {
        m_worldView.rotatePitch(-m_pitchRotationStep);
        m_isMovementKeyPressed = true;
    }
    
    // add gizmo axes click like blender
}

void WorldController::handleZoomEvents(const sf::RenderWindow& window, const sf::Event &event)
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
    // keyboard
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
        m_currentSelectionMode = (m_currentSelectionMode == SelectionMode::TILE)
                        ? SelectionMode::TILE_CORNER
                        : SelectionMode::TILE;
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Add)
        updateSelectedCornersHeight(m_heightStep);
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Subtract)
        updateSelectedCornersHeight(- m_heightStep);

    // mouse
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
        && event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
        updateSelectedCornersHeight(m_heightStep * static_cast<int>(event.mouseWheelScroll.delta));
}

void WorldController::updateSelectedCornersHeight(const int heightStep)
{
    const std::vector<TileCorner *> corners = m_selectionController.getSelectedTileCorners();
    for (TileCorner * corner: corners)
        corner->addHeight(heightStep);
    m_worldView.updateTileCorners(m_worldModel.getTiles(), corners);
}