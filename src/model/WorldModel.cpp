//
// Created by fran on 11/03/2026.
//

#include "WorldModel.hpp"

WorldModel::WorldModel()
    : m_highestTileCornerHeight(0)
{
}

WorldModel::~WorldModel()
{
}

void WorldModel::loadMap(std::string mapName, bool quitOnFailure)
{
    m_tileCornersHeightmap.clear();
    m_tileTextureIdMap.clear();
    std::unique_ptr<WorldMap> worldMap = loadMapFromFile(mapName, quitOnFailure);
    if (!worldMap) {
        if (quitOnFailure)
            throw std::runtime_error("Failed to load world map from file: " + mapName);
        return;
    }
    m_tileCornersHeightmap = std::move(worldMap->TileCornersHeightMap);
    m_tileTextureIdMap = std::move(worldMap->TileTextureIdMap);
    worldMap = nullptr;
    createWorldTileCorners();
    createWorldTiles();
    onTileCornerHeightChanged();
}

std::vector<std::vector<Tile>> &WorldModel::getTiles()
{
    return m_tiles;
}

std::vector<std::vector<std::unique_ptr<TileCorner>>> &WorldModel::getCorners()
{
    return m_corners;
}

sf::Vector2f WorldModel::getCenter() const
{
    const float centerX = (static_cast<float>(m_tileCornersHeightmap[0].size()) - 1.0f) / 2.0f;
    const float centerY = (static_cast<float>(m_tileCornersHeightmap.size()) - 1.0f) / 2.0f;

    return {centerX, centerY};
}

void WorldModel::onTileCornerHeightChanged()
{
    m_highestTileCornerHeight = 0;
    if (m_corners.empty())
        return;
    for (auto & corners_list : m_corners) {
        for (const auto & corner : corners_list) {
            const float height = std::abs(corner.get()->getHeight());
            if (height > m_highestTileCornerHeight)
                m_highestTileCornerHeight = height;
        }
    }
}

void WorldModel::onTileCornerHeightChanged(const float height)
{
    if (std::abs(height) > m_highestTileCornerHeight)
        m_highestTileCornerHeight = std::abs(height);
}

float WorldModel::getHighestTileCornerHeight() const
{
    return m_highestTileCornerHeight;
}

std::unique_ptr<WorldMap> WorldModel::loadMapFromFile(std::string mapName, bool quitOnFailure)
{
    std::ifstream mapFile(mapName);
    if (!mapFile.is_open()) {
        if (quitOnFailure)
            throw std::runtime_error("Failed to open map file: " + mapName);
        return nullptr;
    }
    std::cout << "Map file opened successfully: " << mapName << std::endl;

    sf::Vector2i mapSize = loadMapSize(mapFile);
    if (mapSize.x <= 0 || mapSize.y <= 0) {
        if (quitOnFailure)
            throw std::runtime_error("Invalid map size in file: " + mapName);
        return nullptr;
    }
    int nbCols = mapSize.x;
    int nbRows = mapSize.y;
    std::cout << "Map size: " << nbCols << " cols, " << nbRows << " rows" << std::endl;

    std::vector<std::vector<int>> heightmap = loadTileCornersHeightmap(mapFile, nbRows, nbCols);
    if (heightmap.empty()) {
        if (quitOnFailure)
            throw std::runtime_error("Failed to load tile corners heightmap from file: " + mapName);
        return nullptr;
    }
    std::cout << "Tile corners heightmap loaded successfully" << std::endl;

    std::vector<std::vector<int>> textureIdMap = loadTileTextureIdMap(mapFile, nbRows, nbCols);
    if (textureIdMap.empty()) {
        if (quitOnFailure)
            throw std::runtime_error("Failed to load tile texture ID map from file: " + mapName);
        return nullptr;
    }
    std::cout << "Tile texture ID map loaded successfully" << std::endl;

    std::unique_ptr<WorldMap> worldMap = std::make_unique<WorldMap>();
    worldMap->TileCornersHeightMap = heightmap;
    worldMap->TileTextureIdMap = textureIdMap;
    return std::move(worldMap);
}

