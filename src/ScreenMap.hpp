#ifndef SCREEN_MAP_HPP
#define SCREEN_MAP_HPP

#include "TileCorner.hpp"

class ScreenMap
{
public:
    ScreenMap();
    ~ScreenMap();
private:
    int m_tileSizeX;
    int m_tileSizeY;
    int m_heightScale;
    sf::Vector2f m_translationOffset;
};

#endif // SCREEN_MAP_HPP