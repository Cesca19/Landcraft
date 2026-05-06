//
// Created by fran on 11/03/2026.
//

#include "TileMap.hpp"

TileMap::TileMap(const std::string &tilesetFilepath, const sf::Vector2u tilesSize)
    : m_tilesSize(tilesSize)
    , m_shadedTilesVertexArray(sf::Triangles)
    , m_wireframeTilesVertexArray(sf::Lines)
    , m_shadedTileColor(sf::Color(252, 252, 254/*205, 185, 220*/))
    // , m_shadedTileColor(sf::Color(200, 180, 220))
    , m_wireframeTileColor(sf::Color(110, 110, 120, 25))
{
    m_tilesetTexture = &ResourceManager::getInstance().getTexture(tilesetFilepath);
}

void TileMap::clear()
{
    m_shadedTilesVertexArray.clear();
    m_wireframeTilesVertexArray.clear();
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

void TileMap::paintTiles(const std::vector<std::vector<Tile>> &worldTiles, const std::vector<Tile *> &tilesToPaint,
    const int textureId)
{
    if (worldTiles.empty() || worldTiles[0].empty() || tilesToPaint.empty()) return;
    const int nbCols = static_cast<int>(worldTiles[0].size());
    for (const Tile *tile : tilesToPaint) {
        sf::Vector2f tilePosition = tile->getPosition();
        if (tilePosition == sf::Vector2f{-1, -1})
            continue;
        const int tileIndex = static_cast<int>(tilePosition.y * nbCols + tilePosition.x);
        paintTile(tileIndex * 6, textureId);
    }
}

void TileMap::paintTile(const std::vector<std::vector<Tile>> &worldTiles, Tile *tileToPaint, int textureId)
{
    if (worldTiles.empty() || worldTiles[0].empty() || tileToPaint == nullptr) return;
    const int nbCols = static_cast<int>(worldTiles[0].size());
    sf::Vector2f tilePosition = tileToPaint->getPosition();
    if (tilePosition == sf::Vector2f{-1, -1})
        return;
    const int tileIndex = static_cast<int>(tilePosition.y * nbCols + tilePosition.x);
    paintTile(tileIndex * 6, textureId);
}

void TileMap::addShadedTile(const Tile &tile, const Camera &camera)
{
    // -> shaded tiles
    // up right triangle
    const std::vector<TileCorner *> upRightCorners = tile.getUpRightTriangleCorners();
    for (const TileCorner *corner : upRightCorners)
    {
        sf::Vector2f screenPos = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
        m_shadedTilesVertexArray.append(sf::Vertex(screenPos, m_shadedTileColor));
    }

    // down left triangle
    const std::vector<TileCorner *> downLeftCorners = tile.getDownLeftTriangleCorners();
    for (const TileCorner *corner : downLeftCorners)
    {
        sf::Vector2f screenPos = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
        m_shadedTilesVertexArray.append(sf::Vertex(screenPos, m_shadedTileColor));
    }
    paintTile(m_shadedTilesVertexArray.getVertexCount() - 6, tile.getTextureId());
}

void TileMap::addWireframeTile(const Tile &tile, const Camera &camera)
{
    // -> wireframe tiles
    const std::vector<TileCorner *> corners = tile.getCorners();
    for (size_t i = 0; i < corners.size(); ++i)
    {
        const TileCorner *corner1 = corners[i];
        const TileCorner *corner2 = corners[(i + 1) % corners.size()];
        sf::Vector2f screenPos1 = camera.world_to_screen(corner1->getColumn(), corner1->getRow(), corner1->getHeight());
        sf::Vector2f screenPos2 = camera.world_to_screen(corner2->getColumn(), corner2->getRow(), corner2->getHeight());
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

void TileMap::paintTile(int shadedIndex, const int textureId)
{
    if (textureId <= 0) {
        for (int i = 0; i < 6; i++) {
            m_shadedTilesVertexArray[shadedIndex].texCoords = sf::Vector2f(0.f, 0.f);
            m_shadedTilesVertexArray[shadedIndex].color = m_shadedTileColor;
            shadedIndex++;
        }
        return;
    }
    const unsigned int x = textureId * m_tilesSize.x;
    const int y = 0;
    sf::Vector2f texCoords[6] = {
        sf::Vector2f(x, y),
        sf::Vector2f(x + m_tilesSize.x, y),
        sf::Vector2f(x + m_tilesSize.x, y + m_tilesSize.y),
        sf::Vector2f(x + m_tilesSize.x, y + m_tilesSize.y),
        sf::Vector2f(x, y + m_tilesSize.y),
        sf::Vector2f(x, y)
    };
    for (const auto texCoord : texCoords) {
        m_shadedTilesVertexArray[shadedIndex].texCoords = texCoord;
        m_shadedTilesVertexArray[shadedIndex].color = sf::Color::White;
        shadedIndex++;
    }
}

void TileMap::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = m_tilesetTexture;

    // draw the vertex array
    target.draw(m_shadedTilesVertexArray, states);

    states.texture = nullptr;
    target.draw(m_wireframeTilesVertexArray, states);
}
