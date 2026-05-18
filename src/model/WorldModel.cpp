//
// Created by fran on 11/03/2026.
//

#include <stdexcept>
#include "WorldModel.hpp"

WorldModel::WorldModel()
    : m_highestTileCornerHeight(0)
    , m_tilesSize({-1, -1})
{
}

WorldModel::~WorldModel()
{
}

void WorldModel::loadMap(std::string mapName)
{
    std::unique_ptr<WorldMap> worldMap = loadMapFromFile(mapName);
    m_tileCornersHeightmap.clear();
    m_tileTextureIdMap.clear();
    m_tileCornersHeightmap = std::move(worldMap->TileCornersHeightMap);
    m_tileTextureIdMap = std::move(worldMap->TileTextureIdMap);
    m_tilesSize = worldMap->TilesSize;
    m_splatmapFilepath = worldMap->splatmapFilepath;
    worldMap = nullptr;
    createWorldTileCorners();
    createWorldTiles();
    onTileCornerHeightChanged();
}

sf::Vector2i WorldModel::getTilesSize() const
{
    return m_tilesSize;
}

std::string WorldModel::getSplatmapFilepath() const
{
    return m_splatmapFilepath;
}

sf::Vector2i WorldModel::getMapSize() const
{
    return sf::Vector2i(m_corners.empty() ? 0 : m_corners[0].size() - 1, m_corners.size() - 1);
}

void WorldModel::saveMapToFile(std::string mapName)
{
        std::ofstream mapFile(mapName);
        if (!mapFile.is_open()) {
            throw std::runtime_error("Failed to open map file for writing: " + mapName);
        }
        // Write map size
        mapFile << "[MAP_SIZE]\n";
        mapFile << m_tileTextureIdMap[0].size() << " " << m_tileTextureIdMap.size() << "\n\n";
    
        // Write tile corners heightmap
        mapFile << "[TILES_CORNERS]\n";
        for (const auto & row : m_corners) {
            for (const auto & corner : row) {
                mapFile << corner->getHeight() << " ";
            }
            mapFile << "\n";
        }
        mapFile << "\n";
    
        // Write tile texture ID map
        mapFile << "[TILES]\n";
        for (const auto & row : m_tiles) {
            for (const auto & tile : row) {
                mapFile << tile.getTextureId() << " ";
            }
            mapFile << "\n";
        }
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

std::unique_ptr<WorldMap> WorldModel::loadMapFromFile(std::string mapName)
{
    std::ifstream mapFile(mapName);
    if (!mapFile.is_open())
        throw std::runtime_error("Failed to open map file: " + mapName);

    sf::Vector2i tilesSize = loadTilesSize(mapFile);
    if (tilesSize.x <= 0 || tilesSize.y <= 0)
        throw std::runtime_error("Invalid tiles size in file: " + mapName);

    std::string splatmapFilepath = loadSplatmapFilepath(mapFile);
    if (splatmapFilepath.empty())
         throw std::runtime_error("Failed to load splatmap filepath from file: " + mapName);
    
    sf::Vector2i mapSize = loadMapSize(mapFile);
    if (mapSize.x <= 0 || mapSize.y <= 0) {
        throw std::runtime_error("Invalid map size in file: " + mapName);
    }
    int nbCols = mapSize.x;
    int nbRows = mapSize.y;

    std::vector<std::vector<float>> heightmap = loadTileCornersHeightmap(mapFile, nbRows, nbCols);
    if (heightmap.empty()) {
        throw std::runtime_error("Failed to load tile corners heightmap from file: " + mapName);
    }

    std::vector<std::vector<int>> textureIdMap = loadTileTextureIdMap(mapFile, nbRows, nbCols);
    if (textureIdMap.empty()) {
        throw std::runtime_error("Failed to load tile texture ID map from file: " + mapName);
    }

    std::cout << "Map file loaded successfully: " << mapName << std::endl;
    std::unique_ptr<WorldMap> worldMap = std::make_unique<WorldMap>();
    worldMap->TileCornersHeightMap = heightmap;
    worldMap->TileTextureIdMap = textureIdMap;
    worldMap->TilesSize = tilesSize;
    worldMap->splatmapFilepath = splatmapFilepath;
    return worldMap;
}

sf::Vector2i WorldModel::loadTilesSize(std::ifstream &mapFile) const
{
    std::string line;
    sf::Vector2i defaultSize(-1, -1);
    while (std::getline(mapFile, line)) {
        if (line.empty())
            continue;
        if (line.find("[TILES_SIZE]") != std::string::npos) {
            break;
        } else
            return defaultSize;
    }
    if (std::getline(mapFile, line)) {
        std::istringstream iss(line);
        int width, height;
        if (iss >> width >> height) {
            return sf::Vector2i(width, height);
        }
    }
    return defaultSize;
}

std::string WorldModel::loadSplatmapFilepath(std::ifstream &mapFile) const
{
    std::string line;
    while (std::getline(mapFile, line)) {
        if (line.empty())
            continue;
        if (line.find("[SPLATMAP_FILEPATH]") != std::string::npos)
            break;
        else
            return "";
    }
    if (std::getline(mapFile, line))
        return line;
    return "";
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

std::vector<std::vector<float>> WorldModel::loadTileCornersHeightmap(std::ifstream &mapFile, int nb_rows, int nb_cols) const
{
    std::vector<std::vector<float>> heightmap;
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
        std::vector<float> row;
        std::istringstream iss(line);
        float height;
        while (iss >> height)
            row.push_back(height);
        if (row.size() != static_cast<size_t>(nb_cols) + 1)
            return std::vector<std::vector<float>>(); // return empty heightmap if row size is incorrect
        heightmap.push_back(row);
    }
    if (heightmap.size() != static_cast<size_t>(nb_rows) + 1)
        return std::vector<std::vector<float>>(); // return empty heightmap if number of rows is incorrect
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
                row, col, m_tileCornersHeightmap[row][col]
            );
            rowCorners.push_back(std::move(tileCorner));
        }
        m_corners.push_back(std::move(rowCorners));
    }
}
