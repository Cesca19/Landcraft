//
// Created by fran on 23/06/2026.
//

#include "SetTilesCornersHeightCommand.hpp"

SetTilesCornersHeightCommand::SetTilesCornersHeightCommand(float heightValue)
    : m_heightValue(heightValue)
    , m_minWeightThreshold(0.3f)
{
}

void SetTilesCornersHeightCommand::addCorners(const std::vector<BrushTileCornerHit> &brushSelection, WorldModel &model, const WorldView &view)
{
    std::vector<TileCorner *> cornersToUpdate;
    for (const auto &[corner, weight] : brushSelection) {
        if (corner->getHeight() == m_heightValue)
            continue;
        if (weight <= m_minWeightThreshold)
            continue;
        if (m_previousCornersHeight.find(corner) == m_previousCornersHeight.end())
            m_previousCornersHeight[corner] = corner->getHeight();
        corner->setHeight(m_heightValue);
        cornersToUpdate.push_back(corner);

    }
    model.onTileCornerHeightChanged();
    view.updateTileCorners(model.getTiles(), cornersToUpdate);
}

void SetTilesCornersHeightCommand::execute(WorldModel &model, WorldView &view)
{
    std::vector<TileCorner *> cornersToUpdate;
    for (const auto &[corner, previousHeight] : m_previousCornersHeight) {
        if (corner->getHeight() == m_heightValue)
            continue;
        corner->setHeight(m_heightValue);
        cornersToUpdate.push_back(corner);
    }
    if (cornersToUpdate.empty()) 
        return;
    model.onTileCornerHeightChanged();
    view.updateTileCorners(model.getTiles(), cornersToUpdate);
}

void SetTilesCornersHeightCommand::undo(WorldModel &model, WorldView &view)
{
    std::vector<TileCorner *> cornersToUpdate;
    for (const auto &[corner, previousHeight] : m_previousCornersHeight) {
        if (corner->getHeight() == previousHeight)
            continue;
        corner->setHeight(previousHeight);
        cornersToUpdate.push_back(corner);
    }
    if (cornersToUpdate.empty()) 
        return;
    model.onTileCornerHeightChanged();
    view.updateTileCorners(model.getTiles(), cornersToUpdate);
}

std::string SetTilesCornersHeightCommand::getName()
{
    return "Set Tiles Corners Height";
}
