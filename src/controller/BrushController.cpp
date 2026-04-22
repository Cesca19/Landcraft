//
// Created by fran on 13/03/2026.
//

#include "BrushController.hpp"

BrushController::BrushController(const sf::Vector2f uiStartPosition)
    : m_mouseWorldPosition(-1, -1)
    , m_brushSize(0)
    , m_brushSizeMin(0)
    , m_brushSizeMax(10)
    , m_currentBrushImage(-1)
    , m_brushView(std::make_unique<BrushView>())
    , m_brushMenu(nullptr)
{
    std::vector<std::string> brushImagePaths = {
        "assets/textures/brushes/Carre_Plein.png",
        "assets/textures/brushes/Hexagone.png",
        "assets/textures/brushes/Brosse_Bruit.png",
        "assets/textures/brushes/Etoile_Trouee.png",
        "assets/textures/brushes/Cercle_Dur.png",
        "assets/textures/brushes/Cercle_Flou_Petit.png",
        "assets/textures/brushes/Cercle_Flou_Grand.png",
        "assets/textures/brushes/Etoile.png"
    };
    m_brushMenu = std::make_unique<BrushMenu>(uiStartPosition, brushImagePaths);
    m_brushMenu->setIncrementBrushSizeButtonCallback([this]() { incrementBrushSize(); });
    m_brushMenu->setDecrementBrushSizeButtonCallback([this]() { decrementBrushSize(); });
    m_brushMenu->setBrushSizeValueText(getBrushSizeValue());

    for (int i = 0; i < brushImagePaths.size(); i++) {
        m_brushesImages.push_back(ResourceManager::getInstance().getImage(brushImagePaths[i]));
        m_brushMenu->setBrushTypeButtonCallback(i, [this, i] () { selectBrush(i); });
    }
    selectBrush(0);
}

void BrushController::handleEvents(const sf::RenderWindow &window, const sf::Event &event)
{
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Add) {
        selectBrush((m_currentBrushImage + 1) % static_cast<int>(m_brushesImages.size()));
    }
    else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Subtract) {
        selectBrush(m_currentBrushImage - 1 + static_cast<int>(m_brushesImages.size()) % static_cast<int>(m_brushesImages.size()));
    }
}

void BrushController::update(float deltaTime, const sf::RenderWindow &window, const SelectionMode selectionMode,
                                WorldModel &worldModel, const Camera &camera)
{
    getSelectedCorners(window, camera, worldModel, selectionMode);
    // should we hide the mouse cursor when something is hovered inside the map
}

void BrushController::draw(sf::RenderWindow &window, const Camera &camera)
{
    float radius = static_cast<float>(m_brushSize);
    float hilightTransparency = (m_brushSelectionTiles.empty()) ? 100 : 75;
    
    // MODE CORNER: Draw the full smooth Unity overlay + pins
    if (!m_tilesToHilight.empty() && m_brushSelectionTiles.empty()) {
        m_brushView->drawBrushOverlay(window, m_tilesToHilight, camera, m_brushCenterWorldPosition, radius, m_brushesImages[m_currentBrushImage], hilightTransparency);
    }
    
    if (m_brushSelectionTiles.empty() && !m_brushSelectionTileCorners.empty()) {
        m_brushView->drawTileCorners(window, m_brushSelectionTileCorners, camera);
    }
        
    // MODE TILE: Draw only the border outline of the brush + colored tiles
    if (!m_brushSelectionTiles.empty()) {
        
        // 1. Draw the actual colored tiles with opacity based on their weight
        m_brushView->drawTiles(window, m_brushSelectionTiles, camera);
        
        // 2. Draw the perfect smooth border of the brush image on top
        if (!m_tilesToHilight.empty()) {
            m_brushView->drawBrushBorder(window, m_tilesToHilight, camera, m_brushCenterWorldPosition, radius, m_brushesImages[m_currentBrushImage]);
        }
    }
}

sf::Vector2i BrushController::getMouseWorldPosition() const
{
    return m_mouseWorldPosition;
}

bool BrushController::isAnyTileCornerSelected() const
{
    return !m_brushSelectionTileCorners.empty() || !m_brushSelectionTiles.empty();
}

const std::vector<BrushTileCornerHit>& BrushController::getBrushTileCornersSelection() const
{
    return m_brushSelectionTileCorners;
}

const std::vector<BrushTileHit>& BrushController::getBrushTilesSelection() const
{
    return m_brushSelectionTiles;
}

const std::vector<BrushTileCornerHit> &BrushController::getBrushTilesSelectionAsTileCorners() const
{
    return m_brushSelectionTilesAsTileCorners;
}

