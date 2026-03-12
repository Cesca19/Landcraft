//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_WORLDMODEL_HPP
#define LANDCRAFT_WORLDMODEL_HPP

#include <string>
#include <vector>
#include <memory>
#include "Tile.hpp"

class WorldModel
{
public:
    WorldModel();
    ~WorldModel();
    void loadMap(std::string mapName);
    std::vector<std::vector<Tile>>& getTiles();
    sf::Vector2f getCenter() const;
private:
    void createWorldTiles();
    void createTileFromTileCorner(int row, int col);
    void createWorldTileCorners();

    std::vector<std::vector<int>> m_map;
    std::vector<std::vector<std::unique_ptr<TileCorner>>> m_corners;
    std::vector<std::vector<Tile>> m_tiles;
};


#endif //LANDCRAFT_WORLDMODEL_HPP