//
// Created by fran on 13/03/2026.
//

#include "BrushController.hpp"

BrushController::BrushController(const sf::Vector2f uiStartPosition)
    : m_brushView(std::make_unique<BrushView>())
    , m_brushMenu(nullptr)
    , m_mouseWorldPosition(-1, -1)
    , m_brushSize(2)
    , m_brushSizeMin(0)
    , m_brushSizeMax(20)
    , m_currentBrushImage(-1)
{
    m_brushesImagePaths = {
        "assets/textures/brushes/Carre_Plein.png",
        "assets/textures/brushes/Hexagone.png",
        "assets/textures/brushes/Cercle_Dur.png",
        "assets/textures/brushes/Cercle_Flou_Grand.png",
        "assets/textures/brushes/Cercle_Flou_Petit.png",
        "assets/textures/brushes/Brosse_Bruit.png",
        "assets/textures/brushes/Etoile.png",
        "assets/textures/brushes/Etoile_Trouee.png",
    };

    m_brushMenu = std::make_unique<BrushMenu>(uiStartPosition, m_brushesImagePaths);
    m_brushMenu->setIncrementBrushSizeButtonCallback([this]() { incrementBrushSize(); });
    m_brushMenu->setDecrementBrushSizeButtonCallback([this]() { decrementBrushSize(); });
    m_brushMenu->setBrushSizeValueText(getBrushSizeValue());

    for (int i = 0; i < m_brushesImagePaths.size(); i++) {
        m_brushesImages.push_back(ResourceManager::getInstance().getImage(m_brushesImagePaths[i]));
        BrushUtils::sanitizeBrushImage(m_brushesImages.back());
        m_brushMenu->setBrushTypeButtonCallback(i, [this, i] () { selectBrush(i); });
    }
    selectBrush(0);
}

void BrushController::handleEvents(const sf::RenderWindow &window, const sf::Event &event)
{
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard:: Add:
                incrementBrushSize();
            break;
        case sf::Keyboard::Subtract:
                decrementBrushSize();
            break;
        case sf::Keyboard::A:
                selectBrush((m_currentBrushImage + 1) % static_cast<int>(m_brushesImages.size()));
            break;
            case sf::Keyboard::E:
                selectBrush((m_currentBrushImage - 1 + static_cast<int>(m_brushesImages.size())) % static_cast<int>(m_brushesImages.size()));
            break;
        default:
            break;
        }
    }
}

void BrushController::update(const float deltaTime, const sf::RenderWindow &window, const SelectionMode selectionMode,
                                WorldModel &worldModel, const Camera &camera)
{
    getSelectedCorners(window, camera, worldModel, selectionMode);
    // should we hide the mouse cursor when something is hovered inside the map
}

void BrushController::draw(sf::RenderWindow &window, const Camera &camera, bool areEditableTilesVisible) const
{
    if (!m_tilesToHilight.empty())
        m_brushView->drawBrushOverlay(window, m_tilesToHilight, camera, m_brushCenterWorldPosition, static_cast<float>(m_brushSize), m_brushesImages[m_currentBrushImage]);
    if (!m_brushSelectionTileCorners.empty())
        m_brushView->drawTileCorners(window, m_brushSelectionTileCorners, camera);
    if (!m_brushSelectionTiles.empty() && areEditableTilesVisible) {
        // m_brushView->drawTiles(window, m_brushSelectionTiles, camera);
        m_brushView->drawMaskedSelectedTiles(window, m_brushSelectionTiles, camera, m_brushCenterWorldPosition, static_cast<float>(m_brushSize), m_brushesImages[m_currentBrushImage]);
    }
}

sf::Vector2i BrushController::getMouseWorldPosition() const
{
    return m_mouseWorldPosition;
}

sf::Vector2f BrushController::getBrushCenterWorldPosition() const
{
    return m_brushCenterWorldPosition;
}

bool BrushController::isAnyTileCornerSelected() const
{
    return !m_brushSelectionTileCorners.empty() || !m_brushSelectionTiles.empty();
}

std::vector<std::string> BrushController::getBrushesImagePaths() const
{
    return m_brushesImagePaths;
}

const std::vector<BrushTileCornerHit> &BrushController::getBrushTileCornersSelection() const
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
    std::vector<BrushTileCornerHit> selection;

    for (const Tile *tile : tiles) {
        const float weight = getPointWeightInBrush(tile->getCenterGridPosition(), sf::Vector2f(x, y));
        for (TileCorner *corner : tile->getCorners())
            if (uniqueCorners.find(corner) == uniqueCorners.end() || uniqueCorners[corner] < weight)
                uniqueCorners[corner] = weight;
    }
    for (const auto&[corner, weight] : uniqueCorners)
        selection.push_back(BrushTileCornerHit{corner, weight});
    return selection;
}

