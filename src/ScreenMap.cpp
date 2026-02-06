#include "ScreenMap.hpp"

ScreenMap::ScreenMap(int tileSizeX, int tileSizeY, int heightScale, sf::Vector2f translationOffset, int projectionAngleX, int projectionAngleY)
    : m_projectionAngleX(projectionAngleX)
    , m_projectionAngleY(projectionAngleY)
    , m_tileSizeX(tileSizeX)
    , m_tileSizeY(tileSizeY)
    , m_heightScale(heightScale)
    , m_rotationSpeed(10)
    , m_currentRotationAngle(0)
    , m_targetRotationAngle(0)
    , m_vertexArrayMap(sf::Lines)
    , m_worldMap(std::make_shared<WorldMap>())
{
}

ScreenMap::~ScreenMap()
{
}

void ScreenMap::update(float deltaTime, const sf::RenderWindow &window, const SelectionMode selectionMode)
{
    getSelectedCorners(window, selectionMode);
    resetTilesCornerColors();
    setSelectedTileCornersColors();

    if (std::abs(m_targetRotationAngle - m_currentRotationAngle) > m_epsilon) {
        m_currentRotationAngle = m_currentRotationAngle + (m_targetRotationAngle - m_currentRotationAngle) * m_rotationSpeed * deltaTime;
        rotateMapAroundZAxis(m_currentRotationAngle);
    } else
        if (m_currentRotationAngle != m_targetRotationAngle) {
            m_currentRotationAngle = m_targetRotationAngle;
            rotateMapAroundZAxis(m_currentRotationAngle);
        }

}

void ScreenMap::draw(sf::RenderWindow &window)
{
    createVertexArrayMap();
    window.draw(m_vertexArrayMap);
}

void ScreenMap::init(const std::string &mapFilepath)
{
    m_worldMap->init(mapFilepath);
    initTilesCornersMap();
    initTilesMap();
}

void ScreenMap::setSelectedCornersHeight(int heightOffset)
{
    for (ScreenTileCorner *corner : m_selectedCorners) {
        m_worldMap->setCornerHeight(heightOffset, corner->WorldPosition);
        corner->WorldHeight += heightOffset;
        corner->ScreenHeight += (heightOffset * m_heightScale);
        corner->ScreenPosition = getPointScreenPosition(corner->RotatedWorldPosition, corner->WorldHeight);
    }
}

sf::Vector2f ScreenMap::getScreenMapCenter() const
{
    int x = m_map[0].size() / 2;
    int y = m_map.size() / 2;
    x = std::clamp(x - 1, 0, static_cast<int>(m_map[0].size() - 1));
    y = std::clamp(y - 1, 0, static_cast<int>(m_map.size() - 1));
    return m_map[y][x]->ScreenPosition;
}

void ScreenMap::rotateAroundZAxis(const float angle)
{
    m_targetRotationAngle += angle;
}

void ScreenMap::rotateMapAroundZAxis(const float angle)
{
    for (int y = 0; y < m_map.size(); y++)
        for (int x = 0; x < m_map[y].size(); x++)
            rotateCornerAroundZAxis(angle, m_map[y][x].get());
}

void ScreenMap::rotateCornerAroundZAxis(const float angle, ScreenTileCorner *corner)
{
    const sf::Vector2f worldCenter = {static_cast<float>(m_map[0].size() -1.0f) / 2, static_cast<float>(m_map.size() - 1.0f) / 2 };
    // translate the point to rotate around the maps center
    // then cancel this translation to avoid the gap
    corner->RotatedWorldPosition = rotateAroundZAxis(angle, sf::Vector2f(corner->WorldPosition) - worldCenter) + worldCenter;
    corner->ScreenPosition = getPointScreenPosition(corner->RotatedWorldPosition, corner->WorldHeight);
}

void ScreenMap::initTilesCornersMap()
{
    const std::vector<std::vector<TileCorner>> worldMap = m_worldMap->getMap();
    m_map.clear();
    for (int y = 0; y < worldMap.size(); y++) {
        std::vector<std::unique_ptr<ScreenTileCorner>> row;
        for (int x = 0; x < worldMap[y].size(); x++) {
           sf::Vector2f screenPos = getPointScreenPosition(sf::Vector2f(worldMap[y][x].Position), worldMap[y][x].Height);
            std::unique_ptr<ScreenTileCorner> corner = std::make_unique<ScreenTileCorner>(screenPos, worldMap[y][x].Position,
                                                    worldMap[y][x].Height * m_heightScale,
                                                    worldMap[y][x].Height, worldMap[y][x].Color);
            row.push_back(std::move(corner));
        }
        m_map.push_back(std::move(row));
    }
}

void ScreenMap::initTilesMap()
{
    m_tilesMap.clear();
    for (int y = 0; y < m_map.size(); y++)
        for (int x = 0; x < m_map[y].size(); x++)
            createTileFromTileCorner(x, y);
}

