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
    IsometricProjection(int tileSizeX, int tileSizeY, int heightScale, int projectionAngleX, int projectionAngleY);
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
    sf::Vector2f screen_to_world(int point2dX, int point2dY, int point2dZ) const;

    sf::Vector2f getPointScreenPosition(sf::Vector2f worldPosition, int worldHeight) const;

    /**
     * @brief Rotates a 2D point around the origin (0,0).
     * To rotate around a specific center, subtract the center before calling this,
     * then add it back to the result.
     *
     * @param angle The rotation angle in RADIANS.
     * @param point The point to rotate.
     * @return The new rotated coordinates.
     */
    static sf::Vector2f rotateAroundZAxis(float angle, sf::Vector2f point);
private:
    int m_projectionAngleX;
    int m_projectionAngleY;
    int m_tileSizeX;
    int m_tileSizeY;
    int m_heightScale;
};


#endif //LANDCRAFT_ISOMETRICPROJECTION_HPP