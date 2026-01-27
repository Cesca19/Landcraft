#ifndef WORLDMAP_HPP
#define WORLDMAP_HPP

#include <vector>
#include <string>
#include "TileCorner.hpp"

class WorldMap
{
public:
    WorldMap(std::string filepath);
    ~WorldMap();
    const std::vector<std::vector<TileCorner>>& getMap();
    void updateTilesState(sf::Vector2f mouseWorldPos);
private:
    void onTileCornerHovered(float x, float y);

    std::vector<std::vector<TileCorner>> m_map;
    std::vector<TileCorner> m_selectedCorners;
};

#endif // WORLDMAP_HPP