//
// Created by fran on 07/02/2026.
//

#include "IsometricProjection.hpp"

IsometricProjection::IsometricProjection(const int tileSizeX, const int tileSizeY, const int heightScale,
    const int projectionAngleX, const int projectionAngleY)
    : m_projectionAngleX(projectionAngleX)
    , m_projectionAngleY(projectionAngleY)
    , m_tileSizeX(tileSizeX)
    , m_tileSizeY(tileSizeY)
    , m_heightScale(heightScale)
{
}

IsometricProjection::~IsometricProjection()
{
}

float IsometricProjection::radToDeg(float rad)
{
    return rad * 180 / M_PI;
}

float IsometricProjection::degToRad(float deg)
{
    return deg * M_PI / 180;
}

float IsometricProjection::distanceBetweenPoints(const sf::Vector2f &p1, const sf::Vector2f &p2)
{
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

sf::Vector2f IsometricProjection::world_to_screen(const float point3dX, const float point3dY, const float point3dZ) const
{
    const float scaledWorldX = point3dX * m_tileSizeX;
    const float scaledWorldY = point3dY * m_tileSizeY;
    const float angleX = degToRad(m_projectionAngleX);
    const float angleY = degToRad(m_projectionAngleY);
    sf::Vector2f point2d;

    point2d.x = std::cos(angleX) * scaledWorldX - std::cos(angleX) * scaledWorldY ;
    point2d.y = std::sin(angleY) * scaledWorldY + std::sin(angleY) * scaledWorldX - point3dZ * m_heightScale;
    return point2d;
}

sf::Vector2f IsometricProjection::screen_to_world(const int point2dX, const int point2dY, const int point2dZ) const
{
    const float angleX = degToRad(m_projectionAngleX);
    const float angleY = degToRad(m_projectionAngleY);
    sf::Vector2f scaledPoint3d;

    scaledPoint3d.x = 0.5f * ((point2dX / std::cos(angleX)) + (point2dY + point2dZ * m_heightScale) / std::sin(angleY));
    scaledPoint3d.y = 0.5f * (-(point2dX / std::cos(angleX)) + (point2dY + point2dZ * m_heightScale) / std::sin(angleY));
    return {scaledPoint3d.x / m_tileSizeX, scaledPoint3d.y / m_tileSizeY};
}

sf::Vector2f IsometricProjection::getPointScreenPosition(const sf::Vector2f worldPosition, const int worldHeight) const
{
    return world_to_screen(worldPosition.x, worldPosition.y, worldHeight);/* + m_translationOffset*/
}

sf::Vector2f IsometricProjection::rotateAroundZAxis(const float angle, const sf::Vector2f point)
{
    const float radAngle = degToRad(angle);
    sf::Vector2f rotatedPoint;

    // 2D rotation matrix applied
    rotatedPoint.x = point.x * std::cos(radAngle) - point.y * std::sin(radAngle);
    rotatedPoint.y = point.x * std::sin(radAngle) + point.y * std::cos(radAngle);
    return rotatedPoint;
}

void IsometricProjection::rotateAroundXAxis(const int newProjectionAngleY)
{
    m_projectionAngleY = newProjectionAngleY;
}
