//
// Created by fran on 11/03/2026.
//

#include "TileCorner.hpp"

TileCorner::TileCorner(int row, int col, int height, int textureID) 
    : m_row(row), m_col(col), m_height(height), m_textureID(textureID)
{
}

int TileCorner::getRow() const
{
    return m_row;
}

int TileCorner::getColumn() const
{
    return m_col;
}

int TileCorner::getHeight() const
{
    return m_height;
}

int TileCorner::getTextureID() const
{
    return m_textureID;
}
