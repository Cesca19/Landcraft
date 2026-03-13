//
// Created by fran on 03/03/2026.
//

#ifndef LANDCRAFT_MATHUTILS_HPP
#define LANDCRAFT_MATHUTILS_HPP
#define _USE_MATH_DEFINES

#include <SFML/System/Vector2.hpp>
#include <cmath>

namespace MathUtils
{
    float radToDeg(const float rad);
    float degToRad(const float deg);
    float distanceBetweenPoints(const sf::Vector2f &p1, const sf::Vector2f &p2);
    float dotProduct(const sf::Vector2f &v1, const sf::Vector2f &v2);
    float magnitude(const sf::Vector2f &v);
    sf::Vector2f normalize(const sf::Vector2f &v);

    /**
     * @brief Projects a point onto a line defined by a point and a direction.
     * @param point The point to project.
     * @param linePoint A point on the line.
     * @param direction The direction vector of the line.
     * @return The projected point on the line.
     */
    sf::Vector2f projectPointOnLine(const sf::Vector2f& point,const sf::Vector2f& linePoint, const sf::Vector2f& direction);

    /**
     * @brief Offsets a point along a given direction by a specified radius.
     * @param point The original point to offset.
     * @param direction The direction vector along which to offset the point.
     * @param radius The distance to offset the point.
     * @return The new offset point.
     */
    sf::Vector2f offsetPointAlongDirection(const sf::Vector2f& point, const sf::Vector2f& direction, float radius);

    sf::Vector2f rotateAroundPointZAxis(const float angle, const sf::Vector2f point);

    bool isInsideTriangle(sf::Vector2f point, sf::Vector2f triangleCorner1, 
        sf::Vector2f triangleCorner2, sf::Vector2f triangleCorner3, const float epsilon = 0.01f);
    float triangleArea(sf::Vector2f point1, sf::Vector2f point2, sf::Vector2f point3);
};


#endif //LANDCRAFT_MATHUTILS_HPP