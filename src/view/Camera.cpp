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
    , m_mouseLastDragPosition({0, 0})
    , m_isDraggingForRotation(false)
    , m_continuousRotationSpeed(0.15f)
{
}

Camera::~Camera()
{
}

sf::Vector2f Camera::world_to_screen(const float point3dX, const float point3dY, const float point3dZ) const
{
    // apply current yaw rotation to the point3D
    const sf::Vector2f rotated3dPoint = rotateAroundZAxis(m_currentYawRotationAngle, sf::Vector2f(point3dX, point3dY), m_worldPivot);

    const float centeredWorldX = rotated3dPoint.x - m_worldPivot.x;
    const float centeredWorldY = rotated3dPoint.y - m_worldPivot.y;
    const float scaledWorldX = centeredWorldX * m_tileSizeX;
    const float scaledWorldY = centeredWorldY * m_tileSizeY;
    const float angleX = MathUtils::degToRad(m_projectionAngleX);
    const float angleY = MathUtils::degToRad(m_projectionAngleY);
    sf::Vector2f point2d;

    point2d.x = std::cos(angleX) * scaledWorldX - std::cos(angleX) * scaledWorldY ;
    point2d.y = std::sin(angleY) * scaledWorldY + std::sin(angleY) * scaledWorldX - point3dZ * m_heightScale;

    return point2d;
}

sf::Vector2f Camera::world_to_screen(const sf::Vector2f point3d, const float point3dZ) const
{
    return world_to_screen(point3d.x, point3d.y, point3dZ);
}

sf::Vector2f Camera::screen_to_world(const float point2dX, const float point2dY, const float point2dZ) const
{
    const float angleX = MathUtils::degToRad(m_projectionAngleX);
    const float angleY = MathUtils::degToRad(m_projectionAngleY);
    sf::Vector2f scaledPoint3d;

    scaledPoint3d.x = 0.5f * ((point2dX / std::cos(angleX)) + (point2dY + point2dZ * m_heightScale) / std::sin(angleY));
    scaledPoint3d.y = 0.5f * (-(point2dX / std::cos(angleX)) + (point2dY + point2dZ * m_heightScale) / std::sin(angleY));
    sf::Vector2f tempPos = sf::Vector2f(scaledPoint3d.x / m_tileSizeX, scaledPoint3d.y / m_tileSizeY) + m_worldPivot;
    return  rotateAroundZAxis(-m_currentYawRotationAngle, tempPos, m_worldPivot);
}

void Camera::setWorldPivotWithScreenPosition(const sf::Vector2f worldPivotScreenPosition)
{
    m_worldPivot = screen_to_world(worldPivotScreenPosition.x, worldPivotScreenPosition.y, 0);
}

void Camera::setWorldPivotWithWorldPosition(const sf::Vector2f worldPivotWorldPosition)
{
    m_worldPivot = worldPivotWorldPosition;
}

sf::Vector2f Camera::getWorldPivotInWorldCoordinates() const
{
    return m_worldPivot;
}

void Camera::update(const float deltaTime, bool &hasMoved)
{
    hasMoved = false;
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

    // upd yaw rotation
    if (std::abs(m_targetYawRotationAngle - m_currentYawRotationAngle) > m_epsilon) {
        m_currentYawRotationAngle = m_currentYawRotationAngle +
            (m_targetYawRotationAngle - m_currentYawRotationAngle) * m_yawRotationSpeed * deltaTime;
        hasMoved = true;
    } else
        if (m_currentYawRotationAngle != m_targetYawRotationAngle) {
            m_currentYawRotationAngle = m_targetYawRotationAngle;
            hasMoved = true;
        }
}

void Camera::rotatePitch(const float angle)
{
    m_targetPitchRotationAngle += angle;
    m_targetPitchRotationAngle = std::clamp(m_targetPitchRotationAngle, 5.0f, 90.0f);
}

void Camera::rotateYaw(const float angle)
{
    m_targetYawRotationAngle += angle;
}

void Camera::startContinuousRotation(const sf::Vector2i mousePosition)
{
    m_mouseLastDragPosition = mousePosition;
    m_isDraggingForRotation = true;
}

void Camera::stopContinuousRotation()
{
    m_isDraggingForRotation = false;
}

void Camera::updateContinuousRotation(const sf::Vector2i mousePosition)
{
    if (!m_isDraggingForRotation)
        return;
    // pitch
    const auto deltaY = static_cast<float>(mousePosition.y - m_mouseLastDragPosition.y);
    rotatePitch(deltaY * m_continuousRotationSpeed);

    // yaw
    const int deltaX = mousePosition.x - m_mouseLastDragPosition.x;
    m_currentYawRotationAngle += static_cast<float>(deltaX) * m_continuousRotationSpeed * -1;
    m_targetYawRotationAngle = m_currentYawRotationAngle;

    // update last mouse position
    m_mouseLastDragPosition = mousePosition;
}

void Camera::rotateAroundXAxis(const float newProjectionAngleY)
{
    m_projectionAngleY = newProjectionAngleY;
}

sf::Vector2f Camera::rotateAroundZAxis(const float angle, sf::Vector2f point, const sf::Vector2f rotationCenter)
{
    const float radAngle = MathUtils::degToRad(angle);
    sf::Vector2f rotatedPoint;
    point -= rotationCenter;

    // 2D rotation matrix applied
    rotatedPoint.x = point.x * std::cos(radAngle) - point.y * std::sin(radAngle);
    rotatedPoint.y = point.x * std::sin(radAngle) + point.y * std::cos(radAngle);
    return rotatedPoint + rotationCenter;
}
