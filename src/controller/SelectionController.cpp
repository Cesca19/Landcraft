//
// Created by fran on 13/03/2026.
//

#include "SelectionController.hpp"

SelectionController::SelectionController()
{
}

SelectionController::~SelectionController()
{
}

void SelectionController::update(float deltaTime, sf::RenderWindow &window, SelectionMode selectionMode,
                                WorldModel &worldModel, const Camera &camera, bool &hasModelChanged)
{
    getSelectedCorners(window, camera, worldModel, selectionMode);
}

void SelectionController::draw(sf::RenderWindow &window, const Camera &camera)
{
    if (!m_selectedTileCorners.empty())
        m_selectionView.drawTileCorners(window, m_selectedTileCorners, camera);

    if (!m_selectedTiles.empty())
        m_selectionView.drawTiles(window, m_selectedTiles, camera);
}

void SelectionController::getSelectedCorners(const sf::RenderWindow &window, const Camera &camera, WorldModel &worldModel, SelectionMode selectionMode)
{
    // TO DO : radius should be at least equal to the highest tile height (abs)
    
    m_selectedTileCorners.clear();
    m_selectedTiles.clear();
    // get the current mouse position in the window in pixels
    const sf::Vector2i mousePixelScreenPosition = sf::Mouse::getPosition(window);
    // get it's real coordinates in the current view
    const sf::Vector2f mouseScreenPosition = window.mapPixelToCoords(mousePixelScreenPosition);
    // convert screen-space → isometric world → tile coords
    const sf::Vector2f tempPos = camera.screen_to_world(mouseScreenPosition.x, mouseScreenPosition.y, 0); //getPointTileCoordinates(mouseScreenPosition);
    const sf::Vector2i mouseWorldPosition = {static_cast<int>(std::round(tempPos.x)), static_cast<int>(std::round(tempPos.y))};

    if (selectionMode == SelectionMode::TILE_CORNER)
        getSelectedTilesCorners(camera, worldModel, mouseWorldPosition, sf::Vector2f(mouseScreenPosition));
    else
        getSelectedTiles(camera, worldModel, mouseWorldPosition, sf::Vector2f(mouseScreenPosition));
    
}

void SelectionController::getSelectedTilesCorners(const Camera &camera, WorldModel &worldModel, sf::Vector2i mouseWorldPosition, sf::Vector2f mouseScreenPosition)
{
    // TO DO : radius should be at least equal to the highest tile height (abs)
    TileCorner* closestCorner = getClosestNeighborCornerInRadius(camera, worldModel, mouseWorldPosition, mouseScreenPosition, 3);
    if (closestCorner == nullptr)
        return;
    m_selectedTileCorners.push_back(closestCorner);
}

void SelectionController::getSelectedTiles(const Camera &camera, WorldModel &worldModel, sf::Vector2i mouseWorldPosition, sf::Vector2f mouseScreenPosition)
{
    Tile *hoveredTile = getSelectedTileInRadius(camera, worldModel, mouseWorldPosition, mouseScreenPosition, 3);
    if (hoveredTile == nullptr)
        return;
    m_selectedTiles.push_back(hoveredTile);
}

