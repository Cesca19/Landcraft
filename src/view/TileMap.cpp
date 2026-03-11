//
// Created by fran on 11/03/2026.
//

#include "TileMap.hpp"

void TileMap::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    // states.texture = &m_tileset;

    // draw the vertex array
    target.draw(m_vertices, states);
}
