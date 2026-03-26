//
// Created by fran on 17/03/2026.
//

#include "EditTilesCornersHeightCommand.hpp"

EditTilesCornersHeightCommand::EditTilesCornersHeightCommand(const std::vector<TileCorner *> &corners, const float heightStep)
    : m_corners(corners)
    , m_heightStep(heightStep)
{
}

void EditTilesCornersHeightCommand::addHeight(const float heightStep, WorldModel &model, const WorldView &view)
{
    m_heightStep += heightStep;
    for (TileCorner * corner: m_corners)
        corner->addHeight(heightStep);
    view.updateTileCorners(model.getTiles(), m_corners);
}

void EditTilesCornersHeightCommand::addNewCorners(const std::vector<TileCorner *> &corners, WorldModel &model,
    const WorldView &view)
{
    for (TileCorner * corner: corners) {
        corner->addHeight(m_heightStep);
        m_corners.push_back(corner);
    }
    view.updateTileCorners(model.getTiles(), corners);
}

void EditTilesCornersHeightCommand::execute(WorldModel &model, WorldView &view)
{
    for (TileCorner * corner: m_corners)
        corner->addHeight(m_heightStep);
    view.updateTileCorners(model.getTiles(), m_corners);
}

void EditTilesCornersHeightCommand::undo(WorldModel &model, WorldView &view)
{
    for (TileCorner * corner: m_corners)
        corner->addHeight(-m_heightStep);
    view.updateTileCorners(model.getTiles(), m_corners);
}

std::string EditTilesCornersHeightCommand::getName()
{ 
    return "Set Tiles Corners Height"; 
}