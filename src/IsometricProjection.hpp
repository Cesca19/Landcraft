//
// Created by fran on 07/02/2026.
//

#ifndef LANDCRAFT_ISOMETRICPROJECTION_HPP
#define LANDCRAFT_ISOMETRICPROJECTION_HPP
#define _USE_MATH_DEFINES

#include <cmath>
#include <SFML/Graphics.hpp>

/**
 * @brief Helper class responsible for all isometric projection calculations,
 * coordinate system conversions, and geometric transformations.
 */
class IsometricProjection
{
public:
    /**
     * @brief Constructs the projection system.
     * @param tileSizeX Width of a tile in pixels.
     * @param tileSizeY Height of a tile in pixels.
     * @param heightScale Visual vertical offset per unit of height (Z).
     * @param projectionAngleX Angle of the X axis projection (in degrees).
     * @param projectionAngleY Angle of the Y axis projection (in degrees).
     */
    IsometricProjection(float tileSizeX, float tileSizeY, float heightScale, float projectionAngleX, float projectionAngleY);
    ~IsometricProjection();

    static float radToDeg(float rad);
    static float degToRad(float deg);
    static float distanceBetweenPoints(const sf::Vector2f &p1, const sf::Vector2f &p2);

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

    /**
     * @brief Gets the screen position of a world point with a given height.
     * @param worldPosition The X and Y tile indices in world space.
     * @param worldHeight The Z height/elevation of the tile.
     * @return The corresponding 2D coordinates on the screen (in pixels).
     */
    sf::Vector2f getPointScreenPosition(sf::Vector2f worldPosition, float worldHeight) const;

    /**
     * @brief Rotates a 2D point around the origin (0,0).
     * To rotate around a specific center, subtract the center before calling this,
     * then add it back to the result.
     * This is the Yaw (lacet) rotation :  the rotation around the Z-axis (the vertical axis)
     *      -> it's similar to turning the head left or right (rotating the map on itself).
     * @param angle The rotation angle in RADIANS.
     * @param point The point to rotate.
     * @return The new rotated coordinates.
     */
    static sf::Vector2f rotateAroundZAxis(float angle, sf::Vector2f point);

    /**
     * @brief Rotates the map around the X axis by changing the projection angle.
     * It simulates a pitch rotation by altering the vertical projection of the tiles.
     *  this is the Pitch (tangage) rotation : This is the rotation around the X axis.
     *      -> This is what you will do: raise or lower the nose of the camera (look up or down).
     *      -> more on that here https://www.youtube.com/watch?v=pQ24NtnaLl8
     */
    void rotateAroundXAxis(float newProjectionAngleY);

    /*
     * Sets the world pivot point in screen coordinates.
     * This is used to define a reference point for camera movement and rotation.
     * @param worldPivotScreenPosition The screen coordinates of the pivot point.
     */
    void setWorldPivot(sf::Vector2f worldPivotScreenPosition);

    /** 
     * @brief Returns the current world pivot in world coordinates. 
     * @return The world pivot point in tile grid (world) coordinates.
     */
    sf::Vector2f getWorldPivotInWorldCoordinates() const;

    /**
     * @brief Calculates the dot product of two 2D vectors.
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @return The dot product of the two vectors.
     */
    static float dotProduct(const sf::Vector2f &v1, const sf::Vector2f &v2);

    /**
     * @brief Calculates the magnitude (length) of a 2D vector.
     * @param v The vector for which to calculate the magnitude.
     * @return The magnitude of the vector.
     */
    static float magnitude(const sf::Vector2f &v);

    /**
     * @brief Normalizes a 2D vector to have a magnitude of 1.
     * @param v The vector to normalize.
     * @return The normalized vector. If the input vector has zero magnitude, returns a zero vector.
     */
    static sf::Vector2f normalize(const sf::Vector2f &v);

    /**
     * @brief Projects a point onto a line defined by a point and a direction.
     * @param point The point to project.
     * @param linePoint A point on the line.
     * @param direction The direction vector of the line.
     * @return The projected point on the line.
     */
    static sf::Vector2f projectPointOnLine(const sf::Vector2f& point,const sf::Vector2f& linePoint, const sf::Vector2f& direction);
    
    /**
     * @brief Offsets a point along a given direction by a specified radius.
     * @param point The original point to offset.
     * @param direction The direction vector along which to offset the point.
     * @param radius The distance to offset the point.
     * @return The new offset point.
     */
    static sf::Vector2f offsetPointAlongDirection(const sf::Vector2f& point, const sf::Vector2f& direction, float radius);
private:
    float m_projectionAngleX;
    float m_projectionAngleY;
    float m_tileSizeX;
    float m_tileSizeY;
    float m_heightScale;
    sf::Vector2f m_worldPivot;
};


#endif //LANDCRAFT_ISOMETRICPROJECTION_HPP