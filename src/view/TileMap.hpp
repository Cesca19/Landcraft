//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_TILEMAP_HPP
#define LANDCRAFT_TILEMAP_HPP

#include <SFML/Graphics.hpp>
#include "../model/Tile.hpp"
#include "Camera.hpp"

class TileMap  : public sf::Drawable, public sf::Transformable
{
public:
    TileMap();
    void init(const std::vector<std::vector<Tile>>& tiles, const Camera& camera);
    void updatePositions(const std::vector<std::vector<Tile>>& tiles, const Camera& camera);
private:
    void addShadedTile(const Tile& tile, const Camera& camera);
    void addWireframeTile(const Tile& tile, const Camera& camera);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
    sf::VertexArray m_shadedTilesVertexArray;
    sf::VertexArray m_wireframeTilesVertexArray;

    sf::Color m_shadedTileColor;
    sf::Color m_wireframeTileColor;
    sf::Color m_selectedTileColor;
    // sf::Texture m_tileset;
};


#endif //LANDCRAFT_TILEMAP_HPP