int BrushController::getCurrentBrushId() const
{
    return m_currentBrushImage;
}

int BrushController::getCurrentBrushRadius() const
{
    return m_brushSize;
}

void BrushController::setVisibility(bool visible) const
{
    m_brushMenu->setVisibility(visible);
}

void BrushController::getSelectedCorners(const sf::RenderWindow &window, const Camera &camera, WorldModel &worldModel, const SelectionMode selectionMode)
{
    // get the current mouse position in the window in pixels
    const sf::Vector2i mousePixelScreenPosition = sf::Mouse::getPosition(window);
    // get it's real coordinates in the current view
    const sf::Vector2f mouseScreenPosition = window.mapPixelToCoords(mousePixelScreenPosition);
    // convert screen-space ->  isometric world ->  tile coords
    const sf::Vector2f tempPos = camera.screen_to_world(mouseScreenPosition.x, mouseScreenPosition.y, 0); //getPointTileCoordinates(mouseScreenPosition);

    m_brushSelectionTiles.clear();
    m_brushSelectionTileCorners.clear();
    m_brushSelectionTilesAsTileCorners.clear();
    m_mouseWorldPosition = {static_cast<int>(std::round(tempPos.x)), static_cast<int>(std::round(tempPos.y))};
    if (selectionMode == SelectionMode::TILE_CORNER)
        getSelectedTilesCorners(camera, worldModel, m_mouseWorldPosition, sf::Vector2f(mouseScreenPosition));
    else {
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
    m_mouseWorldPosition = sf::Vector2i(static_cast<int>(closestCorner->getColumn()),
         static_cast<int>(closestCorner->getRow()));
    m_brushCenterWorldPosition = closestCorner->getPosition();
    const std::vector<TileCorner*> neighbors = getPointNeighborsInRadius(worldModel, static_cast<int>(closestCorner->getColumn()), static_cast<int>(closestCorner->getRow()), m_brushSize);
    for (TileCorner* neighbor : neighbors)
        m_brushSelectionTileCorners.push_back(BrushTileCornerHit{neighbor, getPointWeightInBrush(neighbor->getPosition(), m_brushCenterWorldPosition)});
}

void BrushController::getSelectedTiles(const Camera &camera, WorldModel &worldModel, const sf::Vector2i mouseWorldPosition, const sf::Vector2f mouseScreenPosition)
{
    const int searchRadius = getSearchRadius(camera, worldModel);
    const Tile *hoveredTile = getSelectedTileInRadius(camera, worldModel, mouseWorldPosition, mouseScreenPosition, searchRadius);

    if (hoveredTile == nullptr)
        return;
    m_mouseWorldPosition = hoveredTile->getGridPosition();
    
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
    const int viewRadius = static_cast<int>(std::ceil(m_brushSize)) + 1;
    const int centerCol = static_cast<int>(std::round(m_brushCenterWorldPosition.x));
    const int centerRow = static_cast<int>(std::round(m_brushCenterWorldPosition.y));
    m_tilesToHilight = getClosestTilesInRadius(worldModel, centerCol, centerRow, (viewRadius), true);
}


int BrushController::getSearchRadius(const Camera &camera, const WorldModel &worldModel)
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
                                                                  const sf::Vector2f pointScreenPosition, const int radius)
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

std::vector<TileCorner *> BrushController::getPointNeighborsInRadius(WorldModel &worldModel, int x, int y, const int radius )
{
    std::vector<TileCorner*> neighbors;
    const std::vector<std::vector<std::unique_ptr<TileCorner>>>& map = worldModel.getCorners();
    // prevent overflow when mouse is outside the screen
    x = std::clamp(x, 0, static_cast<int>(map[0].size() - 1));
    y = std::clamp(y, 0, static_cast<int>(map.size() - 1));
    const int startX = std::max(0, x - radius);
    const int endX = std::min(static_cast<int>(map[0].size() - 1), x + radius);
    const int startY = std::max(0, y - radius);
    const int endY = std::min(static_cast<int>(map.size() - 1), y + radius);

    for (int j = startY; j <= endY; j++)
        for (int i = startX; i <= endX; i++)
            neighbors.push_back(map[j][i].get());
    return neighbors;
}