sf::Vector2i WorldModel::loadMapSize(std::ifstream &mapFile) const
{
    std::string line;
    while (std::getline(mapFile, line)) {
        if (line.empty())
            continue;
        if (line.find("[MAP_SIZE]") != std::string::npos) {
            break;
        } else 
            return sf::Vector2i(-1, -1);
    }
    if (std::getline(mapFile, line)) {
        std::istringstream iss(line);
        int width, height;
        if (iss >> width >> height) {
            return sf::Vector2i(width, height);
        }
    }
    return sf::Vector2i(-1, -1);
}

std::vector<std::vector<int>> WorldModel::loadTileCornersHeightmap(std::ifstream &mapFile, int nb_rows, int nb_cols) const
{
    std::vector<std::vector<int>> heightmap;
    std::string line;
    while (std::getline(mapFile, line)) {
        if (line.empty())
            continue;
        if (line.find("[TILES_CORNERS]") != std::string::npos)
            break;
        else
            return heightmap; // empty heightmap if section not found
    }
    while (std::getline(mapFile, line)) {
        if (line.empty())
            break;
        std::vector<int> row;
        std::istringstream iss(line);
        int height;
        while (iss >> height)
            row.push_back(height);
        if (row.size() != static_cast<size_t>(nb_cols) + 1)
            return std::vector<std::vector<int>>(); // return empty heightmap if row size is incorrect
        heightmap.push_back(row);
    }
    if (heightmap.size() != static_cast<size_t>(nb_rows) + 1)
        return std::vector<std::vector<int>>(); // return empty heightmap if number of rows is incorrect
    return heightmap;
}

std::vector<std::vector<int>> WorldModel::loadTileTextureIdMap(std::ifstream &mapFile, int nb_rows, int nb_cols) const
{
    std::vector<std::vector<int>> textureIdMap;
    std::string line;
    while (std::getline(mapFile, line)) {
        if (line.empty())
            continue;
        if (line.find("[TILES]") != std::string::npos)
            break;
        else
            return textureIdMap; // empty textureIdMap if section not found
    }
    while (std::getline(mapFile, line)) {
        if (line.empty())
            break;
        std::vector<int> row;
        std::istringstream iss(line);
        int textureId;
        while (iss >> textureId)
            row.push_back(textureId);
        if (row.size() != static_cast<size_t>(nb_cols))
            return std::vector<std::vector<int>>(); // return empty textureIdMap if row size is incorrect
        textureIdMap.push_back(row);
    }
    if (textureIdMap.size() != static_cast<size_t>(nb_rows))
        return std::vector<std::vector<int>>(); // return empty textureIdMap if number of rows is incorrect
    return textureIdMap;
}

void WorldModel::createWorldTiles()
{
    m_tiles.clear();
    for (int row = 0; row < m_corners.size(); row++)
        for (int col = 0; col < m_corners[row].size(); col++)
            createTileFromTileCorner(row, col);
}

void WorldModel::createTileFromTileCorner(const int row, const int col)
{
    if (m_corners.size() <= 1 || m_corners[0].size() <= 1
        || col < 0 || col + 1 >= m_corners[0].size()
        || row < 0 || row + 1 >= m_corners.size())
        return;
    std::vector<TileCorner *> tileCorners = {
        m_corners[row][col].get(),
        m_corners[row][col + 1].get(),
        m_corners[row + 1][col + 1].get(),
        m_corners[row + 1][col].get(),
    };
    if (m_tiles.size() < row + 1)
        m_tiles.emplace_back();
    m_tiles[row].emplace_back(tileCorners, m_tileTextureIdMap[row][col]);
}

void WorldModel::createWorldTileCorners()
{
    m_corners.clear();
    for (int row = 0; row < m_tileCornersHeightmap.size(); row++) {
        std::vector<std::unique_ptr<TileCorner>> rowCorners;
        for (int col = 0; col < m_tileCornersHeightmap[row].size(); col++) {
            std::unique_ptr<TileCorner> tileCorner = std::make_unique<TileCorner>(
                row, col, m_tileCornersHeightmap[row][col] /*, 0 // default white texture*/
            );
            rowCorners.push_back(std::move(tileCorner));
        }
        m_corners.push_back(std::move(rowCorners));
    }
}