std::vector<BrushTileHit> BrushController::getNeighborsTilesInBrush(WorldModel &worldModel, const int x, const int y) const
{
    const std::vector<Tile *> tiles = getClosestTilesInRadius(worldModel, x, y, m_brushSize, true);
    std::vector<BrushTileHit> selection;
    for (Tile* tile : tiles)
        selection.push_back(BrushTileHit{tile, getPointWeightInBrush(tile->getCenterGridPosition(), sf::Vector2f(x, y))});
    return selection;

}

std::vector<BrushTileCornerHit> BrushController::getNeighborsTileCornersInBrush(WorldModel &worldModel, const int x, const int y) const
{
    const std::vector<TileCorner*> corners = getPointNeighborsInRadius(worldModel, x, y, m_brushSize);
    std::vector<BrushTileCornerHit> selection;
    for (TileCorner* corner : corners)
        selection.push_back(BrushTileCornerHit{corner, getPointWeightInBrush(corner->getPosition(), sf::Vector2f(x, y))});
    return selection;
}

std::vector<BrushTileCornerHit> BrushController::getNeighborsTilesInBrushAsTileCorners(WorldModel &worldModel, const int x, const int y) const
{
    const std::vector<Tile *> tiles = getClosestTilesInRadius(worldModel, x, y, m_brushSize, true);
    std::unordered_map<TileCorner*, float> uniqueCorners;

    for (const Tile *tile : tiles) {
        const float weight = getPointWeightInBrush(tile->getCenterGridPosition(), sf::Vector2f(x, y));
        for (TileCorner *corner : tile->getCorners())
            if (uniqueCorners.find(corner) == uniqueCorners.end() || uniqueCorners[corner] < weight)
                uniqueCorners[corner] = weight;
    }

    std::vector<BrushTileCornerHit> selection;
    for (const auto&[corner, weight] : uniqueCorners)
        selection.push_back(BrushTileCornerHit{corner, weight});

    return selection;
}

void BrushController::getSelectedCorners(const sf::RenderWindow &window, const Camera &camera, WorldModel &worldModel, const SelectionMode selectionMode)
{
    m_brushSelectionTiles.clear();
    m_brushSelectionTileCorners.clear();
    m_brushSelectionTilesAsTileCorners.clear();
    m_mouseWorldPosition = sf::Vector2i(-1, -1);
    // get the current mouse position in the window in pixels
    const sf::Vector2i mousePixelScreenPosition = sf::Mouse::getPosition(window);
    // get it's real coordinates in the current view
    const sf::Vector2f mouseScreenPosition = window.mapPixelToCoords(mousePixelScreenPosition);
    // convert screen-space → isometric world → tile coords
    const sf::Vector2f tempPos = camera.screen_to_world(mouseScreenPosition.x, mouseScreenPosition.y, 0); //getPointTileCoordinates(mouseScreenPosition);
    m_mouseWorldPosition = {static_cast<int>(std::round(tempPos.x)), static_cast<int>(std::round(tempPos.y))};

    if (selectionMode == SelectionMode::TILE_CORNER) {
        getSelectedTilesCorners(camera, worldModel, m_mouseWorldPosition, sf::Vector2f(mouseScreenPosition));
    } else {
        getSelectedTiles(camera, worldModel, m_mouseWorldPosition, sf::Vector2f(mouseScreenPosition));
        std::unordered_map<TileCorner*, float> uniqueCorners;
        for (const auto&[tile, weight] : m_brushSelectionTiles) {
            for (const TileCorner* corner : tile->getCorners()) {
                auto currentCorner = const_cast<TileCorner*>(corner);
                if (uniqueCorners.find(currentCorner) == uniqueCorners.end() || uniqueCorners[currentCorner] < weight)
                    uniqueCorners[currentCorner] = weight;
            }
        }
        for (const auto&[corner, weight] : uniqueCorners)
            m_brushSelectionTilesAsTileCorners.push_back(BrushTileCornerHit{corner, weight});
    }
    fillHoveredSelection(worldModel, selectionMode);
}

void BrushController::getSelectedTilesCorners(const Camera &camera, WorldModel &worldModel, const sf::Vector2i mouseWorldPosition, const sf::Vector2f mouseScreenPosition)
{
    const int searchRadius = getSearchRadius(camera, worldModel);
    const TileCorner* closestCorner = getClosestNeighborCornerInRadius(camera, worldModel, mouseWorldPosition, mouseScreenPosition, searchRadius);
    if (closestCorner == nullptr)
        return;
    m_brushCenterWorldPosition = closestCorner->getPosition();
    const std::vector<TileCorner*> neighbors = getPointNeighborsInRadius(worldModel, closestCorner->getColumn(), closestCorner->getRow(), m_brushSize);
    for (TileCorner* neighbor : neighbors)
        m_brushSelectionTileCorners.push_back(BrushTileCornerHit{neighbor, getPointWeightInBrush(neighbor->getPosition(), m_brushCenterWorldPosition)});
}

