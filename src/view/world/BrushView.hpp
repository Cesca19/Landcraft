//
// Created by fran on 13/03/2026.
//

#ifndef LANDCRAFT_BRUSHVIEW_HPP
#define LANDCRAFT_BRUSHVIEW_HPP

#include <SFML/Graphics.hpp>
#include "Camera.hpp"
#include "../../model/Tile.hpp"

class BrushView {
public:
    BrushView();
    ~BrushView();

    void drawTiles(sf::RenderWindow &window, const std::vector<Tile*> &tilesToHighlight, const Camera &camera);
    void drawTileCorners(sf::RenderWindow &window, const std::vector<TileCorner *> &cornersToHighlight, const Camera &camera);
private:
    float m_tileCornerRadius;
    sf::VertexArray m_highlightedTilesVertexArray;
    sf::CircleShape m_highlightedTileCorner;
    sf::Color m_highlightedTileCornerColor;
    sf::Color m_highlightedTileColor;
};


#endif //LANDCRAFT_BRUSHVIEW_HPP