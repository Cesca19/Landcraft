//
// Created by fran on 11/03/2026.
//

#include "TileMap.hpp"

TileMap::TileMap()
    : m_shadedTilesVertexArray(sf::Triangles)
    , m_wireframeTilesVertexArray(sf::Lines)
    , m_shadedTileColor(sf::Color(150, 150, 150, 40))
    , m_wireframeTileColor(sf::Color::White)
{
}

void TileMap::init(const std::vector<std::vector<Tile>> &tiles, const Camera &camera)
{
    if (tiles.empty() || tiles[0].empty())
        return; // TO DO: Handle empty tile map case

    m_shadedTilesVertexArray.clear();
    m_wireframeTilesVertexArray.clear();
    for (int row = 0; row < tiles.size(); ++row)
        for (int col = 0; col < tiles[0].size(); ++col)
        {
            const Tile &tile = tiles[row][col];
            addShadedTile(tile, camera);
            addWireframeTile(tile, camera);
        }
}

void TileMap::updatePositions(const std::vector<std::vector<Tile>> &worldTiles, const Camera &camera)
{
    if (worldTiles.empty() || worldTiles[0].empty())
        return; // TO DO: Handle empty tile map case

    int shadedIndex = 0;
    int wireframeIndex = 0;

    for (int row = 0; row < worldTiles.size(); ++row)
    {
        for (int col = 0; col < worldTiles[0].size(); ++col)
        {
            const Tile& tile = worldTiles[row][col];

            // shaded triangles update
            for (const TileCorner* corner : tile.getUpRightTriangleCorners()) {
                m_shadedTilesVertexArray[shadedIndex++].position =
                    camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
            }
            for (const TileCorner* corner : tile.getDownLeftTriangleCorners()) {
                m_shadedTilesVertexArray[shadedIndex++].position =
                    camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
            }

            // wireframe update
            std::vector<TileCorner*> corners = tile.getCorners();
            for (size_t i = 0; i < corners.size(); ++i) {
                const TileCorner* corner1 = corners[i];
                const TileCorner* corner2 = corners[(i + 1) % corners.size()];

                m_wireframeTilesVertexArray[wireframeIndex++].position =
                    camera.world_to_screen(corner1->getColumn(), corner1->getRow(), corner1->getHeight());
                m_wireframeTilesVertexArray[wireframeIndex++].position =
                    camera.world_to_screen(corner2->getColumn(), corner2->getRow(), corner2->getHeight());
            }
        }
    }
}

void TileMap::updatePositions(const std::vector<std::vector<Tile>>& worldTiles, const std::vector<TileCorner *> &modifiedCorners, const Camera &camera)
{
    if (worldTiles.empty() || worldTiles[0].empty() || modifiedCorners.empty()) return;
    const int nbRows = static_cast<int>(worldTiles.size());
    const int nbCols = static_cast<int>(worldTiles[0].size());
    // Get the coordinates (Row, Col) of all modified tiles
    std::set<std::pair<int, int>> tilesToUpdate;

    for (const TileCorner* corner : modifiedCorners) {
        int r = static_cast<int>(corner->getRow());
        int c = static_cast<int>(corner->getColumn());
        // A corner (r, c) can belong with up to 4 tiles :
        if (r < nbRows && c < nbCols) tilesToUpdate.insert({r, c});
        if (r < nbRows && c > 0)      tilesToUpdate.insert({r, c - 1});
        if (r > 0 && c < nbCols)      tilesToUpdate.insert({r - 1, c});
        if (r > 0 && c > 0)           tilesToUpdate.insert({r - 1, c - 1});
    }
    // only update those tiles positions
    updateTiles(worldTiles, tilesToUpdate, camera);
}

void TileMap::addShadedTile(const Tile &tile, const Camera &camera)
{
    // -> shaded tiles
    // up right triangle
    const std::vector<TileCorner *> upRightCorners = tile.getUpRightTriangleCorners();
    for (const TileCorner *corner : upRightCorners)
    {
        sf::Vector2f screenPos = camera.world_to_screen(corner->getRow(), corner->getColumn(), corner->getHeight());
        m_shadedTilesVertexArray.append(sf::Vertex(screenPos, m_shadedTileColor));
        // TO DO: Set vertex texture coordinates and texture based on corner->getTextureID()
    }

    // down left triangle
    const std::vector<TileCorner *> downLeftCorners = tile.getDownLeftTriangleCorners();
    for (const TileCorner *corner : downLeftCorners)
    {
        sf::Vector2f screenPos = camera.world_to_screen(corner->getRow(), corner->getColumn(), corner->getHeight());
        m_shadedTilesVertexArray.append(sf::Vertex(screenPos, m_shadedTileColor));
        // TO DO: Set vertex texture coordinates and texture based on corner->getTextureID()
    }
}

void TileMap::addWireframeTile(const Tile &tile, const Camera &camera)
{
    // -> wireframe tiles
    const std::vector<TileCorner *> corners = tile.getCorners();
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

void TileMap::updateTiles(const std::vector<std::vector<Tile>>& worldTiles, const std::set<std::pair<int, int>>& tilesToUpdate, const Camera &camera)
{
    const int nbCols = static_cast<int>(worldTiles[0].size());
    for (const auto&[row, col] : tilesToUpdate) {
        const Tile& tile = worldTiles[row][col];
        // tile index the worldModel tilemap
        const int tileIndex = (row * nbCols) + col;
        // tile index in the shaded tilemap : made of triangles each tile has 6 vertices
        const int shadedIndex = tileIndex * 6;
        // tile index in the wireframe tilemap : made of lines each tile has 8 vertices
        const int wireframeIndex = tileIndex * 8;

        // --- UPDATE TRIANGLES ---
        updateShadedTile(tile, camera, shadedIndex);
        // --- UPDATE WIREFRAME ---
        updateWireframeTile(tile, camera, wireframeIndex);
    }
}

void TileMap::updateShadedTile(const Tile &tile, const Camera &camera, int shadedIndex)
{
    for (const TileCorner* corner : tile.getUpRightTriangleCorners())
        m_shadedTilesVertexArray[shadedIndex++].position =
            camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
    for (const TileCorner* corner : tile.getDownLeftTriangleCorners())
        m_shadedTilesVertexArray[shadedIndex++].position =
            camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
}

void TileMap::updateWireframeTile(const Tile &tile, const Camera &camera, int wireframeIndex)
{
    const std::vector<TileCorner*> corners = tile.getCorners();
    for (size_t i = 0; i < corners.size(); i++) {
        const TileCorner* corner1 = corners[i];
        const TileCorner* corner2 = corners[(i + 1) % corners.size()];

        m_wireframeTilesVertexArray[wireframeIndex++].position =
            camera.world_to_screen(corner1->getColumn(), corner1->getRow(), corner1->getHeight());
        m_wireframeTilesVertexArray[wireframeIndex++].position =
            camera.world_to_screen(corner2->getColumn(), corner2->getRow(), corner2->getHeight());
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
