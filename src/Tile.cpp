//
// Created by fran on 01/02/2026.
//

#include "Tile.hpp"

Tile::Tile()
{
}

Tile::Tile(std::vector<ScreenTileCorner *> corners) :
    m_corners(corners)
{
}

Tile::~Tile()
{
}

void Tile::addCorners(std::vector<ScreenTileCorner *> corners)
{
    m_corners.clear();
    m_corners = corners;
}

bool Tile::containsPoint(sf::Vector2f point)
{
    if (m_corners.size() != 4)
        return false;
  return isInsideTriangle(point, m_corners[0]->ScreenPosition, m_corners[1]->ScreenPosition, m_corners[2]->ScreenPosition)
    || isInsideTriangle(point, m_corners[2]->ScreenPosition, m_corners[3]->ScreenPosition, m_corners[0]->ScreenPosition);
}

std::vector<ScreenTileCorner *> Tile::getCorners()
{
    return m_corners;
}

float Tile::triangleArea(sf::Vector2f point1, sf::Vector2f point2, sf::Vector2f point3)
{
    return std::abs((point1.x*(point2.y-point3.y)
                    + point2.x*(point3.y-point1.y)
                    + point3.x*(point1.y-point2.y))
                    /2.0);
}

bool Tile::isInsideTriangle(sf::Vector2f point, sf::Vector2f triangleCorner1, sf::Vector2f triangleCorner2,
    sf::Vector2f triangleCorner3)
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
     return std::abs(A - (A1 + A2 + A3)) < m_epsilon;
}