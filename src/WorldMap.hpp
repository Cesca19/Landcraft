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
    std::vector<std::vector<TileCorner>> &getMap() const;
    void upateTileState(sf::Vector2f mouseWorldPos);
    void onTileCornerHovered(float x, float y);
private:
    std::vector<std::vector<TileCorner>> m_map;
    std::vector<TileCorner> m_selectedCorners;
};
#endif // WORLDMAP_HPP