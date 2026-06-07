//
// Created by fran on 03/06/2026.
//

#include "SetTerrainHeightMapCommand.hpp"

SetTerrainHeightMapCommand::SetTerrainHeightMapCommand(const std::vector<std::vector<float>> &heightmap)
    : m_heightmap(heightmap)
{
}

void SetTerrainHeightMapCommand::execute(WorldModel &model, WorldView &view)
{
    const std::vector<std::vector<std::unique_ptr<TileCorner>>>& corners = model.getCorners();
    
    m_previousHeightmap.resize(corners.size());
    for (size_t row = 0; row < corners.size(); row++) {
        m_previousHeightmap[row].resize(corners[row].size());
        for (size_t col = 0; col < corners[row].size(); col++) {
            m_previousHeightmap[row][col] = corners[row][col]->getHeight();
        }
    }
    applyHeightmapToModel(model, view, m_heightmap);
}

void SetTerrainHeightMapCommand::undo(WorldModel &model, WorldView &view)
{
    applyHeightmapToModel(model, view, m_previousHeightmap);
}

std::string SetTerrainHeightMapCommand::getName()
{
    return "Set Terrain Height Map";
}

void SetTerrainHeightMapCommand::applyHeightmapToModel(WorldModel &model, const WorldView &view, const std::vector<std::vector<float>> &heightmap)
{
    const std::vector<std::vector<std::unique_ptr<TileCorner>>>& corners = model.getCorners();
    for (size_t row = 0; row < corners.size(); row++) {
        for (size_t col = 0; col < corners[row].size(); col++) {
            corners[row][col]->setHeight(heightmap[row][col]);
        }
    }
    model.onTileCornerHeightChanged();
    view.updatePositions(model.getTiles(), view.getCamera());
}
