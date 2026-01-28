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

WorldMap::~WorldMap()
{
}

const std::vector<std::vector<TileCorner>> &WorldMap::getMap()
{
    return m_map;
}

void WorldMap::onTileCornerHovered(float x, float y)
{
    if (x < 0 || y < 0 ||
        y >= m_map.size() ||
        x >= m_map[0].size())
        return;
    m_selectedCorners.push_back(m_map[y][x]);
    updateTiles();
}

void WorldMap::updateTiles()
{
    resetTilesColors();
    setSelectedTileCornersColors(m_selectedTilesColor);
}

void WorldMap::removeSelectedTilesCorners()
{
    m_selectedCorners.clear();
}

void WorldMap::setSelectedTilesCornersHeight(int heightOffset)
{
    for (TileCorner &corner : m_selectedCorners)
        m_map[corner.Position.y][corner.Position.x].Height += heightOffset;
}

void WorldMap::resetTilesColors()
{
    for (int y = 0; y < m_map.size(); y++)
        for (int x = 0; x < m_map[y].size(); x++)
            m_map[y][x].Color = sf::Color::Cyan;
}

void WorldMap::setSelectedTileCornersColors(sf::Color color)
{
    for (TileCorner &corner : m_selectedCorners)
        m_map[corner.Position.y][corner.Position.x].Color = color;
}


