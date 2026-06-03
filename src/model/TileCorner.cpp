//
// Created by fran on 11/03/2026.
//

#include "TileCorner.hpp"

TileCorner::TileCorner(const int row, const int col, const float height, const float minElevation, const float maxElevation)
    : m_row(row)
    , m_col(col)
    , m_height(height)
    , m_minElevation(minElevation)
    , m_maxElevation(maxElevation)
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

sf::Vector2f TileCorner::getPosition() const
{
    return sf::Vector2f(getColumn(), getRow());
}

void TileCorner::addHeight(const float heightStep)
{
    m_height += heightStep;
    m_height = std::clamp(m_height, m_minElevation, m_maxElevation);
}

void TileCorner::setHeight(float height)
{
    m_height = std::clamp(height, m_minElevation, m_maxElevation);
}
