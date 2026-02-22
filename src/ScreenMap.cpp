#include "ScreenMap.hpp"
#include <iostream>

ScreenMap::ScreenMap(const float tileSizeX, const float tileSizeY, const float heightScale, const float projectionAngleX, const float projectionAngleY)
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
    , m_gizmoVertexArray(sf::Lines)
    , m_worldReferenceVertexArray(sf::Lines)
    , m_lastPitchRotationAngle(0)
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

    // by modifying the word pivot like that I make sure that the center of the map
    // which world coordinates are (mapWidth/2, mapHeight/2) in world space,
    // and which is the point around which I want to rotate the map,
    // is at the center of the view,an then will have {0, 0} 
    setWorldPivot(getScreenMapCenter());
    m_doesNeedVertexUpdate = true;
}

void ScreenMap::setSelectedCornersHeight(const float heightOffset)
{
    for (ScreenTileCorner *corner : m_selectedCorners) {
        m_worldMap->setCornerHeight(heightOffset, corner->WorldPosition);
        corner->WorldHeight += heightOffset;
        corner->ScreenHeight += (m_heightScale * heightOffset);
        corner->ScreenPosition = m_isometricProjection.getPointScreenPosition(corner->RotatedWorldPosition, corner->WorldHeight);
    }
    m_doesNeedVertexUpdate = true;
}

sf::Vector2f ScreenMap::getWorldMapCenter() const
{
    const float centerX = (static_cast<float>(m_map[0].size()) - 1.0f) / 2.0f;
    const float centerY = (static_cast<float>(m_map.size()) - 1.0f) / 2.0f;

    return {centerX, centerY};
}

sf::Vector2f ScreenMap::getScreenMapCenter() const
{
    const sf::Vector2f worldCenter = getWorldMapCenter();

    // convert that point into screen space coordinates
    return m_isometricProjection.getPointScreenPosition({std::round(worldCenter.x), std::round(worldCenter.y)}, 0);
}

void ScreenMap::rotateAroundZAxis(const float angle)
{
    m_targetYawRotationAngle += angle;
}

void ScreenMap::rotateAroundXAxis(const float angle)
{
    m_targetPitchRotationAngle += angle;
}

void ScreenMap::drawGizmo(sf::RenderWindow &window, const sf::Vector2f &uiPosition, const float size) {
    const sf::Vector2f rotatedX = IsometricProjection::rotateAroundZAxis(m_currentYawRotationAngle, {1.0f, 0.0f});
    const sf::Vector2f rotatedY = IsometricProjection::rotateAroundZAxis(m_currentYawRotationAngle, {0.0f, 1.0f});
    // projected origin point
    const sf::Vector2f origin = m_isometricProjection.getPointScreenPosition(sf::Vector2f(0, 0), 0);
    const sf::Vector2f pX = m_isometricProjection.getPointScreenPosition(rotatedX, 0);
    const sf::Vector2f pY = m_isometricProjection.getPointScreenPosition(rotatedY, 0);
    // Z axis isn't rotated
    const sf::Vector2f pZ = m_isometricProjection.getPointScreenPosition(sf::Vector2f(0, 0), 1.0f);
    // normalized direction vectors
    sf::Vector2f dirX = IsometricProjection::normalize({pX - origin});
    sf::Vector2f dirY = IsometricProjection::normalize({pY - origin});
    sf::Vector2f dirZ = IsometricProjection::normalize({pZ - origin});

    if (m_gizmoAxes.size() == 3 && m_gizmoAxes[0] == dirX && m_gizmoAxes[1] == dirY && m_gizmoAxes[2] == dirZ) {
        window.draw(m_gizmoVertexArray);
        return;
    }
    m_gizmoAxes.clear();
    m_gizmoAxes.push_back(dirX);
    m_gizmoAxes.push_back(dirY);
    m_gizmoAxes.push_back(dirZ);
    // scaling dir vectors
    dirX *= size;
    dirY *= size;
    dirZ *= size;
    m_gizmoVertexArray.clear();
    // x axis
    m_gizmoVertexArray.append(sf::Vertex(uiPosition, sf::Color::Red));
    m_gizmoVertexArray.append(sf::Vertex(uiPosition + dirX, sf::Color::Red));
    // y axis
    m_gizmoVertexArray.append(sf::Vertex(uiPosition, sf::Color::Green));
    m_gizmoVertexArray.append(sf::Vertex(uiPosition + dirY, sf::Color::Green));
    // Z axis
    m_gizmoVertexArray.append(sf::Vertex(uiPosition, sf::Color::Blue));
    m_gizmoVertexArray.append(sf::Vertex(uiPosition + dirZ, sf::Color::Blue));
    window.draw(m_gizmoVertexArray);
}

