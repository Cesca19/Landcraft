//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_TILEMAP_HPP
#define LANDCRAFT_TILEMAP_HPP

#include <SFML/Graphics.hpp>

class TileMap  : public sf::Drawable, public sf::Transformable
{
public:
    TileMap();
    void init(int nbCols, int nbRows);
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    sf::VertexArray m_vertices;
    // sf::Texture m_tileset;
};


#endif //LANDCRAFT_TILEMAP_HPP