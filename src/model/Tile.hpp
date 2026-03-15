//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_TILE_HPP
#define LANDCRAFT_TILE_HPP

#include "TileCorner.hpp"
#include <vector>
#include <iostream>

class Tile {
public:
    Tile();
    Tile(const std::vector<TileCorner*>& corners);
    void setCorners(const std::vector<TileCorner*>& corners);
    std::vector<TileCorner*> getCorners() const;
    std::vector<TileCorner*> getUpRightTriangleCorners() const;
    std::vector<TileCorner*> getDownLeftTriangleCorners() const;
    sf::Vector2f getPosition() const;
    int getTextureId() const;
    void setTextureId(int textureId) const;
private:
    void updateTriangleCorners();
    std::vector<TileCorner*> m_corners;
    std::vector<TileCorner*> m_upRightTriangleCorners;
    std::vector<TileCorner*> m_downLeftTriangleCorners;
};


#endif //LANDCRAFT_TILE_HPP