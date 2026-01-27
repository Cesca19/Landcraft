#include "ScreenMap.hpp"

ScreenMap::ScreenMap(int tileSizeX, int tileSizeY, int heightScale, sf::Vector2f translationOffset, int projectionAngleX, int projectionAngleY)
    : m_tileSizeX(tileSizeX)
    , m_tileSizeY(tileSizeY)
    , m_heightScale(heightScale)
    , m_translationOffset(translationOffset)
    , m_projectionAngleX(projectionAngleX)
    , m_projectionAngleY(projectionAngleY)
    , m_vertexArrayMap(sf::Lines)
{
}

ScreenMap::~ScreenMap()
{
}

void ScreenMap::updateScreenMap(const std::vector<std::vector<TileCorner>> & worldMap)
{
    m_map.clear();
    for (int y = 0; y < worldMap.size(); y++) {
        std::vector<TileCorner> row;
        for (int x = 0; x < worldMap[y].size(); x++) {
            int worldX = worldMap[y][x].Position.x * m_tileSizeX;
            int worldY = worldMap[y][x].Position.y * m_tileSizeY;
            sf::Vector2f screenPos = world_to_screen(worldX, worldY, worldMap[y][x].Height) + m_translationOffset;
            TileCorner corner = {screenPos, worldMap[y][x].Height * m_heightScale, worldMap[y][x].Color};
            row.push_back(corner);
        }
        m_map.push_back(row);
    }
}

void ScreenMap::draw(sf::RenderWindow &window)
{
    createVertexArrayMap();
    window.draw(m_vertexArrayMap);
}

void ScreenMap::createVertexArrayMap()
{
    m_vertexArrayMap.setPrimitiveType(sf::Lines);
    m_vertexArrayMap.clear();
    for (int y = 0; y < m_map.size(); y++) {
        for (int x = 0; x < m_map[y].size(); x++) {
            std::vector<TileCorner> neighbors = getPointNeighbors(x, y);
            for (TileCorner neighbor : neighbors) {
                m_vertexArrayMap.append(sf::Vertex(m_map[y][x].Position, m_map[y][x].Color));
                m_vertexArrayMap.append(sf::Vertex(neighbor.Position, neighbor.Color));
            }
        }
    }
}

const std::vector<TileCorner> &ScreenMap::getPointNeighbors(int x, int y)
{
    std::vector<TileCorner> neighbors;
    if (x + 1 < m_map[y].size())
        neighbors.push_back(m_map[y][x + 1]);
    if (y + 1 < m_map.size())
        neighbors.push_back(m_map[y + 1][x]);
    return neighbors;
}

float ScreenMap::radToDeg(float rad)
{
    return rad * 180 / M_PI;
}

float ScreenMap::degToRad(float deg)
{
    return deg * M_PI / 180;
}

sf::Vector2f ScreenMap::world_to_screen(int point3dX, int point3dY, int point3dZ)
{
    float angleX = degToRad(m_projectionAngleX);
    float angleY = degToRad(m_projectionAngleY);
    sf::Vector2f point2d;

    point2d.x = std::cos(angleX) * point3dX - std::cos(angleX) * point3dY ;
    point2d.y = std::sin(angleY) * point3dY + std::sin(angleY) * point3dX - point3dZ * m_heightScale;
    return point2d;
}

sf::Vector2f ScreenMap::screen_to_world(int point2dX, int point2dY, int point2dZ)
{
        float angleX = degToRad(m_projectionAngleX);
    float angleY = degToRad(m_projectionAngleY);
    sf::Vector2f point3d;

    point3d.x = 0.5f * ((point2dX / std::cos(angleX)) + (point2dY + point2dZ * m_heightScale) / std::sin(angleY));
    point3d.y = 0.5f * (-(point2dX / std::cos(angleX)) + (point2dY + point2dZ * m_heightScale) / std::sin(angleY));
    return point3d;
}