void BrushController::getSelectedTiles(const Camera &camera, WorldModel &worldModel, const sf::Vector2i mouseWorldPosition, const sf::Vector2f mouseScreenPosition)
{
    const int searchRadius = getSearchRadius(camera, worldModel);
    Tile *hoveredTile = getSelectedTileInRadius(camera, worldModel, mouseWorldPosition, mouseScreenPosition, searchRadius);
    if (hoveredTile == nullptr)
        return;
    m_brushCenterWorldPosition = hoveredTile->getCenterGridPosition();
    const std::vector<Tile *> neighbors = getClosestTilesInRadius(worldModel, hoveredTile->getGridPosition().x, hoveredTile->getGridPosition().y, m_brushSize, true);
    for (Tile* neighbor : neighbors)
        m_brushSelectionTiles.push_back(BrushTileHit{neighbor, getPointWeightInBrush(neighbor->getCenterGridPosition(), m_brushCenterWorldPosition)});
}

void BrushController::fillHoveredSelection(WorldModel &worldModel, SelectionMode selectionMode)
{
    m_hoveredTileCorners.clear();
    m_hoveredTiles.clear();
    m_tilesToHilight.clear();

    if (m_brushSelectionTileCorners.empty() && m_brushSelectionTiles.empty())
        return;

    int viewRadius = static_cast<int>(std::ceil(m_brushSize)) + 1; 
    int centerCol = static_cast<int>(std::round(m_brushCenterWorldPosition.x));
    int centerRow = static_cast<int>(std::round(m_brushCenterWorldPosition.y));
    m_tilesToHilight = getClosestTilesInRadius(worldModel, centerCol, centerRow, (viewRadius), true);
}

int BrushController::getSearchRadius(const Camera &camera, const WorldModel &worldModel) const
{
    const float maxAbsZ = worldModel.getHighestTileCornerHeight();
    const sf::Vector2f flatPos = camera.screen_to_world(0.0f, 0.0f, 0.0f);
    const sf::Vector2f highPos = camera.screen_to_world(0.0f, 0.0f, maxAbsZ);

    const float gridDistance = MathUtils::distanceBetweenPoints(flatPos, highPos);
    const int optimalRadius = static_cast<int>(std::ceil(gridDistance));
    constexpr int minRadius = 2;

    return (minRadius > optimalRadius) ? minRadius : optimalRadius;
}

TileCorner *BrushController::getClosestNeighborCornerInRadius(const Camera &camera, WorldModel &worldModel, const sf::Vector2i pointWorldPosition,
                                                                  const sf::Vector2f pointScreenPosition, const int radius) const
{
    const std::vector<TileCorner*> neighbors = getPointNeighborsInRadius(worldModel, pointWorldPosition.x, pointWorldPosition.y, radius);
    if (neighbors.empty())
        return nullptr;
    TileCorner* closestNeighbor = neighbors[0];
    float minDistance = MathUtils::distanceBetweenPoints(getTileCornerScreenCoordinates(camera, neighbors[0]), pointScreenPosition);
    const float refMinDistance = std::max(camera.getTileSizeX(), camera.getTileSizeY());

    for ( TileCorner* neighbor : neighbors) {
        if (const float dist = MathUtils::distanceBetweenPoints(getTileCornerScreenCoordinates(camera, neighbor), pointScreenPosition); dist < minDistance)
        {
            minDistance = dist;
            closestNeighbor = neighbor;
        }
    }
    if (refMinDistance < minDistance)
        return nullptr;
    return closestNeighbor;
}

std::vector<TileCorner *> BrushController::getPointNeighborsInRadius(WorldModel &worldModel, int x, int y, const int radius ) const
{
    std::vector<TileCorner*> neighbors;
    const std::vector<std::vector<std::unique_ptr<TileCorner>>>& map = worldModel.getCorners();
    // prevent overflow when mouse is outside the screen
    x = std::clamp(x, 0, static_cast<int>(map[0].size() - 1));
    y = std::clamp(y, 0, static_cast<int>(map.size() - 1));

    int startX = std::max(0, x - radius);
    int endX = std::min(static_cast<int>(map[0].size() - 1), x + radius);
    int startY = std::max(0, y - radius);
    int endY = std::min(static_cast<int>(map.size() - 1), y + radius);

    for (int j = startY; j <= endY; j++)
        for (int i = startX; i <= endX; i++)
            neighbors.push_back(map[j][i].get());
    return neighbors;
}

