//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_TILEMAP_HPP
#define LANDCRAFT_TILEMAP_HPP

#include <set>
#include <SFML/Graphics.hpp>
#include "../model/Tile.hpp"
#include "Camera.hpp"

class TileMap  : public sf::Drawable, public sf::Transformable
{
public:
    TileMap();
    void init(const std::vector<std::vector<Tile>>& tiles, const Camera& camera);
    void updatePositions(const std::vector<std::vector<Tile>>& worldTiles, const Camera& camera);
    void updatePositions(const std::vector<std::vector<Tile>>& worldTiles, const std::vector<TileCorner *> &modifiedCorners, const Camera& camera);
private:
    void addShadedTile(const Tile& tile, const Camera& camera);
    void addWireframeTile(const Tile& tile, const Camera& camera);
    void updateTiles(const std::vector<std::vector<Tile>>& worldTiles, const std::set<std::pair<int, int>>& tilesToUpdate, const Camera& camera);
    void updateShadedTile(const Tile& tile, const Camera& camera, int shadedIndex);
    void updateWireframeTile(const Tile& tile, const Camera& camera, int wireframeIndex);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
    sf::VertexArray m_shadedTilesVertexArray;
    sf::VertexArray m_wireframeTilesVertexArray;

    sf::Color m_shadedTileColor;
    sf::Color m_wireframeTileColor;
    // sf::Texture m_tileset;
};


#endif //LANDCRAFT_TILEMAP_HPP