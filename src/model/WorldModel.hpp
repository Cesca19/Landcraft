//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_WORLDMODEL_HPP
#define LANDCRAFT_WORLDMODEL_HPP

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include "Tile.hpp"

struct WorldMap {
    sf::Vector2i TilesSize;
    std::string splatmapFilepath;
    std::vector<std::vector<float>> TileCornersHeightMap;
};

class WorldModel
{
public:
    WorldModel();
    ~WorldModel();
    void loadMap(std::string mapName);
    sf::Vector2i getTilesSize() const;
    std::string getSplatmapFilepath() const;
    sf::Vector2i getMapSize() const;
    void saveMapToFile(std::string mapName, std::string splatmapFileName);
    std::vector<std::vector<Tile>>& getTiles();
    std::vector<std::vector<std::unique_ptr<TileCorner>>>& getCorners();
    sf::Vector2f getCenter() const;
    void onTileCornerHeightChanged();
    void onTileCornerHeightChanged(float height);
    float getHighestTileCornerHeight() const;

    float getMinElevation() const;
    float getMaxElevation() const;
    float getWaterHeight() const;
private:
    std::unique_ptr<WorldMap> loadMapFromFile(std::string mapName);
    sf::Vector2i loadTilesSize(std::ifstream &mapFile) const;
    std::string loadSplatmapFilepath(std::ifstream &mapFile) const;
    sf::Vector2i loadMapSize(std::ifstream &mapFile) const;
    std::vector<std::vector<float>> loadTileCornersHeightmap(std::ifstream &mapFile, int nb_rows, int nb_cols) const;

    void createWorldTiles();
    void createTileFromTileCorner(int row, int col);
    void createWorldTileCorners();

    std::vector<std::vector<float>> m_tileCornersHeightmap;
    // std::vector<std::vector<int>> m_tileTextureIdMap;
    std::vector<std::vector<std::unique_ptr<TileCorner>>> m_corners;
    std::vector<std::vector<Tile>> m_tiles;
    float m_highestTileCornerHeight;

    sf::Vector2i m_tilesSize;
    std::string m_splatmapFilepath;

    float m_minElevation;
    float m_maxElevation;
    float m_waterHeight;
};


#endif //LANDCRAFT_WORLDMODEL_HPP