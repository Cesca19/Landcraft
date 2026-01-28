#ifndef WORLDMAP_HPP
#define WORLDMAP_HPP

#include <cmath>
#include <vector>
#include <string>
#include "TileCorner.hpp"

class WorldMap
{
public:
    WorldMap(std::string filepath);
    ~WorldMap();
    const std::vector<std::vector<TileCorner>>& getMap();
    void updateTilesState(sf::Vector2f mouseWorldPos, SelectionMode mode = SelectionMode::TILE_CORNER);
    void setSelectedTilesCornersHeight(int heightOffset);
private:
    void resetTilesColors();
    void setSelectedTileCornersColors(sf::Color color);
    void onTileCornerHovered(float x, float y);

    std::vector<std::vector<TileCorner>> m_map;
    std::vector<TileCorner> m_selectedCorners;
};

#endif // WORLDMAP_HPP