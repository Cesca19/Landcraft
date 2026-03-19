//
// Created by fran on 15/03/2026.
//

#include "EnvironmentView.hpp"

EnvironmentView::EnvironmentView()
    : m_skyBox(sf::Quads, 4)
    , m_worldReferenceVertexArray(sf::Lines)
    , m_gizmoVertexArray(sf::Lines)
    , m_maxScreenViewRadius(200)
{
}

void EnvironmentView::init(const sf::Vector2u windowSize)
{
    initSkyBox(windowSize);
    initWorldGizmo();
}

void EnvironmentView::update(const Camera &camera, const sf::Vector2f viewCenter,
    const sf::Vector2f viewSize, const sf::Vector2f &gizmoPosition, float gizmoSize, const bool isViewMoving)
{
    updateWorldReference(camera, viewCenter, viewSize, isViewMoving);
    updateWorldGizmo(camera, gizmoPosition, gizmoSize, isViewMoving);
}

void EnvironmentView::drawSkyBox(sf::RenderWindow &window) const
{
    const sf::View previousView = window.getView();
    window.setView(window.getDefaultView());
    window.draw(m_skyBox);
    window.setView(previousView);
}

void EnvironmentView::drawWorldReference(sf::RenderWindow &window) const
{
    window.draw(m_worldReferenceVertexArray);
}

void EnvironmentView::drawWorldGizmo(sf::RenderWindow &window) const
{
    const sf::View previousView = window.getView();
    window.setView(window.getDefaultView());
    window.draw(m_gizmoVertexArray);
    window.setView(previousView);
}

void EnvironmentView::updateWorldReference(const Camera &camera, const sf::Vector2f viewCenter, const sf::Vector2f viewSize, const bool isViewMoving)
{
    if (!isViewMoving && m_worldReferenceVertexArray.getVertexCount() > 0)
        return;
    constexpr float height = 0.0f;
    // get the unrotated world coordinate of the view center
    sf::Vector2f viewCenterWorld = camera.screen_to_world(viewCenter.x, viewCenter.y, height);
    viewCenterWorld = {std::round(viewCenterWorld.x), std::round(viewCenterWorld.y)};
    // get the wireframe tile nb required to cover the entire screen
    const float screenViewDiagonalLength = std::hypot(viewSize.x, viewSize.y);
    const auto minTileScale = std::min(camera.getTileSizeX(), camera.getTileSizeY());
    // const float screenViewRadius = (std::round(screenViewDiagonalLength / minTileScale) * 2);
    const float calculatedRadius = std::round(screenViewDiagonalLength / minTileScale) * 2;
    const float screenViewRadius = std::min(calculatedRadius, 200.0f); // crash guard security
    const sf::Color linesColor(255, 255, 255, 50); // White semi-transparent lines (Wireframe)

    m_worldReferenceVertexArray.clear();
    // We create a non-rotated wireframe tile-grid
    // that will be rotated by the camera
    for (int i = - static_cast<int>(screenViewRadius); i <= static_cast<int>(screenViewRadius); i++) {
        const auto offset = static_cast<float>(i);
        // x axis parallel lines
        const sf::Vector2f startX(viewCenterWorld.x - screenViewRadius, viewCenterWorld.y + offset);
        const sf::Vector2f endX(viewCenterWorld.x + screenViewRadius, viewCenterWorld.y + offset);
        m_worldReferenceVertexArray.append(sf::Vertex(camera.world_to_screen(startX, height), linesColor));
        m_worldReferenceVertexArray.append(sf::Vertex(camera.world_to_screen(endX, height), linesColor));
        // y axis parallel lines
        const sf::Vector2f startY(viewCenterWorld.x + offset, viewCenterWorld.y - screenViewRadius);
        const sf::Vector2f endY(viewCenterWorld.x + offset, viewCenterWorld.y + screenViewRadius);
        m_worldReferenceVertexArray.append(sf::Vertex(camera.world_to_screen(startY, height), linesColor));
        m_worldReferenceVertexArray.append(sf::Vertex(camera.world_to_screen(endY, height), linesColor));
    }
}

void EnvironmentView::updateWorldGizmo(const Camera &camera, const sf::Vector2f &gizmoPosition,
    const float size, const bool isViewMoving)
{
    if (!isViewMoving)
        return;
    // projected origin point
    const sf::Vector2f origin = camera.world_to_screen(sf::Vector2f(0, 0), 0);
    const sf::Vector2f pX = camera.world_to_screen({1.0f, 0.0f}, 0);
    const sf::Vector2f pY = camera.world_to_screen({0.0f, 1.0f}, 0);
    // Z axis isn't rotated
    const sf::Vector2f pZ = camera.world_to_screen(sf::Vector2f(0, 0), 1.0f);
    // normalized direction vectors
    sf::Vector2f dirX = MathUtils::normalize({pX - origin});
    sf::Vector2f dirY = MathUtils::normalize({pY - origin});
    sf::Vector2f dirZ = MathUtils::normalize({pZ - origin});
    
    // scaling dir vectors
    dirX *= size;
    dirY *= size;
    dirZ *= size;
    // x axis
    m_gizmoVertexArray[0].position = gizmoPosition;
    m_gizmoVertexArray[1].position = gizmoPosition + dirX;
    // y axis
    m_gizmoVertexArray[2].position = gizmoPosition;
    m_gizmoVertexArray[3].position = gizmoPosition + dirY;
    // Z axis
    m_gizmoVertexArray[4].position = gizmoPosition;
    m_gizmoVertexArray[5].position = gizmoPosition + dirZ;
}

void EnvironmentView::initSkyBox(const sf::Vector2u windowSize)
{
    // may be create a shader and add some particles for night or day
    // sf::Color bottomColor(120, 72, 153);   // purple
    // sf::Color topColor(255, 179, 193);  // pink
    sf::Color bottomColor(255, 179, 193);  // pink
    sf::Color topColor(196, 218, 242);

    m_skyBox[0].position = sf::Vector2f(0, 0);
    m_skyBox[0].color = topColor;
    m_skyBox[1].position = sf::Vector2f(windowSize.x, 0);
    m_skyBox[1].color = topColor;
    m_skyBox[2].position = sf::Vector2f(windowSize.x, windowSize.y);
    m_skyBox[2].color = bottomColor;
    m_skyBox[3].position = sf::Vector2f(0, windowSize.y);
    m_skyBox[3].color = bottomColor;
}

void EnvironmentView::initWorldGizmo()
{
    m_gizmoVertexArray.resize(6);
    m_gizmoVertexArray[0].color = sf::Color::Red;
    m_gizmoVertexArray[1].color = sf::Color:: Red;
    m_gizmoVertexArray[2].color = sf::Color::Green;
    m_gizmoVertexArray[3].color = sf::Color::Green;
    m_gizmoVertexArray[4].color = sf::Color::Blue;
    m_gizmoVertexArray[5].color = sf::Color::Blue;
}
