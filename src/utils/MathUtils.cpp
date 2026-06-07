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

    std::string toString(float value)
    {
        std::stringstream s;
        s << value;
        std::string result=s.str();
      return result;
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

    std::vector<sf::Vector2i> getBresenhamLine(const sf::Vector2i start, const sf::Vector2i end)
    {
        std::vector<sf::Vector2i> linePoints;
        int dx = std::abs(end.x - start.x);
        int dy = std::abs(end.y - start.y);

        int mainAxisStep = (dx > dy) ? dx : dy;
        int secondaryAxisStep = (dx > dy) ? dy : dx;

        int stepX = (end.x > start.x) ? 1 : -1;
        int stepY = (end.y > start.y) ? 1 : -1;

        int slopeError = 0;
        int x = start.x;
        int y = start.y;

        linePoints.emplace_back(x, y);
        while (x != end.x || y != end.y) {
            if (dx > dy)
                x += stepX;
            else
                y += stepY;
            // mathematically we should have slopeError += (secondaryAxisStep / mainAxisStep)
            // but to avoid floating point precision issues by multiplying everything by mainAxisStep * 2
            // slopeError += (secondaryAxisStep / mainAxisStep) * 2 * mainAxisStep;
            // we finally end up with
            slopeError += secondaryAxisStep * 2;
            // we do the same thing for the comparison with 0.5,
            // we multiply by mainAxisStep * 2 to avoid floating point precision issues
            // so instead of comparing slopeError >= 0.5 we compare
            // slopeError >= (0.5 * mainAxisStep * 2) an finally we end up with
            if (slopeError >= mainAxisStep) {
                if (dx > dy)
                    y += stepY;
                else
                    x += stepX;
                // and we should reset the slope error by subtracting 1 but since the other
                // operations are multiplied by mainAxisStep * 2 we need to do the same
                // for the reset, so we subtract mainAxisStep * 2 instead of just 1
                slopeError -= mainAxisStep * 2;
            }
            linePoints.emplace_back(x, y);
        }
        return linePoints;
    }
}