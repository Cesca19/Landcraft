//
// Created by fran on 17/03/2026.
//

#ifndef LANDCRAFT_EDITTILESCORNERSHEIGHTCOMMAND_HPP
#define LANDCRAFT_EDITTILESCORNERSHEIGHTCOMMAND_HPP
#include "ICommand.hpp"
#include <unordered_map>

class EditTilesCornersHeightCommand : public ICommand
{
public:
    EditTilesCornersHeightCommand();
    void addCorners(const std::vector<BrushTileCornerHit> &brushSelection, float heightStep, WorldModel &model, const WorldView &view);
    // void addCorners(const std::vector<TileCorner *> &corners, float heightStep, WorldModel &model, const WorldView &view);
    void execute(WorldModel& model, WorldView& view) override;
    void undo(WorldModel& model, WorldView& view) override;
    std::string getName() override;
private:
    std::unordered_map<TileCorner *, float> m_previousCornersHeight;
    std::unordered_map<TileCorner *, float> m_cornersHeightStep;
};


#endif //LANDCRAFT_EDITTILESCORNERSHEIGHTCOMMAND_HPP