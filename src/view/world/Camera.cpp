//
// Created by fran on 03/03/2026.
//

#include "Camera.hpp"

Camera::Camera(const float tileSizeX, const float tileSizeY, const float heightScale, const float projectionAngleX, const float projectionAngleY)
    : m_projectionAngleX(projectionAngleX)
    , m_projectionAngleY(projectionAngleY)
    , m_projectionCosX(1.0f)
    , m_projectionSinY(1.0f)
    , m_tileSizeX(tileSizeX)
    , m_tileSizeY(tileSizeY)
    , m_heightScale(heightScale)
    , m_worldPivot({0, 0})
    , m_pitchRotationSpeed(20)
    , m_currentPitchRotationAngle(projectionAngleY)
    , m_targetPitchRotationAngle(projectionAngleY)
    , m_defaultPitchRotationAngle(projectionAngleY)
    , m_yawRotationSpeed(10)
    , m_currentYawRotationAngle(0)
    , m_targetYawRotationAngle(0)
    , m_yawCos(1.0f)
    , m_yawSin(0.0f)
    , m_mouseLastDragPosition({0, 0})
    , m_isDraggingForRotation(false)
    , m_continuousRotationSpeed(0.15f)
    , m_isRotating(false)
{
    refreshProjectionCache();
    refreshYawCache();
}

Camera::~Camera()
{
}

sf::Vector2f Camera::world_to_screen(const float point3dX, const float point3dY, const float point3dZ) const
{
    const float relativeX = point3dX - m_worldPivot.x;
    const float relativeY = point3dY - m_worldPivot.y;
    const float centeredWorldX = relativeX * m_yawCos - relativeY * m_yawSin;
    const float centeredWorldY = relativeX * m_yawSin + relativeY * m_yawCos;
    const float scaledWorldX = centeredWorldX * m_tileSizeX;
    const float scaledWorldY = centeredWorldY * m_tileSizeY;
    sf::Vector2f point2d;

    point2d.x = m_projectionCosX * (scaledWorldX - scaledWorldY);
    point2d.y = m_projectionSinY * (scaledWorldY + scaledWorldX) - point3dZ * m_heightScale;

    return point2d;
}

sf::Vector2f Camera::world_to_screen(const sf::Vector2f point3d, const float point3dZ) const
{
    return world_to_screen(point3d.x, point3d.y, point3dZ);
}

sf::Vector2f Camera::screen_to_world(const float point2dX, const float point2dY, const float point2dZ) const
{
    sf::Vector2f scaledPoint3d;

    scaledPoint3d.x = 0.5f * ((point2dX / m_projectionCosX) + (point2dY + point2dZ * m_heightScale) / m_projectionSinY);
    scaledPoint3d.y = 0.5f * (-(point2dX / m_projectionCosX) + (point2dY + point2dZ * m_heightScale) / m_projectionSinY);
    sf::Vector2f tempPos = sf::Vector2f(scaledPoint3d.x / m_tileSizeX, scaledPoint3d.y / m_tileSizeY) + m_worldPivot;

    const float rotatedX = tempPos.x - m_worldPivot.x;
    const float rotatedY = tempPos.y - m_worldPivot.y;
    return {
        rotatedX * m_yawCos + rotatedY * m_yawSin + m_worldPivot.x,
        -rotatedX * m_yawSin + rotatedY * m_yawCos + m_worldPivot.y
    };
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

void Camera::update(const float deltaTime)
{
    m_isRotating = m_isDraggingForRotation;
    //upd pitch rotation
    if (std::abs(m_targetPitchRotationAngle - m_currentPitchRotationAngle) > m_epsilon) {
        m_currentPitchRotationAngle = m_currentPitchRotationAngle +
                                        (m_targetPitchRotationAngle - m_currentPitchRotationAngle)
                                        * m_pitchRotationSpeed * deltaTime;
        rotateAroundXAxis(m_currentPitchRotationAngle);
        m_isRotating = true;
    } else
        if (m_currentPitchRotationAngle != m_targetPitchRotationAngle) {
            m_currentPitchRotationAngle = m_targetPitchRotationAngle;
            rotateAroundXAxis(m_currentPitchRotationAngle);
            m_isRotating = true;
        }

    // upd yaw rotation
    if (std::abs(m_targetYawRotationAngle - m_currentYawRotationAngle) > m_epsilon) {
        m_currentYawRotationAngle = m_currentYawRotationAngle +
            (m_targetYawRotationAngle - m_currentYawRotationAngle) * m_yawRotationSpeed * deltaTime;
        refreshYawCache();
        m_isRotating = true;
    } else
        if (m_currentYawRotationAngle != m_targetYawRotationAngle) {
            m_currentYawRotationAngle = m_targetYawRotationAngle;
            refreshYawCache();
            m_isRotating = true;
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
    refreshYawCache();

    // update last mouse position
    m_mouseLastDragPosition = mousePosition;
}

void Camera::resetRotation()
{
    m_targetPitchRotationAngle = m_defaultPitchRotationAngle;
    m_targetYawRotationAngle = 0;
}

void Camera::rotateAroundXAxis(const float newProjectionAngleY)
{
    m_projectionAngleY = newProjectionAngleY;
    refreshProjectionCache();
}

void Camera::refreshProjectionCache()
{
    const float angleX = MathUtils::degToRad(m_projectionAngleX);
    const float angleY = MathUtils::degToRad(m_projectionAngleY);
    m_projectionCosX = std::cos(angleX);
    m_projectionSinY = std::sin(angleY);
}

void Camera::refreshYawCache()
{
    const float radAngle = MathUtils::degToRad(m_currentYawRotationAngle);
    m_yawCos = std::cos(radAngle);
    m_yawSin = std::sin(radAngle);
}
