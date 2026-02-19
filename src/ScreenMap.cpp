#include "ScreenMap.hpp"

ScreenMap::ScreenMap(const int tileSizeX, const int tileSizeY, const int heightScale, const int projectionAngleX, const int projectionAngleY)
    : m_tileSizeX(tileSizeX)
    , m_tileSizeY(tileSizeY)
    , m_heightScale(heightScale)
    , m_isometricProjection(tileSizeX, tileSizeY, heightScale, projectionAngleX, projectionAngleY)
    , m_yawRotationSpeed(10)
    , m_currentYawRotationAngle(0)
    , m_targetYawRotationAngle(0)
    , m_pitchRotationSpeed(20)
    , m_currentPitchRotationAngle(projectionAngleY)
    , m_targetPitchRotationAngle(projectionAngleY)
    , m_doesNeedVertexUpdate(true)
    , m_vertexArrayMap(sf::Lines)
    , m_worldMap(std::make_shared<WorldMap>())
{
}

ScreenMap::~ScreenMap()
{
}

void ScreenMap::update(const float deltaTime, const sf::RenderWindow &window, const SelectionMode selectionMode)
{
    getSelectedCorners(window, selectionMode);
    // TO DO : add a selection layer to handle selected tiles colors
    resetTilesCornerColors();
    setSelectedTileCornersColors();
    // upd yaw rotation
    if (std::abs(m_targetYawRotationAngle - m_currentYawRotationAngle) > m_epsilon) {
        m_currentYawRotationAngle = m_currentYawRotationAngle + (m_targetYawRotationAngle - m_currentYawRotationAngle) * m_yawRotationSpeed * deltaTime;
        rotateMapAroundZAxis(m_currentYawRotationAngle);
    } else
        if (m_currentYawRotationAngle != m_targetYawRotationAngle) {
            m_currentYawRotationAngle = m_targetYawRotationAngle;
            rotateMapAroundZAxis(m_currentYawRotationAngle);
        }

    //upd pitch rotation
    if (std::abs(m_targetPitchRotationAngle - m_currentPitchRotationAngle) > m_epsilon) {
        m_currentPitchRotationAngle = m_currentPitchRotationAngle + (m_targetPitchRotationAngle - m_currentPitchRotationAngle) * m_pitchRotationSpeed * deltaTime;
        rotateMapAroundXAxis(m_currentPitchRotationAngle);
    } else
        if (m_currentPitchRotationAngle != m_targetPitchRotationAngle) {
            m_currentPitchRotationAngle = m_targetPitchRotationAngle;
            rotateMapAroundXAxis(m_currentPitchRotationAngle);
        }
}

void ScreenMap::draw(sf::RenderWindow &window)
{
    if (m_doesNeedVertexUpdate) {
        buildVertexArrayMap();
        m_doesNeedVertexUpdate = false;
    }
    window.draw(m_vertexArrayMap);
}

void ScreenMap::init(const std::string &mapFilepath)
{
    m_worldMap->init(mapFilepath);
    initTilesCornersMap();
    initTilesMap();
    m_doesNeedVertexUpdate = true;
}

void ScreenMap::setSelectedCornersHeight(const int heightOffset)
{
    for (ScreenTileCorner *corner : m_selectedCorners) {
        m_worldMap->setCornerHeight(heightOffset, corner->WorldPosition);
        corner->WorldHeight += heightOffset;
        corner->ScreenHeight += (heightOffset * m_heightScale);
        corner->ScreenPosition = m_isometricProjection.getPointScreenPosition(corner->RotatedWorldPosition, corner->WorldHeight);
    }
    m_doesNeedVertexUpdate = true;
}

sf::Vector2f ScreenMap::getScreenMapCenter() const
{
    const float centerX = (static_cast<float>(m_map[0].size()) - 1.0f) / 2.0f;
    const float centerY = (static_cast<float>(m_map.size()) - 1.0f) / 2.0f;

    // convert that point into screen space coordinates
    return m_isometricProjection.getPointScreenPosition({std::round(centerX), std::round(centerY)}, 0);
}

void ScreenMap::rotateAroundZAxis(const float angle)
{
    m_targetYawRotationAngle += angle;
}

void ScreenMap::rotateAroundXAxis(const float angle)
{
    m_targetPitchRotationAngle += angle;
}

void ScreenMap::drawGizmo(sf::RenderWindow &window, const sf::Vector2f &uiPosition, float size)
{
    sf::Vector2f worldX(1.0f, 0.0f);
    sf::Vector2f worldY(0.0f, 1.0f);
    sf::Vector2f rotatedX = IsometricProjection::rotateAroundZAxis(m_currentYawRotationAngle, worldX);
    sf::Vector2f rotatedY = IsometricProjection::rotateAroundZAxis(m_currentYawRotationAngle, worldY);

    // projected origin point
    sf::Vector2f center = m_isometricProjection.getPointScreenPosition(sf::Vector2f(0, 0), 0);
    sf::Vector2f pX = m_isometricProjection.getPointScreenPosition(rotatedX, 0);
    sf::Vector2f pY = m_isometricProjection.getPointScreenPosition(rotatedY, 0);
    // Z axis isn't rotated
    sf::Vector2f pZ = m_isometricProjection.getPointScreenPosition(sf::Vector2f(0, 0), 1.0f);

    // direction vectors
    sf::Vector2f dirX = pX - center;
    sf::Vector2f dirY = pY - center;
    sf::Vector2f dirZ = pZ - center;

    auto normalize = [](sf::Vector2f v) { float l = std::hypot(v.x, v.y); return (l>0)? v/l : v; };
    dirX = normalize(dirX) * size;
    dirY = normalize(dirY) * size;
    dirZ = normalize(dirZ) * size;

    sf::Vertex lines[] = {
        // AXE X (Rouge)
        sf::Vertex(uiPosition, sf::Color::Red),
        sf::Vertex(uiPosition + dirX, sf::Color::Red),

        // AXE Y (Vert)
        sf::Vertex(uiPosition, sf::Color::Green),
        sf::Vertex(uiPosition + dirY, sf::Color::Green),

        // AXE Z (Bleu - Vertical/Hauteur)
        sf::Vertex(uiPosition, sf::Color::Blue),
        sf::Vertex(uiPosition + dirZ, sf::Color::Blue)
    };

    window.draw(lines, 6, sf::Lines);
}

