//
// Created by fran on 11/03/2026.
//

#include "Tile.hpp"

Tile::Tile()
{
}

Tile::Tile(const std::vector<TileCorner*> &corners, const int textureId)
    : m_corners(corners)
    , m_textureId(textureId)
{
    // TODO: Add proper error handling with exceptions or assertions
    if (corners.size() != 4)
        std::cerr << "A tile must have exactly 4 corners." << std::endl;
    updateTriangleCorners();
}

void Tile::setCorners(const std::vector<TileCorner*> &corners)
{
    m_corners = corners;
    // TODO: Add proper error handling with exceptions or assertions
    if (corners.size() != 4)
        std::cerr << "A tile must have exactly 4 corners." << std::endl;
    updateTriangleCorners();
}

std::vector<TileCorner*> Tile::getCorners() const
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

int Tile::getTextureId() const
{
    return m_textureId;
}

void Tile::setTextureId(const int textureId) {
    for (TileCorner* corner : m_corners)
        corner->setTextureId(textureId);
    m_textureId = textureId;
}

void Tile::updateTriangleCorners()
{
    m_upRightTriangleCorners = {m_corners[0], m_corners[1], m_corners[2]};
    m_downLeftTriangleCorners = {m_corners[2], m_corners[3], m_corners[0]};
}
