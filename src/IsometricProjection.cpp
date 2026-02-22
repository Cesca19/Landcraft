//
// Created by fran on 07/02/2026.
//

#include "IsometricProjection.hpp"

IsometricProjection::IsometricProjection(const float tileSizeX, const float tileSizeY, const float heightScale,
    const float projectionAngleX, const float projectionAngleY)
    : m_projectionAngleX(projectionAngleX)
    , m_projectionAngleY(projectionAngleY)
    , m_tileSizeX(tileSizeX)
    , m_tileSizeY(tileSizeY)
    , m_heightScale(heightScale)
    , m_worldPivot({0, 0})
{
}

IsometricProjection::~IsometricProjection()
{
}

float IsometricProjection::radToDeg(const float rad)
{
    return rad * 180 / M_PI;
}

float IsometricProjection::degToRad(const float deg)
{
    return deg * M_PI / 180;
}

float IsometricProjection::distanceBetweenPoints(const sf::Vector2f &p1, const sf::Vector2f &p2)
{
    return static_cast<float>(std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2)));
}

sf::Vector2f IsometricProjection::world_to_screen(const float point3dX, const float point3dY, const float point3dZ) const
{
    const float centeredWorldX = point3dX - m_worldPivot.x;
    const float centeredWorldY = point3dY - m_worldPivot.y;
    const float scaledWorldX = centeredWorldX * m_tileSizeX;
    const float scaledWorldY = centeredWorldY * m_tileSizeY;
    const float angleX = degToRad(m_projectionAngleX);
    const float angleY = degToRad(m_projectionAngleY);
    sf::Vector2f point2d;

    point2d.x = std::cos(angleX) * scaledWorldX - std::cos(angleX) * scaledWorldY ;
    point2d.y = std::sin(angleY) * scaledWorldY + std::sin(angleY) * scaledWorldX - point3dZ * m_heightScale;
    return point2d;
}

sf::Vector2f IsometricProjection::screen_to_world(const float point2dX, const float point2dY, const float point2dZ) const
{
    const float angleX = degToRad(m_projectionAngleX);
    const float angleY = degToRad(m_projectionAngleY);
    sf::Vector2f scaledPoint3d;

    scaledPoint3d.x = 0.5f * ((point2dX / std::cos(angleX)) + (point2dY + point2dZ * m_heightScale) / std::sin(angleY));
    scaledPoint3d.y = 0.5f * (-(point2dX / std::cos(angleX)) + (point2dY + point2dZ * m_heightScale) / std::sin(angleY));
    return sf::Vector2f(scaledPoint3d.x / m_tileSizeX, scaledPoint3d.y / m_tileSizeY) + m_worldPivot;
}

sf::Vector2f IsometricProjection::getPointScreenPosition(const sf::Vector2f worldPosition, const float worldHeight) const
{
    return world_to_screen(worldPosition.x, worldPosition.y, worldHeight);
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

void IsometricProjection::rotateAroundXAxis(const float newProjectionAngleY)
{
    m_projectionAngleY = newProjectionAngleY;
}

void IsometricProjection::setWorldPivot(const sf::Vector2f worldPivotScreenPosition)
{
    m_worldPivot = screen_to_world(worldPivotScreenPosition.x, worldPivotScreenPosition.y, 0);
}

sf::Vector2f IsometricProjection::getWorldPivotInWorldCoordinates() const
{
    return m_worldPivot;
}

float IsometricProjection::dotProduct(const sf::Vector2f &v1, const sf::Vector2f &v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

float IsometricProjection::magnitude(const sf::Vector2f &v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

sf::Vector2f IsometricProjection::normalize(const sf::Vector2f &v)
{
    float mag = magnitude(v);
    if (mag == 0.f)
        return {0.f, 0.f};
    return v / mag;
}

sf::Vector2f IsometricProjection::projectPointOnLine(const sf::Vector2f &point, const sf::Vector2f &linePoint, const sf::Vector2f &direction)
{
    const sf::Vector2f AP = point - linePoint;
    const float dot = dotProduct(AP, direction);
    const float magSq = dotProduct(direction, direction);

    if (magSq == 0.f)
        return linePoint;

    const sf::Vector2f projection = (dot / magSq) * direction;
    return linePoint + projection;
}

sf::Vector2f IsometricProjection::offsetPointAlongDirection(const sf::Vector2f &point, const sf::Vector2f &direction, const float radius)
{
    const sf::Vector2f normalizedDirection = direction / magnitude(direction);
    return point + normalizedDirection * radius;
}