Tile *BrushController::getSelectedTileInRadius(const Camera &camera, WorldModel &worldModel, const sf::Vector2i pointWorldPosition, const sf::Vector2f pointScreenPosition, const int radius) const
{
    //pointScreenPosition = camera.screen_to_world(pointScreenPosition.x, pointScreenPosition.y, 0);
    std::vector<std::vector<Tile>>& tilesMap = worldModel.getTiles();
    if (pointWorldPosition.x >= 0 && pointWorldPosition.x < tilesMap[0].size()
        && pointWorldPosition.y >= 0 && pointWorldPosition.y < tilesMap.size()
        && isPointInsideTile(camera, &tilesMap[pointWorldPosition.y][pointWorldPosition.x], pointScreenPosition))
        return &tilesMap[pointWorldPosition.y][pointWorldPosition.x];
    for (int searchRadius = 1; searchRadius <= radius; searchRadius++) {
        const std::vector<Tile *> &tilesInRadius =  getClosestTilesInRadius(worldModel,
            pointWorldPosition.x, pointWorldPosition.y, searchRadius);
        for (  Tile *tile : tilesInRadius)
            if (isPointInsideTile(camera, tile, pointScreenPosition))
                return tile;

    }
    return nullptr;
}

std::vector<Tile *> BrushController::getClosestTilesInRadius(WorldModel &worldModel, int x, int y, const int radius, const bool includeInside) const
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

    if (includeInside) {
        for (int j = startY; j <= endY; j++)
            for (int i = startX; i <= endX; i++)
                tiles.push_back(&tilesMap[j][i]);
    } else {
        for (int i = startX; i <= endX; i++) {
            tiles.push_back(&tilesMap[startY][i]);
            tiles.push_back(&tilesMap[endY][i]);
        }
        for (int j = startY + 1; j <= endY - 1; j++) {
            tiles.push_back(&tilesMap[j][startX]);
            tiles.push_back(&tilesMap[j][endX]);
        }
    }
    return tiles;
}

bool BrushController::isPointInsideTile(const Camera &camera, Tile *tile, sf::Vector2f pointScreenPosition) const
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

sf::Vector2f BrushController::getTileCornerScreenCoordinates(const Camera &camera, const TileCorner *corner) const
{
    return camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
}

float BrushController::getPointWeightInBrush(const sf::Vector2f &pointWorldPosition, const sf::Vector2f &brushCenterWorldPosition) const
{
    const float brushRadius = static_cast<float>(m_brushSize);
    
    // to remove ??
    if (brushRadius <= 0.0f)
        return 1.0f;
    
    const float brushTextureWidth = static_cast<float>(m_brushesImages[m_currentBrushImage].getSize().x);
    const float brushTextureHeight = static_cast<float>(m_brushesImages[m_currentBrushImage].getSize().y);

    const float boundingBoxWidth = (2.0f * brushRadius) + 1.0f;

    const float minX = brushCenterWorldPosition.x - (boundingBoxWidth / 2.0f);
    const float minY = brushCenterWorldPosition.y - (boundingBoxWidth / 2.0f);

    const float u = (pointWorldPosition.x - minX) / boundingBoxWidth;
    const float v = (pointWorldPosition.y - minY) / boundingBoxWidth;

    if (u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f)
        return 0.0f; // Outside the brush area
    
    float pixelX = u * brushTextureWidth;
    float pixelY = v * brushTextureHeight;
    
    pixelX = std::clamp(pixelX, 0.0f, brushTextureWidth - 1.0f);
    pixelY = std::clamp(pixelY, 0.0f, brushTextureHeight - 1.0f);
    
    const sf::Color pixelColor = m_brushesImages[m_currentBrushImage].getPixel(static_cast<unsigned int>(pixelX), static_cast<unsigned int>(pixelY));
    float finalWeight = (pixelColor.r / 255.0f) * (pixelColor.a / 255.0f);
    
    return finalWeight;
}

void BrushController::selectBrush(const int index)
{
    if (m_currentBrushImage == index || index >= static_cast<int>(m_brushesImages.size()))
        return;
    if (m_currentBrushImage != - 1)
        m_brushMenu->unselectBrush(m_currentBrushImage);
    m_currentBrushImage = index;
    m_brushMenu->selectBrush(m_currentBrushImage);
}

void BrushController::incrementBrushSize()
{
    m_brushSize += 1.0f;
    if (m_brushSize > m_brushSizeMax)
        m_brushSize = m_brushSizeMax;
    m_brushMenu->setBrushSizeValueText(getBrushSizeValue());
}

void BrushController::decrementBrushSize()
{
    m_brushSize -= 1.0f;
    if (m_brushSize <= m_brushSizeMin)
        m_brushSize = m_brushSizeMin;
    m_brushMenu->setBrushSizeValueText(getBrushSizeValue());
}

std::string BrushController::getBrushSizeValue() const
{
    std::string value = std::to_string(static_cast<int>(m_brushSize));
    return (value.size() > 1) ? value : "0" + value;
}