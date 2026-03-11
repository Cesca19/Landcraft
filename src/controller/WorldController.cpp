//
// Created by fran on 11/03/2026.
//

#include "WorldController.hpp"

WorldController::WorldController()
{
}

void WorldController::init(const std::string &mapName)
{
    m_worldModel.loadMap(mapName);
    std::vector<std::vector<Tile>>& tiles = m_worldModel.getTiles();
    // iterate through the tiles triangle and for each of them get their coordinates
    // send thoses data to the world view to create the vertex array
}
