//
// Created by fran on 17/03/2026.
//

#include "EditTilesCornersHeightCommand.hpp"

EditTilesCornersHeightCommand::EditTilesCornersHeightCommand()
{
}

void EditTilesCornersHeightCommand::addCorners(const std::vector<BrushTileCornerHit> &brushSelection, const float heightStep, WorldModel &model, const WorldView &view)
{
    std::vector<TileCorner *> cornersToUpdate;
    for (const auto &[corner, weight]: brushSelection) {
        float previousHeight = corner->getHeight();
        corner->addHeight(heightStep * weight);
        model.onTileCornerHeightChanged(corner->getHeight());
        if (m_cornersHeightStep.find(corner) != m_cornersHeightStep.end())
            m_cornersHeightStep[corner] += heightStep * weight;
        else {
            m_cornersHeightStep.insert({corner, heightStep * weight});
            m_previousCornersHeight[corner] = previousHeight;
        }
        cornersToUpdate.push_back(corner);
    }
    view.updateTileCorners(model.getTiles(), cornersToUpdate);
}

void EditTilesCornersHeightCommand::execute(WorldModel &model, WorldView &view)
{
    std::vector<TileCorner *> cornersToUpdate;
    for (const auto &[corner, height] : m_cornersHeightStep) {
        // corner->addHeight(height);
        corner->setHeight(m_previousCornersHeight[corner] + height);
        cornersToUpdate.push_back(corner);
    }
    view.updateTileCorners(model.getTiles(), cornersToUpdate);
    model.onTileCornerHeightChanged();
}

void EditTilesCornersHeightCommand::undo(WorldModel &model, WorldView &view)
{
    std::vector<TileCorner *> cornersToUpdate;
    for (const auto &[corner, height] : m_cornersHeightStep) {
        corner->setHeight(m_previousCornersHeight[corner]);
        cornersToUpdate.push_back(corner);
    }
    view.updateTileCorners(model.getTiles(), cornersToUpdate);
    model.onTileCornerHeightChanged();
}

std::string EditTilesCornersHeightCommand::getName()
{ 
    return "Set Tiles Corners Height"; 
}