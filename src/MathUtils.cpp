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

    sf::Vector2f offsetPointAlongDirection(const sf::Vector2f &point, const sf::Vector2f &direction, const float radius)
    {
        const sf::Vector2f normalizedDirection = direction / magnitude(direction);
        return point + normalizedDirection * radius;

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
}