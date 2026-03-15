//
// Created by fran on 03/03/2026.
//

#ifndef LANDCRAFT_CAMERA_HPP
#define LANDCRAFT_CAMERA_HPP

#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include "../utils/MathUtils.hpp"

class Camera {
public:
    /**
     * @brief Constructs the projection system.
     * @param tileSizeX Width of a tile in pixels.
     * @param tileSizeY Height of a tile in pixels.
     * @param heightScale Visual vertical offset per unit of height (Z).
     * @param projectionAngleX Angle of the X axis projection (in degrees).
     * @param projectionAngleY Angle of the Y axis projection (in degrees).
     */
    Camera(float tileSizeX, float tileSizeY, float heightScale, float projectionAngleX, float projectionAngleY);
    ~Camera();

    /**
     * Projects a 3D point from World Space (Tile Grid) to 2D Screen Space (Pixels).
     * Applies isometric projection, tile scaling, and height offset.
     *
     * @param point3dX The X index on the tile grid.
     * @param point3dY The Y index on the tile grid.
     * @param point3dZ The Z height/elevation of the tile.
     * @return The corresponding 2D coordinates on the screen (in pixels).
     */
    sf::Vector2f world_to_screen(float point3dX, float point3dY, float point3dZ) const;
    sf::Vector2f world_to_screen(sf::Vector2f point3d, float point3dZ) const;
    /**
     * Un-projects a 2D point from Screen Space (Pixels) back to 3D World Space (Tile Grid).
     * This is the inverse operation of world_to_screen.
     *
     * @param point2dX The X position on the screen (pixels).
     * @param point2dY The Y position on the screen (pixels).
     * @param point2dZ The assumed Z plane for the intersection (usually 0 for ground picking).
     * @return The unscaled world coordinates (tile indices).
     */
    sf::Vector2f screen_to_world(float point2dX, float point2dY, float point2dZ) const;

    /*
     * Sets the world pivot point in screen coordinates.
     * This is used to define a reference point for camera movement and rotation.
     * @param worldPivotScreenPosition The screen coordinates of the pivot point.
     */
    void setWorldPivotWithScreenPosition(sf::Vector2f worldPivotScreenPosition);
    void setWorldPivotWithWorldPosition(sf::Vector2f worldPivotWorldPosition);
    /**
     * @brief Returns the current world pivot in world coordinates.
     * @return The world pivot point in tile grid (world) coordinates.
     */
    sf::Vector2f getWorldPivotInWorldCoordinates() const;

    void update(float deltaTime, bool &hasMoved);
    void rotatePitch(float angle);
    void rotateYaw(float angle);
    void startContinuousRotation(sf::Vector2i mousePosition);
    void stopContinuousRotation();
    void updateContinuousRotation(sf::Vector2i mousePosition);

    float getTileSizeX() const { return m_tileSizeX; }
    float getTileSizeY() const { return m_tileSizeY; }

private:
    /**
     * @brief Rotates the map around the X axis by changing the projection angle.
     * It simulates a pitch rotation by altering the vertical projection of the tiles.
     *  this is the Pitch (tangage) rotation : This is the rotation around the X axis.
     *      -> This is what you will do: raise or lower the nose of the camera (look up or down).
     *      -> more on that here https://www.youtube.com/watch?v=pQ24NtnaLl8
     */
    void rotateAroundXAxis(float newProjectionAngleY);

    /**
     * @brief Rotates a 2D point around a specific center
     * This is the Yaw (lacet) rotation :  the rotation around the Z-axis (the vertical axis)
     *      -> it's similar to turning the head left or right (rotating the map on itself).
     * @param angle The rotation angle in RADIANS.
     * @param point The point to rotate.
     * @return The new rotated coordinates.
     */
    static sf::Vector2f rotateAroundZAxis(float angle, sf::Vector2f point, sf::Vector2f rotationCenter);
private:
    float m_projectionAngleX;
    float m_projectionAngleY;
    float m_tileSizeX;
    float m_tileSizeY;
    float m_heightScale;
    sf::Vector2f m_worldPivot;

    float m_pitchRotationSpeed;
    float m_currentPitchRotationAngle;
    float m_targetPitchRotationAngle;

    float m_yawRotationSpeed;
    float m_currentYawRotationAngle;
    float m_targetYawRotationAngle;
    float m_epsilon = 0.5f;

    sf::Vector2i m_mouseLastDragPosition;
    bool m_isDraggingForRotation;
    float m_continuousRotationSpeed;
};


#endif //LANDCRAFT_CAMERA_HPP