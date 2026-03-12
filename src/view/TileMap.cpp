//
// Created by fran on 11/03/2026.
//

#include "TileMap.hpp"

TileMap::TileMap()
    : m_shadedTilesVertexArray(sf::Triangles)
    , m_wireframeTilesVertexArray(sf::Lines)
    , m_shadedTileColor(sf::Color(150, 150, 150, 200))
    , m_wireframeTileColor(sf::Color::White)
    , m_selectedTileColor(sf::Color::Magenta)
{
}

void TileMap::init(const std::vector<std::vector<Tile>> &tiles, Camera &camera)
{
    if (tiles.empty() || tiles[0].empty())
    {
        return; // TO DO: Handle empty tile map case
    }
    int nbRows = tiles.size();
    int nbCols = tiles[0].size();
    m_shadedTilesVertexArray.clear();
    m_shadedTilesVertexArray.resize(nbRows * nbCols * 6); // 2 triangles per tile

    for (int row = 0; row < nbRows; ++row)
    {
        for (int col = 0; col < nbCols; ++col)
        {
            const Tile &tile = tiles[row][col];
            addShadedTile(tile, camera);
            addWireframeTile(tile, camera);
        }
    }
}

void TileMap::addShadedTile(const Tile &tile, Camera &camera)
{
    // -> shaded tiles

    // up right triangle
    std::vector<TileCorner *> upRightCorners = tile.getUpRightTriangleCorners();
    for (const TileCorner *corner : upRightCorners)
    {
        sf::Vector2f screenPos = camera.world_to_screen(corner->getRow(), corner->getColumn(), corner->getHeight());
        m_shadedTilesVertexArray.append(sf::Vertex(screenPos, m_shadedTileColor));
        // TO DO: Set vertex texture coordinates and texture based on corner->getTextureID()
    }

    // down left triangle
    std::vector<TileCorner *> downLeftCorners = tile.getDownLeftTriangleCorners();
    for (const TileCorner *corner : downLeftCorners)
    {
        sf::Vector2f screenPos = camera.world_to_screen(corner->getRow(), corner->getColumn(), corner->getHeight());
        m_shadedTilesVertexArray.append(sf::Vertex(screenPos, m_shadedTileColor));
        // TO DO: Set vertex texture coordinates and texture based on corner->getTextureID()
    }
}

void TileMap::addWireframeTile(const Tile &tile, Camera &camera)
{
    // -> wireframe tiles
    std::vector<TileCorner *> corners = tile.getCorners();
    for (size_t i = 0; i < corners.size(); ++i)
    {
        const TileCorner *corner1 = corners[i];
        const TileCorner *corner2 = corners[(i + 1) % corners.size()];
        sf::Vector2f screenPos1 = camera.world_to_screen(corner1->getRow(), corner1->getColumn(), corner1->getHeight());
        sf::Vector2f screenPos2 = camera.world_to_screen(corner2->getRow(), corner2->getColumn(), corner2->getHeight());
        m_wireframeTilesVertexArray.append(sf::Vertex(screenPos1, m_wireframeTileColor));
        m_wireframeTilesVertexArray.append(sf::Vertex(screenPos2, m_wireframeTileColor));
    }
}

void TileMap::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    // states.texture = &m_tileset;

    // draw the vertex array
    target.draw(m_shadedTilesVertexArray, states);
    target.draw(m_wireframeTilesVertexArray, states);
}
