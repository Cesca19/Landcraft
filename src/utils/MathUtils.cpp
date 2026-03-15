//
// Created by fran on 03/03/2026.
//

#include "MathUtils.hpp"

namespace MathUtils {
    float radToDeg(const float rad)
    {
        return rad * 180 / M_PI;
    }

    float degToRad(const float deg)
    {
        return deg * M_PI / 180;
    }

    float distanceBetweenPoints(const sf::Vector2f &p1, const sf::Vector2f &p2)
    {
        return static_cast<float>(std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2)));
    }


    float dotProduct(const sf::Vector2f &v1, const sf::Vector2f &v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }

    float magnitude(const sf::Vector2f &v)
    {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    sf::Vector2f normalize(const sf::Vector2f &v)
    {
        float mag = magnitude(v);
        if (mag == 0.f)
            return {0.f, 0.f};
        return v / mag;
    }

    sf::Vector2f projectPointOnLine(const sf::Vector2f &point, const sf::Vector2f &linePoint, const sf::Vector2f &direction)
    {
        const sf::Vector2f AP = point - linePoint;
        const float dot = dotProduct(AP, direction);
        const float magSq = dotProduct(direction, direction);

        if (magSq == 0.f)
            return linePoint;

        const sf::Vector2f projection = (dot / magSq) * direction;
        return linePoint + projection;
    }

    sf::Vector2f offsetPointAlongDirection(const sf::Vector2f &point, const sf::Vector2f &direction, const int radius)
    {
        const sf::Vector2f normalizedDirection = direction / magnitude(direction);
        return point + normalizedDirection * static_cast<float>(radius);

    }

    sf::Vector2f rotateAroundPointZAxis(const float angle, const sf::Vector2f point)
    {
        const float radAngle = degToRad(angle);
        sf::Vector2f rotatedPoint;

        // 2D rotation matrix applied
        rotatedPoint.x = point.x * std::cos(radAngle) - point.y * std::sin(radAngle);
        rotatedPoint.y = point.x * std::sin(radAngle) + point.y * std::cos(radAngle);
        return rotatedPoint;
    }

    
    bool isInsideTriangle(sf::Vector2f point, sf::Vector2f triangleCorner1, sf::Vector2f triangleCorner2,
        sf::Vector2f triangleCorner3, const float epsilon)
    {
        /* Calculate area of triangle ABC */
        float A = triangleArea (triangleCorner1, triangleCorner2, triangleCorner3);

        /* Calculate area of triangle PBC */
        float A1 = triangleArea (point, triangleCorner2, triangleCorner3);

        /* Calculate area of triangle PAC */
        float A2 = triangleArea (triangleCorner1, point, triangleCorner3);

        /* Calculate area of triangle PAB */
        float A3 = triangleArea (triangleCorner1, triangleCorner2, point);

        /* Check if sum of A1, A2 and A3 is same as A */
        return std::abs(A - (A1 + A2 + A3)) < epsilon;
    }

    float triangleArea(sf::Vector2f point1, sf::Vector2f point2, sf::Vector2f point3)
    {
        return std::abs((point1.x*(point2.y-point3.y)
                        + point2.x*(point3.y-point1.y)
                        + point3.x*(point1.y-point2.y))
                        /2.0);
    }
}