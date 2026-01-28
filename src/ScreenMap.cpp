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
        std::vector<ScreenTileCorner> row;
        for (int x = 0; x < worldMap[y].size(); x++) {
            int worldX = worldMap[y][x].Position.x * m_tileSizeX;
            int worldY = worldMap[y][x].Position.y * m_tileSizeY;
            sf::Vector2f screenPos = world_to_screen(worldX, worldY, worldMap[y][x].Height) + m_translationOffset;
            ScreenTileCorner corner = {screenPos, worldMap[y][x].Position, worldMap[y][x].Height * m_heightScale, worldMap[y][x].Color};
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

std::vector<sf::Vector2i> ScreenMap::getSelectedTilesCorner(sf::RenderWindow &window)
{
    sf::Vector2i mouseScreenPosition = sf::Mouse::getPosition(window);
    sf::Vector2f tempPos = GetMouseWorldPosition(mouseScreenPosition);
    sf::Vector2i mouseWorldPosition = {(int)std::round(tempPos.x), (int)std::round(tempPos.y)};

    ScreenTileCorner closestCorner = getClosestNeighborInRadius(mouseWorldPosition.x, mouseWorldPosition.y, 2, sf::Vector2f(mouseScreenPosition));
    if (closestCorner.WorldPosition.x == -1 && closestCorner.WorldPosition.y == -1)
        return  {};
    return { closestCorner.WorldPosition };
}

void ScreenMap::createVertexArrayMap()
{
    m_vertexArrayMap.setPrimitiveType(sf::Lines);
    m_vertexArrayMap.clear();
    for (int y = 0; y < m_map.size(); y++) {
        for (int x = 0; x < m_map[y].size(); x++) {
            std::vector<ScreenTileCorner> neighbors = getPointNeighbors(x, y);
            for (ScreenTileCorner neighbor : neighbors) {
                m_vertexArrayMap.append(sf::Vertex(m_map[y][x].ScreenPosition, m_map[y][x].Color));
                m_vertexArrayMap.append(sf::Vertex(neighbor.ScreenPosition, neighbor.Color));
            }
        }
    }
}

sf::Vector2f ScreenMap::GetMouseWorldPosition(sf::Vector2i mouseScreenPosition)
{
    sf::Vector2f worldPosition = screen_to_world(mouseScreenPosition.x - m_translationOffset.x, mouseScreenPosition.y - m_translationOffset.y, 0);
    float tileX = (worldPosition.x) / m_tileSizeX;
    float tileY = (worldPosition.y) / m_tileSizeY;
    return {tileX, tileY};
}

std::vector<ScreenTileCorner> ScreenMap::getPointNeighbors(int x, int y)
{
    std::vector<ScreenTileCorner> neighbors;
    if (x + 1 < m_map[y].size())
        neighbors.push_back(m_map[y][x + 1]);
    if (y + 1 < m_map.size())
        neighbors.push_back(m_map[y + 1][x]);
    return neighbors;
}

std::vector<ScreenTileCorner> ScreenMap::getPointNeighborsInRadius(int x, int y, int radius)
{
    std::vector<ScreenTileCorner> neighbors;
    int startX = std::max(0, x - radius);
    int endX = std::min((int)m_map[0].size(), x + radius);
    int startY = std::max(0, y - radius);
    int endY = std::min((int)m_map.size(), y + radius);

    for (int j = startY; j < endY; j++)
        for (int i = startX; i < endX; i++)
            neighbors.push_back(m_map[j][i]);
    return neighbors;
}

ScreenTileCorner ScreenMap::getClosestNeighborInRadius(int worldX, int worldY, int radius, sf::Vector2f referencePoint)
{
    std::vector<ScreenTileCorner> neighbors = getPointNeighborsInRadius(worldX, worldY, radius);
    if (neighbors.empty())
        return ScreenTileCorner{ sf::Vector2f(0,0), sf::Vector2i(-1,-1), 0, sf::Color::Black };
    ScreenTileCorner closestNeighbor = neighbors[0];
    float minDistance = distanceBetweenPoints(neighbors[0].ScreenPosition, referencePoint);

    for(const ScreenTileCorner& neighbor : neighbors)
    {
        float dist = distanceBetweenPoints(neighbor.ScreenPosition, referencePoint);
        if (dist < minDistance)
        {
            minDistance = dist;
            closestNeighbor = neighbor;
        }
    }
    return closestNeighbor;
}

float ScreenMap::distanceBetweenPoints(const sf::Vector2f &p1, const sf::Vector2f &p2)
{
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
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
