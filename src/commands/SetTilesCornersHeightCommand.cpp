//
// Created by fran on 23/06/2026.
//

#include "SetTilesCornersHeightCommand.hpp"

SetTilesCornersHeightCommand::SetTilesCornersHeightCommand()
    : m_minWeightThreshold(0.3f)
{
}

void SetTilesCornersHeightCommand::addCorners(const std::vector<BrushTileCornerHit> &brushSelection, float heightValue, WorldModel &model, const WorldView &view)
{
    std::vector<TileCorner *> cornersToUpdate;
    for (const auto &[corner, weight] : brushSelection) {
        if (corner->getHeight() == heightValue)
            continue;
        if (weight <= m_minWeightThreshold)
            continue;
        if (m_previousCornersHeight.find(corner) == m_previousCornersHeight.end())
            m_previousCornersHeight[corner] = corner->getHeight();
        if (m_cornersHeightValues.find(corner) == m_cornersHeightValues.end())
            m_cornersHeightValues[corner] = heightValue;
        corner->setHeight(heightValue);
        cornersToUpdate.push_back(corner);

    }
    model.onTileCornerHeightChanged();
    view.updateTileCorners(model.getTiles(), cornersToUpdate);
}

void SetTilesCornersHeightCommand::addCorners(std::unordered_map<TileCorner *, float> cornersTargetHeightValues, WorldModel &model, const WorldView &view)
{
    std::vector<TileCorner *> cornersToUpdate;
    for (const auto &[corner, targetHeight] : cornersTargetHeightValues) {
        if (corner->getHeight() == targetHeight)
            continue;
        if (m_previousCornersHeight.find(corner) == m_previousCornersHeight.end())
            m_previousCornersHeight[corner] = corner->getHeight();
        if (m_cornersHeightValues.find(corner) == m_cornersHeightValues.end())
            m_cornersHeightValues[corner] = targetHeight;
        corner->setHeight(targetHeight);
        cornersToUpdate.push_back(corner);
    }
    model.onTileCornerHeightChanged();
    view.updateTileCorners(model.getTiles(), cornersToUpdate);
}

void SetTilesCornersHeightCommand::execute(WorldModel &model, WorldView &view)
{
    std::vector<TileCorner *> cornersToUpdate;
    for (const auto &[corner, previousHeight] : m_previousCornersHeight) {
        if (corner->getHeight() == m_cornersHeightValues[corner])
            continue;
        corner->setHeight(m_cornersHeightValues[corner]);
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
