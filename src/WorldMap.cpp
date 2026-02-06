#include "WorldMap.hpp"

WorldMap::WorldMap()
{

}

WorldMap::~WorldMap()
{
}

void WorldMap::init(const std::string &filePath)
{
    m_map.clear();
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
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

    for (int y = 0; y < input3dMap.size(); y++) {
        std::vector<TileCorner> row;
        for (int x = 0; x < input3dMap[y].size(); x++) {
            TileCorner tile = {sf::Vector2i(x, y), input3dMap[y][x], sf::Color::Cyan};
            row.push_back(tile);
        }
        m_map.push_back(row);
    }
}

const std::vector<std::vector<TileCorner>> &WorldMap::getMap()
{
    return m_map;
}

void WorldMap::setCornerHeight(const int heightOffset, const sf::Vector2i &corner)
{
    m_map[corner.y][corner.x].Height += heightOffset;
}

void WorldMap::setTilesCornersHeight(const int heightOffset, const std::vector<sf::Vector2i> &corners)
{
    for (const sf::Vector2i &cornerPos : corners)
        m_map[cornerPos.y][cornerPos.x].Height += heightOffset;
}