void ScreenMap::createTileFromTileCorner(const int tileCornerX, const int tileCornerY)
{
    if (m_map.size() <= 1 || m_map[0].size() <= 1
        || tileCornerX < 0 || tileCornerX + 1 >= m_map[0].size()
        || tileCornerY < 0 || tileCornerY + 1 >= m_map.size())
        return;
    std::vector<ScreenTileCorner *> tileCorners = {
        m_map[tileCornerY][tileCornerX].get(),
        m_map[tileCornerY][tileCornerX + 1].get(),
        m_map[tileCornerY + 1][tileCornerX + 1].get(),
        m_map[tileCornerY + 1][tileCornerX].get(),
    };
    if (m_tilesMap.size() < tileCornerY + 1)
        m_tilesMap.emplace_back();
    m_tilesMap[tileCornerY].emplace_back(std::make_unique<Tile>(tileCorners));
}

void ScreenMap::createVertexArrayMap()
{
    m_vertexArrayMap.setPrimitiveType(sf::Lines);
    m_vertexArrayMap.clear();
    for (int y = 0; y < m_map.size(); y++) {
        for (int x = 0; x < m_map[y].size(); x++) {
            std::vector<ScreenTileCorner*> neighbors = getPointNeighbors(x, y);
            for (ScreenTileCorner* neighbor : neighbors) {
                m_vertexArrayMap.append(sf::Vertex(m_map[y][x]->ScreenPosition, m_map[y][x]->Color));
                m_vertexArrayMap.append(sf::Vertex(neighbor->ScreenPosition, neighbor->Color));
            }
        }
    }
}

sf::Vector2f ScreenMap::getMouseWorldPosition(sf::Vector2f mouseScreenPosition)
{
    sf::Vector2f worldPosition = screen_to_world(mouseScreenPosition.x /*- m_translationOffset.x*/, mouseScreenPosition.y /*- m_translationOffset.y*/, 0);
    float tileX = (worldPosition.x) / m_tileSizeX;
    float tileY = (worldPosition.y) / m_tileSizeY;
    return {tileX, tileY};
}

void ScreenMap::resetTilesCornerColors() const
{
    for (auto & y : m_map)
        for (const auto & x : y)
            x->Color = m_defaultTilesColor;
}

void ScreenMap::setSelectedTileCornersColors() const
{
    for (ScreenTileCorner* corner : m_selectedCorners)
        m_map[corner->WorldPosition.y][corner->WorldPosition.x]->Color = m_selectedTilesColor;
}

std::vector<ScreenTileCorner*> ScreenMap::getPointNeighbors(int x, int y) const
{
    std::vector<ScreenTileCorner*> neighbors;
    if (x + 1 < m_map[y].size())
        neighbors.push_back(m_map[y][x + 1].get());
    if (y + 1 < m_map.size())
        neighbors.push_back(m_map[y + 1][x].get());
    return neighbors;
}

std::vector<ScreenTileCorner*> ScreenMap::getPointNeighborsInRadius(int x, int y, int radius) const {
    std::vector<ScreenTileCorner*> neighbors;
    // prevent overflow when mouse is outside the screen
    x = std::clamp(x, 0, static_cast<int>(m_map[0].size()));
    y = std::clamp(y, 0, static_cast<int>(m_map.size()));

    int startX = std::max(0, x - radius);
    int endX = std::min((int)m_map[0].size(), x + radius);
    int startY = std::max(0, y - radius);
    int endY = std::min((int)m_map.size(), y + radius);

    for (int j = startY; j < endY; j++)
        for (int i = startX; i < endX; i++)
            neighbors.push_back(m_map[j][i].get());
    return neighbors;
}

ScreenTileCorner* ScreenMap::getClosestNeighborCornerInRadius(const sf::Vector2i pointWorldPosition, const sf::Vector2f pointScreenPosition, const int radius)
{
    const std::vector<ScreenTileCorner*> neighbors = getPointNeighborsInRadius(pointWorldPosition.x, pointWorldPosition.y, radius);
    if (neighbors.empty())
        return nullptr;
    ScreenTileCorner* closestNeighbor = neighbors[0];
    float minDistance = distanceBetweenPoints(neighbors[0]->ScreenPosition, pointScreenPosition);
    float refMinDistance = std::max(m_tileSizeX, m_tileSizeY);

    for ( ScreenTileCorner* neighbor : neighbors) {
        float dist = distanceBetweenPoints(neighbor->ScreenPosition, pointScreenPosition);
        if (dist < minDistance)
        {
            minDistance = dist;
            closestNeighbor = neighbor;
        }
    }
    if (refMinDistance < minDistance)
        return nullptr;
    return closestNeighbor;
}

