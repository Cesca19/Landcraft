//
// Created by fran on 17/03/2026.
//

#include "SetTilesCornersHeightCommand.hpp"

SetTilesCornersHeightCommand::SetTilesCornersHeightCommand(const std::vector<TileCorner *> corners, const int heightStep)
    : m_corners(corners)
    , m_heightStep(heightStep)
{
}

void SetTilesCornersHeightCommand::execute(WorldModel &model, WorldView &view)
{
    for (TileCorner * corner: m_corners)
        corner->addHeight(m_heightStep);
    view.updateTileCorners(model.getTiles(), m_corners);
}

void SetTilesCornersHeightCommand::undo(WorldModel &model, WorldView &view)
{
    for (TileCorner * corner: m_corners)
        corner->addHeight(-m_heightStep);
    view.updateTileCorners(model.getTiles(), m_corners);
}

std::string SetTilesCornersHeightCommand::getName()
{ 
    return "Set Tiles Corners Height"; 
}