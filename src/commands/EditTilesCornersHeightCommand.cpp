//
// Created by fran on 17/03/2026.
//

#include "EditTilesCornersHeightCommand.hpp"

EditTilesCornersHeightCommand::EditTilesCornersHeightCommand(const std::vector<TileCorner *> &corners, const float heightStep)
{
    for (TileCorner * corner: corners)
        m_cornersHeightStep.insert({corner, heightStep});
}

void EditTilesCornersHeightCommand::addHeight(const std::vector<TileCorner *> &corners, float heightStep, WorldModel &model, const WorldView &view)
{
    std::vector<TileCorner *> cornersToUpdate;
    for (TileCorner * corner: corners) {
        corner->addHeight(heightStep);
        if (m_cornersHeightStep.find(corner) != m_cornersHeightStep.end())
            m_cornersHeightStep[corner] += heightStep;
        else
            m_cornersHeightStep.insert({corner, heightStep});
        cornersToUpdate.push_back(corner);
    }
    view.updateTileCorners(model.getTiles(), cornersToUpdate);
}

void EditTilesCornersHeightCommand::execute(WorldModel &model, WorldView &view)
{
    std::vector<TileCorner *> cornersToUpdate;
    for (const auto &pair : m_cornersHeightStep) {
        pair.first->addHeight(pair.second);
        cornersToUpdate.push_back(pair.first);
    }
    view.updateTileCorners(model.getTiles(), cornersToUpdate);
}

void EditTilesCornersHeightCommand::undo(WorldModel &model, WorldView &view)
{
    std::vector<TileCorner *> cornersToUpdate;
    for (const auto &pair : m_cornersHeightStep) {
        pair.first->addHeight(-pair.second);
        cornersToUpdate.push_back(pair.first);
    }
    view.updateTileCorners(model.getTiles(), cornersToUpdate);
}

std::string EditTilesCornersHeightCommand::getName()
{ 
    return "Set Tiles Corners Height"; 
}