std::vector<Tile *> ScreenMap::getClosestTilesInRadius(int x, int y, int radius) const
{
    std::vector<Tile*> tiles;
    // prevent overflow when mouse is outside the screen
    x = std::clamp(x, 0, static_cast<int>(m_tilesMap[0].size()) - 1);
    y = std::clamp(y, 0, static_cast<int>(m_tilesMap.size()) - 1);
    int startX = std::max(0, x - radius);
    int endX = std::min(static_cast<int>(m_tilesMap[0].size()) -1, x + radius);
    int startY = std::max(0, y - radius);
    int endY = std::min(static_cast<int>(m_tilesMap.size() -1), y + radius);

    for (int i = startX; i <= endX; i++) {
        tiles.push_back(m_tilesMap[startY][i].get());
        tiles.push_back(m_tilesMap[endY][i].get());
    }

    for (int j = startY + 1; j <= endY - 1; j++) {
        tiles.push_back(m_tilesMap[j][startX].get());
        tiles.push_back(m_tilesMap[j][endX].get());
    }
    return tiles;
}

Tile* ScreenMap::getSelectedTileInRadius(sf::Vector2i pointWorldPosition, sf::Vector2f pointScreenPosition,
    int radius) const
{
    if (pointWorldPosition.x >= 0 && pointWorldPosition.x < m_tilesMap[0].size()
        && pointWorldPosition.y >= 0 && pointWorldPosition.y < m_tilesMap.size()
        && m_tilesMap[pointWorldPosition.y][pointWorldPosition.x]->containsPoint(pointScreenPosition))
        return m_tilesMap[pointWorldPosition.y][pointWorldPosition.x].get();
    for (int searchRadius = 1; searchRadius <= radius; searchRadius++) {
        std::vector<Tile *> tilesInRadius =  getClosestTilesInRadius(pointWorldPosition.x, pointWorldPosition.y, searchRadius);
        for (Tile* tile : tilesInRadius)
            if (tile->containsPoint(pointScreenPosition)) {
                return tile;
            }
    }
    return nullptr;
}

void ScreenMap::getSelectedTiles(const sf::Vector2i mouseWorldPosition, const sf::Vector2f mouseScreenPosition)
{
    Tile *hoveredTile = getSelectedTileInRadius(mouseWorldPosition, mouseScreenPosition, 3);
    if (hoveredTile == nullptr)
        return;
    for (ScreenTileCorner* corner : hoveredTile->getCorners())
        m_selectedCorners.push_back(corner);
}

void ScreenMap::getSelectedTilesCorners(const sf::Vector2i mouseWorldPosition, const sf::Vector2f mouseScreenPosition)
{
    // TO DO : radius should be at least equal to the highest tile height (abs)
    ScreenTileCorner* closestCorner = getClosestNeighborCornerInRadius(mouseWorldPosition, mouseScreenPosition, 3);
    if (closestCorner == nullptr)
        return;
    m_selectedCorners.push_back(closestCorner);
}

void ScreenMap::getSelectedCorners(const sf::RenderWindow &window, const SelectionMode selectionMode)
{
    // TO DO : radius should be at least equal to the highest tile height (abs)
    m_selectedCorners.clear();
    // get the current mouse position in the window in pixels
    const sf::Vector2i mousePixelScreenPosition = sf::Mouse::getPosition(window);
    // get it's real coordinates in the current view
    const sf::Vector2f mouseScreenPosition = window.mapPixelToCoords(mousePixelScreenPosition);
    // convert screen-space → isometric world → tile coords
    const sf::Vector2f tempPos = getMouseWorldPosition(mouseScreenPosition);
    const sf::Vector2i mouseWorldPosition = {static_cast<int>(std::round(tempPos.x)), static_cast<int>(std::round(tempPos.y))};

    if (selectionMode == SelectionMode::TILE_CORNER)
        getSelectedTilesCorners(mouseWorldPosition, sf::Vector2f(mouseScreenPosition));
    else
        getSelectedTiles(mouseWorldPosition, sf::Vector2f(mouseScreenPosition));
}

float ScreenMap::radToDeg(float rad)
{
    return rad * 180 / M_PI;
}

float ScreenMap::degToRad(float deg)
{
    return deg * M_PI / 180;
}

float ScreenMap::distanceBetweenPoints(const sf::Vector2f &p1, const sf::Vector2f &p2)
{
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

sf::Vector2f ScreenMap::world_to_screen(float point3dX, float point3dY, float point3dZ)
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

sf::Vector2f ScreenMap::getPointScreenPosition(const sf::Vector2f worldPosition, const int worldHeight)
{
    const float scaledWorldX = worldPosition.x * m_tileSizeX;
    const float scaledWorldY = worldPosition.y * m_tileSizeY;
    return world_to_screen(scaledWorldX, scaledWorldY, worldHeight)/* + m_translationOffset*/;
}

sf::Vector2f ScreenMap::rotateAroundZAxis(const float angle, const sf::Vector2f point)
{
    const float radAngle = degToRad(angle);
    sf::Vector2f rotatedPoint;

    // 2D rotation matrix applied
    rotatedPoint.x = point.x * std::cos(radAngle) - point.y * std::sin(radAngle);
    rotatedPoint.y = point.x * std::sin(radAngle) + point.y * std::cos(radAngle);
    return rotatedPoint;
}