void ScreenMap::drawWorldReference(sf::RenderWindow &window, const sf::Vector2f viewCenter, const sf::Vector2f viewSize)
{
    const sf::Vector2f rotatedOrigin = IsometricProjection::rotateAroundZAxis(m_currentYawRotationAngle, {0, 0});
    const sf::Vector2f yPointWorld = IsometricProjection::rotateAroundZAxis(m_currentYawRotationAngle, sf::Vector2f(0, 1));
    const sf::Vector2f xPointWorld = IsometricProjection::rotateAroundZAxis(m_currentYawRotationAngle, sf::Vector2f(1, 0));
    const sf::Vector2f xAxisWorld = xPointWorld - rotatedOrigin;
    const sf::Vector2f yAxisWorld = yPointWorld - rotatedOrigin;
    const sf::Vector2f xAxisNormal = IsometricProjection::normalize(sf::Vector2f{-xAxisWorld.y, xAxisWorld.x}); // Perpendicular to the view X axis (to get the up direction)
    const sf::Vector2f yAxisNormal = IsometricProjection::normalize(sf::Vector2f{-yAxisWorld.y, yAxisWorld.x}); // Perpendicular to the view Y axis (to get the left direction)
    constexpr float height = 0.0f;
    const sf::Vector2f mapCenter = getWorldMapCenter();
    sf::Vector2f viewCenterWorld = getPointTileCoordinates(viewCenter, height);
    viewCenterWorld  = {std::round(viewCenterWorld.x), std::round(viewCenterWorld.y)};
    const sf::Vector2f worldCenterRotated = IsometricProjection::rotateAroundZAxis(m_currentYawRotationAngle, viewCenterWorld - mapCenter) + mapCenter;

    if (m_worldReferenceAxesNormals.size() == 3 && m_worldReferenceAxesNormals[0] == xAxisNormal
        && m_worldReferenceAxesNormals[1] == yAxisNormal  && m_worldReferenceAxesNormals[2] == worldCenterRotated
        && m_lastPitchRotationAngle == m_currentPitchRotationAngle && m_lastViewSize == viewSize) {
        window.draw(m_worldReferenceVertexArray);
        return;
    }
    m_worldReferenceAxesNormals.clear();
    m_worldReferenceAxesNormals.push_back(xAxisNormal);
    m_worldReferenceAxesNormals.push_back(yAxisNormal);
    m_worldReferenceAxesNormals.push_back(worldCenterRotated);
    m_lastPitchRotationAngle = m_currentPitchRotationAngle;
    m_lastViewSize = viewSize;

    const float screenViewDiagonalLength = std::hypot(viewSize.x, viewSize.y);
    const auto minTileScale = std::min(m_tileSizeX, m_tileSizeY);
    const int screenViewRadius = static_cast<int>(std::round(screenViewDiagonalLength / minTileScale)) * 2;
    const sf::Vector2f offsetUpViewCenterWorld = IsometricProjection::offsetPointAlongDirection(worldCenterRotated, yAxisWorld, screenViewRadius);
    const sf::Vector2f offsetDownViewCenterWorld = IsometricProjection::offsetPointAlongDirection(worldCenterRotated, yAxisWorld, -screenViewRadius);
    const sf::Vector2f offsetLeftViewCenterWorld = IsometricProjection::offsetPointAlongDirection(worldCenterRotated, xAxisWorld, -screenViewRadius);
    const sf::Vector2f offsetRightViewCenterWorld = IsometricProjection::offsetPointAlongDirection(worldCenterRotated, xAxisWorld, screenViewRadius);
    const sf::Color linesColor(255, 255, 255, 50); // White semi-transparent lines (Wireframe)

    m_worldReferenceVertexArray.clear();
    for (int i = -screenViewRadius; i <= screenViewRadius; i++) {
        const auto normalScale = static_cast<float>(i);
        // y axis parallel lines
        const sf::Vector2f upPositionOnParallelAxis = offsetUpViewCenterWorld +  (yAxisNormal * normalScale);
        const sf::Vector2f downPositionOnParallelAxis = offsetDownViewCenterWorld + (yAxisNormal * normalScale);
        m_worldReferenceVertexArray.append(sf::Vertex(m_isometricProjection.getPointScreenPosition(upPositionOnParallelAxis, height), linesColor));
        m_worldReferenceVertexArray.append(sf::Vertex(m_isometricProjection.getPointScreenPosition(downPositionOnParallelAxis, height), linesColor));

        // x axis parallel lines
        const sf::Vector2f leftPositionOnParallelAxis = offsetLeftViewCenterWorld +  (xAxisNormal * normalScale);
        const sf::Vector2f rightPositionOnParallelAxis = offsetRightViewCenterWorld + (xAxisNormal * normalScale);
        m_worldReferenceVertexArray.append(sf::Vertex(m_isometricProjection.getPointScreenPosition(leftPositionOnParallelAxis, height), linesColor));
        m_worldReferenceVertexArray.append(sf::Vertex(m_isometricProjection.getPointScreenPosition(rightPositionOnParallelAxis, height), linesColor));
    }
    window.draw(m_worldReferenceVertexArray);
}

