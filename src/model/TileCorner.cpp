//
// Created by fran on 11/03/2026.
//

#include "TileCorner.hpp"

TileCorner::TileCorner(const int row, const int col, const int height, const int textureID)
    : m_row(row), m_col(col), m_height(height), m_textureID(textureID)
{
}

float TileCorner::getRow() const
{
    return static_cast<float>(m_row);
}

float TileCorner::getColumn() const
{
    return static_cast<float>(m_col);
}

float TileCorner::getHeight() const
{
    return static_cast<float>(m_height);
}

int TileCorner::getTextureID() const
{
    return m_textureID;
}

sf::Vector2f TileCorner::getPosition() const
{
    return sf::Vector2f(getColumn(), getRow());
}

void TileCorner::addHeight(const int heightStep)
{
    m_height += heightStep;
}

void TileCorner::setTextureId(const int textureId)
{
    m_textureID = textureId;
}
