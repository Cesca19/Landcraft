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
    void setSelectedTilesCornersHeight(int heightOffset);
    void onTileCornerHovered(float x, float y);
    void updateTiles();
    void removeSelectedTilesCorners();
private:
    void resetTilesColors();
    void setSelectedTileCornersColors(sf::Color color);
    sf::Color m_selectedTilesColor = sf::Color::Magenta;

    std::vector<std::vector<TileCorner>> m_map;
    std::vector<TileCorner> m_selectedCorners;
};

#endif // WORLDMAP_HPP