void ScreenMap::setWorldPivot(const sf::Vector2f worldPivotScreenPosition)
{
    m_isometricProjection.setWorldPivot(worldPivotScreenPosition);
    updateMap();
}

void ScreenMap::updateMap()
{
    for (auto & screenTileCornerList : m_map)
        for (const auto & screenTileCorner : screenTileCornerList)
            updateCorner(screenTileCorner.get());
    m_doesNeedVertexUpdate = true;
}

void ScreenMap::updateCorner(ScreenTileCorner *corner) const
{
    corner->ScreenPosition = m_isometricProjection.getPointScreenPosition(corner->RotatedWorldPosition, corner->WorldHeight);
}


void ScreenMap::rotateMapAroundZAxis(const float angle)
{
    for (auto & screenTileCornerList : m_map)
        for (const auto & screenTileCorner : screenTileCornerList)
            rotateCornerAroundZAxis(angle, screenTileCorner.get());
    m_doesNeedVertexUpdate = true;
}

void ScreenMap::rotateCornerAroundZAxis(const float angle, ScreenTileCorner *corner) const
{
    const sf::Vector2f worldCenter = {(static_cast<float>(m_map[0].size()) -1.0f) / 2,
        (static_cast<float>(m_map.size()) - 1.0f) / 2 };
    // translate the point to rotate around the maps center
    // then cancel this translation to avoid the gap
    corner->RotatedWorldPosition = IsometricProjection::rotateAroundZAxis(angle, sf::Vector2f(corner->WorldPosition) - worldCenter) + worldCenter;
    corner->ScreenPosition = m_isometricProjection.getPointScreenPosition(corner->RotatedWorldPosition, corner->WorldHeight);
}

void ScreenMap::rotateMapAroundXAxis(const float angle)
{
    m_isometricProjection.rotateAroundXAxis(angle);
    updateMap();
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

sf::Vector2f ScreenMap::getPointScreenCoordinates(sf::Vector2f pointWorld, float height) const
{
    // isometric projection already applies Pitch rotation, so we only need to apply Yaw rotation to the point before projecting it to screen space
    const sf::Vector2f pointWorldRotated = IsometricProjection::rotateAroundZAxis(m_currentYawRotationAngle, pointWorld);
    return m_isometricProjection.getPointScreenPosition(pointWorldRotated, height);
}

sf::Vector2f ScreenMap::getPointTileCoordinates(const sf::Vector2f pointScreenPosition, float height) const
{
    const sf::Vector2f worldCenter = {static_cast<float>(m_map[0].size() -1.0f) / 2, static_cast<float>(m_map.size() - 1.0f) / 2 };
    const sf::Vector2f worldPosition = m_isometricProjection.screen_to_world(pointScreenPosition.x, pointScreenPosition.y, height);

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
    const sf::Vector2f tempPos = getPointTileCoordinates(mouseScreenPosition);
    const sf::Vector2i mouseWorldPosition = {static_cast<int>(std::round(tempPos.x)), static_cast<int>(std::round(tempPos.y))};

    if (selectionMode == SelectionMode::TILE_CORNER)
        getSelectedTilesCorners(mouseWorldPosition, sf::Vector2f(mouseScreenPosition));
    else
        getSelectedTiles(mouseWorldPosition, sf::Vector2f(mouseScreenPosition));
    if (!m_selectedCorners.empty())
        m_doesNeedVertexUpdate = true;
}