void ScreenMap::rotateMapAroundZAxis(const float angle)
{
    for (int y = 0; y < m_map.size(); y++)
        for (int x = 0; x < m_map[y].size(); x++)
            rotateCornerAroundZAxis(angle, m_map[y][x].get());
    m_doesNeedVertexUpdate = true;
}

void ScreenMap::rotateCornerAroundZAxis(const float angle, ScreenTileCorner *corner) const
{
    const sf::Vector2f worldCenter = {static_cast<float>(m_map[0].size() -1.0f) / 2, static_cast<float>(m_map.size() - 1.0f) / 2 };
    // translate the point to rotate around the maps center
    // then cancel this translation to avoid the gap
    corner->RotatedWorldPosition = IsometricProjection::rotateAroundZAxis(angle, sf::Vector2f(corner->WorldPosition) - worldCenter) + worldCenter;
    corner->ScreenPosition = m_isometricProjection.getPointScreenPosition(corner->RotatedWorldPosition, corner->WorldHeight);
}

void ScreenMap::rotateMapAroundXAxis(const float angle)
{
    m_isometricProjection.rotateAroundXAxis(angle);

    for (int y = 0; y < m_map.size(); y++)
        for (int x = 0; x < m_map[y].size(); x++)
            rotateCornerAroundXAxis(angle, m_map[y][x].get());
    m_doesNeedVertexUpdate = true;
}

void ScreenMap::rotateCornerAroundXAxis(float angle, ScreenTileCorner *corner) const
{
    corner->ScreenPosition = m_isometricProjection.getPointScreenPosition(corner->RotatedWorldPosition, corner->WorldHeight);
}

void ScreenMap::initTilesCornersMap()
{
    const std::vector<std::vector<TileCorner>> worldMap = m_worldMap->getMap();
    m_map.clear();
    for (int y = 0; y < worldMap.size(); y++) {
        std::vector<std::unique_ptr<ScreenTileCorner>> row;
        for (int x = 0; x < worldMap[y].size(); x++) {
           sf::Vector2f screenPos = m_isometricProjection.getPointScreenPosition(sf::Vector2f(worldMap[y][x].Position), worldMap[y][x].Height);
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

void ScreenMap::buildVertexArrayMap()
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

sf::Vector2f ScreenMap::getMouseWorldPosition(const sf::Vector2f mouseScreenPosition) const
{
    const sf::Vector2f worldCenter = {static_cast<float>(m_map[0].size() -1.0f) / 2, static_cast<float>(m_map.size() - 1.0f) / 2 };
    const sf::Vector2f worldPosition = m_isometricProjection.screen_to_world(mouseScreenPosition.x, mouseScreenPosition.y, 0);

    // inversed of the transformation applied to rotated points
    const sf::Vector2f finalWorldPos = IsometricProjection::rotateAroundZAxis(-m_currentYawRotationAngle, worldPosition - worldCenter) + worldCenter;
    return finalWorldPos;
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

std::vector<ScreenTileCorner*> ScreenMap::getPointNeighborsInRadius(int x, int y, const int radius) const
{
    std::vector<ScreenTileCorner*> neighbors;
    // prevent overflow when mouse is outside the screen
    x = std::clamp(x, 0, static_cast<int>(m_map[0].size()));
    y = std::clamp(y, 0, static_cast<int>(m_map.size()));

    int startX = std::max(0, x - radius);
    int endX = std::min(static_cast<int>(m_map[0].size()), x + radius);
    int startY = std::max(0, y - radius);
    int endY = std::min(static_cast<int>(m_map.size()), y + radius);

    for (int j = startY; j < endY; j++)
        for (int i = startX; i < endX; i++)
            neighbors.push_back(m_map[j][i].get());
    return neighbors;
}

ScreenTileCorner* ScreenMap::getClosestNeighborCornerInRadius(const sf::Vector2i pointWorldPosition,
    const sf::Vector2f pointScreenPosition, const int radius) const
{
    const std::vector<ScreenTileCorner*> neighbors = getPointNeighborsInRadius(pointWorldPosition.x, pointWorldPosition.y, radius);
    if (neighbors.empty())
        return nullptr;
    ScreenTileCorner* closestNeighbor = neighbors[0];
    float minDistance = IsometricProjection::distanceBetweenPoints(neighbors[0]->ScreenPosition, pointScreenPosition);
    float refMinDistance = std::max(m_tileSizeX, m_tileSizeY);

    for ( ScreenTileCorner* neighbor : neighbors) {
        float dist = IsometricProjection::distanceBetweenPoints(neighbor->ScreenPosition, pointScreenPosition);
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
    if (!m_selectedCorners.empty())
        m_doesNeedVertexUpdate = true;
}