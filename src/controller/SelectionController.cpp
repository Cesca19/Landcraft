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

sf::Vector2f SelectionController::getTileCornerScreenCoordinates(const Camera &camera, TileCorner *corner) const
{
    return camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
}