TileCorner *SelectionController::getClosestNeighborCornerInRadius(const Camera &camera, WorldModel &worldModel, sf::Vector2i pointWorldPosition, 
                                                                    sf::Vector2f pointScreenPosition, int radius) const
{
    const std::vector<TileCorner*> neighbors = getPointNeighborsInRadius(camera, worldModel, pointWorldPosition.x, pointWorldPosition.y, radius);
    if (neighbors.empty())
        return nullptr;
    TileCorner* closestNeighbor = neighbors[0];
    float minDistance = MathUtils::distanceBetweenPoints(getTileCornerScreenCoordinates(camera, neighbors[0]), pointScreenPosition);
    float refMinDistance = std::max(camera.getTileSizeX(), camera.getTileSizeY());

    for ( TileCorner* neighbor : neighbors) {
        float dist = MathUtils::distanceBetweenPoints(getTileCornerScreenCoordinates(camera, neighbor), pointScreenPosition);
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

std::vector<TileCorner *> SelectionController::getPointNeighborsInRadius(const Camera &camera, WorldModel &worldModel, int x, int y, int radius ) const
{
    std::vector<TileCorner*> neighbors;
    std::vector<std::vector<std::unique_ptr<TileCorner>>>& map = worldModel.getCorners();
    // prevent overflow when mouse is outside the screen
    x = std::clamp(x, 0, static_cast<int>(map[0].size()));
    y = std::clamp(y, 0, static_cast<int>(map.size()));

    int startX = std::max(0, x - radius);
    int endX = std::min(static_cast<int>(map[0].size()), x + radius);
    int startY = std::max(0, y - radius);
    int endY = std::min(static_cast<int>(map.size()), y + radius);

    for (int j = startY; j < endY; j++)
        for (int i = startX; i < endX; i++)
            neighbors.push_back(map[j][i].get());
    return neighbors;
}

Tile *SelectionController::getSelectedTileInRadius(const Camera &camera, WorldModel &worldModel, sf::Vector2i pointWorldPosition, sf::Vector2f pointScreenPosition, int radius) const
{
    //pointScreenPosition = camera.screen_to_world(pointScreenPosition.x, pointScreenPosition.y, 0);
    std::vector<std::vector<Tile>>& tilesMap = worldModel.getTiles();
    if (pointWorldPosition.x >= 0 && pointWorldPosition.x < tilesMap[0].size()
        && pointWorldPosition.y >= 0 && pointWorldPosition.y < tilesMap.size()
        && isPointInsideTile(camera, &tilesMap[pointWorldPosition.y][pointWorldPosition.x], pointScreenPosition))
        return &tilesMap[pointWorldPosition.y][pointWorldPosition.x];
    for (int searchRadius = 1; searchRadius <= radius; searchRadius++) {
        const std::vector<Tile *> &tilesInRadius =  getClosestTilesInRadius(camera, worldModel, 
            pointWorldPosition.x, pointWorldPosition.y, searchRadius);
        for (  Tile *tile : tilesInRadius)
            if (isPointInsideTile(camera, tile, pointScreenPosition))
                return tile;

    }
    return nullptr;
}

std::vector<Tile *> SelectionController::getClosestTilesInRadius(const Camera &camera, WorldModel &worldModel, int x, int y, int radius) const
{
    std::vector<Tile *> tiles;
    std::vector<std::vector<Tile>>& tilesMap = worldModel.getTiles();

    // prevent overflow when mouse is outside the screen
    x = std::clamp(x, 0, static_cast<int>(tilesMap[0].size()) - 1);
    y = std::clamp(y, 0, static_cast<int>(tilesMap.size()) - 1);
    int startX = std::max(0, x - radius);
    int endX = std::min(static_cast<int>(tilesMap[0].size()) -1, x + radius);
    int startY = std::max(0, y - radius);
    int endY = std::min(static_cast<int>(tilesMap.size() -1), y + radius);

    for (int i = startX; i <= endX; i++) {
        tiles.push_back(&tilesMap[startY][i]);
        tiles.push_back(&tilesMap[endY][i]);
    }

    for (int j = startY + 1; j <= endY - 1; j++) {
        tiles.push_back(&tilesMap[j][startX]);
        tiles.push_back(&tilesMap[j][endX]);
    }
    return tiles;
}

bool SelectionController::isPointInsideTile(const Camera &camera, Tile *tile, sf::Vector2f pointScreenPosition) const
{
    std::vector<TileCorner*> tileCorners = tile->getCorners();
    if (tileCorners.size() != 4)
        return false;
    sf::Vector2f corner0ScreenPos = getTileCornerScreenCoordinates(camera, tileCorners[0]);
    sf::Vector2f corner1ScreenPos = getTileCornerScreenCoordinates(camera, tileCorners[1]);
    sf::Vector2f corner2ScreenPos = getTileCornerScreenCoordinates(camera, tileCorners[2]);
    sf::Vector2f corner3ScreenPos = getTileCornerScreenCoordinates(camera, tileCorners[3]);

    if (MathUtils::isInsideTriangle(pointScreenPosition, corner0ScreenPos, corner1ScreenPos, corner2ScreenPos)
        || MathUtils::isInsideTriangle(pointScreenPosition, corner2ScreenPos, corner3ScreenPos, corner0ScreenPos))
        return true;

    return false;
}

sf::Vector2f SelectionController::getTileCornerScreenCoordinates(const Camera &camera, TileCorner *corner) const
{
    return camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
}
