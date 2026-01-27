#include "WorldMap.hpp"

WorldMap::WorldMap(std::string filepath)
{
    // ideally load from filepath
    std::vector<std::vector<int>> input3dMap = {
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
        {0, 0, 0, 5, 3, 0, 0, 1, 1, 0},
        {0, 9, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 7, 5, 0, 0, 0, 0, 1, 0, 0},
        {0, 3, 6, 0, 0, 0, 0, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };


    for (int y = 0; y < input3dMap.size(); y++) {
        std::vector<TileCorner> row;
        for (int x = 0; x < input3dMap[y].size(); x++) {
            TileCorner tile = {sf::Vector2f(x, y), input3dMap[y][x], sf::Color::Cyan};
            row.push_back(tile);
        }
        m_map.push_back(row);
    }
}

WorldMap::~WorldMap()
{
}

const std::vector<std::vector<TileCorner>>& WorldMap::getMap()
{
    return m_map;
}

void WorldMap::updateTilesState(sf::Vector2f mouseWorldPos)
{
    int x = mouseWorldPos.x;
    int y = mouseWorldPos.y;

    for (int y = 0; y < m_map.size(); y++) {
        for (int x = 0; x < m_map[y].size(); x++) {
            m_map[y][x].Color = sf::Color::Cyan;
        }
    }
    m_map[y][x].Color = sf::Color::Magenta;
}

void WorldMap::onTileCornerHovered(float x, float y)
{
}