Tile *BrushController::getSelectedTileInRadius(const Camera &camera, WorldModel &worldModel, const sf::Vector2i pointWorldPosition, const sf::Vector2f pointScreenPosition, const int radius) const
{
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


std::vector<Tile *> BrushController::getClosestTilesInRadius(WorldModel &worldModel, int x, int y, const int radius, const bool includeInside)
{
    std::vector<Tile *> tiles;
    std::vector<std::vector<Tile>>& tilesMap = worldModel.getTiles();

    // prevent overflow when mouse is outside the screen
    x = std::clamp(x, 0, static_cast<int>(tilesMap[0].size()) - 1);
    y = std::clamp(y, 0, static_cast<int>(tilesMap.size()) - 1);

    const int startX = std::max(0, x - radius);
    const int endX = std::min(static_cast<int>(tilesMap[0].size()) -1, x + radius);
    const int startY = std::max(0, y - radius);
    const int endY = std::min(static_cast<int>(tilesMap.size()) -1, y + radius);

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


bool BrushController::isPointInsideTile(const Camera &camera, const Tile *tile, const sf::Vector2f pointScreenPosition)
{
    const std::vector<TileCorner*> &tileCorners = tile->getCorners();
    if (tileCorners.size() != 4)
        return false;
    const sf::Vector2f corner0ScreenPos = getTileCornerScreenCoordinates(camera, tileCorners[0]);
    const sf::Vector2f corner1ScreenPos = getTileCornerScreenCoordinates(camera, tileCorners[1]);
    const sf::Vector2f corner2ScreenPos = getTileCornerScreenCoordinates(camera, tileCorners[2]);
    const sf::Vector2f corner3ScreenPos = getTileCornerScreenCoordinates(camera, tileCorners[3]);


    if (MathUtils::isInsideTriangle(pointScreenPosition, corner0ScreenPos, corner1ScreenPos, corner2ScreenPos)
        || MathUtils::isInsideTriangle(pointScreenPosition, corner2ScreenPos, corner3ScreenPos, corner0ScreenPos))
        return true;
    return false;
}

sf::Vector2f BrushController::getTileCornerScreenCoordinates(const Camera &camera, const TileCorner *corner)
{
    return camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
}


float BrushController::getPointWeightInBrush(const sf::Vector2f &pointWorldPosition, const sf::Vector2f &brushCenterWorldPosition) const
{
    const auto brushRadius = static_cast<float>(m_brushSize);

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
    const float finalWeight = (static_cast<float>(pixelColor.r) / 255) * (static_cast<float>(pixelColor.a) / 255);

    return finalWeight;
}

// void BrushController::sanitizeBrushImage(sf::Image &img)
// {
//     const sf::Vector2u size = img.getSize();
//     if (size.x == 0 || size.y == 0) return;

//     bool hasMeaningfulAlpha = false;
//     for (unsigned int y = 0; y < size.y && !hasMeaningfulAlpha; ++y) {
//         for (unsigned int x = 0; x < size.x; ++x) {
//             const sf::Uint8 a = img.getPixel(x, y).a;
//             if (a != 255) { // any non-opaque pixel => alpha is being used
//                 hasMeaningfulAlpha = true;
//                 break;
//             }
//         }
//     }
//     if (hasMeaningfulAlpha) {
//         // Force RGB to white for all pixels so the shader can tint it correctly without dark artifacts.
//         for (unsigned int y = 0; y < size.y; ++y) {
//             for (unsigned int x = 0; x < size.x; ++x) {
//                 sf::Color c = img.getPixel(x, y);
//                 c.r = 255;
//                 c.g = 255;
//                 c.b = 255;
//                 img.setPixel(x, y, c);
//             }
//         }
//         return;
//     }

//     auto lumAt = [&](unsigned int x, unsigned int y) -> float {
//         const sf::Color c = img.getPixel(x, y);
//         return (0.299f * c.r + 0.587f * c.g + 0.114f * c.b) / 255.f;
//     };
//     const unsigned int maxX = size.x - 1;
//     const unsigned int maxY = size.y - 1;
//     const float bgLum = (lumAt(0, 0) + lumAt(maxX, 0) + lumAt(0, maxY) + lumAt(maxX, maxY)) * 0.25f;
//     // If corners are dark: background is dark. If corners are gray: background is gray.
//     const float bgCut = std::clamp(bgLum, 0.02f, 0.90f);
//     const float denom = std::max(1e-5f, 1.0f - bgCut);

//     for (unsigned int y = 0; y < size.y; ++y) {
//         for (unsigned int x = 0; x < size.x; ++x) {
//             constexpr float blackCut = 0.02f;
//             const float lum = lumAt(x, y);
//             float keyed = (lum - std::max(blackCut, bgCut)) / denom;
//             keyed = std::clamp(keyed, 0.0f, 1.0f);
//             // Steepen transition: keep background really transparent.
//             keyed = keyed * keyed;
//             const auto outA = static_cast<sf::Uint8>(std::round(keyed * 255.f));

//             // Force RGB to white so only the vertex color drives the tint,
//             // and to avoid dark halos when filtering.
//             img.setPixel(x, y, sf::Color(255, 255, 255, outA));
//         }
//     }
// }

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
    std::string value = std::to_string(m_brushSize);
    return (value.size() > 1) ? value : "0" + value;
}