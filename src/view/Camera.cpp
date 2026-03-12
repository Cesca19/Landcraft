//
// Created by fran on 03/03/2026.
//

#include "Camera.hpp"

Camera::Camera(const float tileSizeX, const float tileSizeY, const float heightScale, const float projectionAngleX, const float projectionAngleY)
    : m_projectionAngleX(projectionAngleX)
    , m_projectionAngleY(projectionAngleY)
    , m_tileSizeX(tileSizeX)
    , m_tileSizeY(tileSizeY)
    , m_heightScale(heightScale)
    , m_worldPivot({0, 0})
    , m_pitchRotationSpeed(20)
    , m_currentPitchRotationAngle(projectionAngleY)
    , m_targetPitchRotationAngle(projectionAngleY)
    , m_yawRotationSpeed(10)
    , m_currentYawRotationAngle(0)
    , m_targetYawRotationAngle(0)
{
}

Camera::~Camera()
{
}

sf::Vector2f Camera::world_to_screen(float point3dX, float point3dY, float point3dZ) const
{
    const float centeredWorldX = point3dX - m_worldPivot.x;
    const float centeredWorldY = point3dY - m_worldPivot.y;
    const float scaledWorldX = centeredWorldX * m_tileSizeX;
    const float scaledWorldY = centeredWorldY * m_tileSizeY;
    const float angleX = MathUtils::degToRad(m_projectionAngleX);
    const float angleY = MathUtils::degToRad(m_projectionAngleY);
    sf::Vector2f point2d;

    point2d.x = std::cos(angleX) * scaledWorldX - std::cos(angleX) * scaledWorldY ;
    point2d.y = std::sin(angleY) * scaledWorldY + std::sin(angleY) * scaledWorldX - point3dZ * m_heightScale;
    return point2d;
}

sf::Vector2f Camera::screen_to_world(float point2dX, float point2dY, float point2dZ) const
{
    const float angleX = MathUtils::degToRad(m_projectionAngleX);
    const float angleY = MathUtils::degToRad(m_projectionAngleY);
    sf::Vector2f scaledPoint3d;

    scaledPoint3d.x = 0.5f * ((point2dX / std::cos(angleX)) + (point2dY + point2dZ * m_heightScale) / std::sin(angleY));
    scaledPoint3d.y = 0.5f * (-(point2dX / std::cos(angleX)) + (point2dY + point2dZ * m_heightScale) / std::sin(angleY));
    return sf::Vector2f(scaledPoint3d.x / m_tileSizeX, scaledPoint3d.y / m_tileSizeY) + m_worldPivot;
}

void Camera::setWorldPivot(const sf::Vector2f worldPivotScreenPosition)
{
    m_worldPivot = screen_to_world(worldPivotScreenPosition.x, worldPivotScreenPosition.y, 0);
}

sf::Vector2f Camera::getWorldPivotInWorldCoordinates() const
{
    return m_worldPivot;
}

bool Camera::update(const float deltaTime)
{
    bool hasMoved = false;
    //upd pitch rotation
    if (std::abs(m_targetPitchRotationAngle - m_currentPitchRotationAngle) > m_epsilon) {
        m_currentPitchRotationAngle = m_currentPitchRotationAngle +
                                        (m_targetPitchRotationAngle - m_currentPitchRotationAngle)
                                        * m_pitchRotationSpeed * deltaTime;
        rotateAroundXAxis(m_currentPitchRotationAngle);
        hasMoved = true;
    } else
        if (m_currentPitchRotationAngle != m_targetPitchRotationAngle) {
            m_currentPitchRotationAngle = m_targetPitchRotationAngle;
            rotateAroundXAxis(m_currentPitchRotationAngle);
            hasMoved = true;
        }
    return hasMoved;
}

void Camera::rotatePitch(const float angle)
{
    m_targetPitchRotationAngle += angle;
}

void Camera::rotateYaw(float angle)
{
}

void Camera::rotateAroundXAxis(const float newProjectionAngleY)
{
    m_projectionAngleY = newProjectionAngleY;
}