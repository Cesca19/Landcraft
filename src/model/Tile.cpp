//
// Created by fran on 11/03/2026.
//

#include <stdexcept>
#include "Tile.hpp"

Tile::Tile()
{
    m_corners.clear();
}

Tile::Tile(const std::vector<TileCorner*> &corners)
    : m_corners(corners)
{
    if (corners.size() != 4)
        throw std::invalid_argument("A tile must have exactly 4 corners.");
    updateTriangleCorners();
}

void Tile::setCorners(const std::vector<TileCorner*> &corners)
{
    m_corners = corners;
    if (corners.size() != 4)
        throw std::invalid_argument("A tile must have exactly 4 corners.");
    updateTriangleCorners();
}

const std::vector<TileCorner*> &Tile::getCorners() const
{
    return m_corners;
}

std::vector<TileCorner*> Tile::getUpRightTriangleCorners() const
{
    return m_upRightTriangleCorners;
}

std::vector<TileCorner*> Tile::getDownLeftTriangleCorners() const
{
    return m_downLeftTriangleCorners;
}

sf::Vector2f Tile::getPosition() const
{
    if (m_corners.empty())
        return {-1, -1};
    return m_corners[0]->getPosition();
}

sf::Vector2i Tile::getGridPosition() const
{
    if (m_corners.empty())
        return {-1, -1};
    return sf::Vector2i(
        static_cast<int>(m_corners[0]->getColumn()),
        static_cast<int>(m_corners[0]->getRow())
    );
}

sf::Vector2f Tile::getCenterGridPosition() const
{
    return getPosition() + sf::Vector2f(0.5f, 0.5f);
}

void Tile::updateTriangleCorners()
{
    m_upRightTriangleCorners = {m_corners[0], m_corners[1], m_corners[2]};
    m_downLeftTriangleCorners = {m_corners[2], m_corners[3], m_corners[0]};
}
