#ifndef WORLDMAP_HPP
#define WORLDMAP_HPP

#include <cmath>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

#include "TileCorner.hpp"

class WorldMap
{
public:
    WorldMap();
    ~WorldMap();
    void init(const std::string &filePath);
    const std::vector<std::vector<TileCorner>>& getMap();
    void setCornerHeight(float heightOffset, const sf::Vector2i &corner);
    void setTilesCornersHeight(float heightOffset, const std::vector<sf::Vector2i>& corners);
private:
    std::vector<std::vector<TileCorner>> m_map;
};

#endif // WORLDMAP_HPP