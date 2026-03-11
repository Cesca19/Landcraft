//
// Created by fran on 01/02/2026.
//

#ifndef LANDCRAFT_TILE_H
#define LANDCRAFT_TILE_H

#include <vector>
#include "TileCorner.hpp"

class Tile
{
public:
    Tile();
    Tile(std::vector<ScreenTileCorner *> corners);
    ~Tile();
    void addCorners(std::vector<ScreenTileCorner *> corners);
    bool containsPoint(sf::Vector2f point);
    std::vector<ScreenTileCorner *> getCorners();
private:
    float triangleArea(sf::Vector2f point1, sf::Vector2f point2, sf::Vector2f point3);
    bool isInsideTriangle(sf::Vector2f point, sf::Vector2f triangleCorner1, sf::Vector2f triangleCorner2, sf::Vector2f triangleCorner3);
    float m_epsilon = 0.01f;
    std::vector<ScreenTileCorner *> m_corners;
};


#endif //LANDCRAFT_TILE_H