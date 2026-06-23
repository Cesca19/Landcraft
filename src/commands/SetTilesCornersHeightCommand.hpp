//
// Created by fran on 23/06/2026.
//

#ifndef LANDCRAFT_SETTILESCORNERSHEIGHTCOMMAND_HPP
#define LANDCRAFT_SETTILESCORNERSHEIGHTCOMMAND_HPP

#include "ICommand.hpp"
#include <unordered_map>

class SetTilesCornersHeightCommand : public ICommand {
public:
    SetTilesCornersHeightCommand();
    void addCorners(const std::vector<BrushTileCornerHit> &brushSelection, float heightValue, WorldModel &model, const WorldView &view);
    void addCorners(std::unordered_map<TileCorner *, float> cornersTargetHeightValues, WorldModel &model, const WorldView &view);
    void execute(WorldModel& model, WorldView& view) override;
    void undo(WorldModel& model, WorldView& view) override;
    std::string getName() override;
private:
    std::unordered_map<TileCorner *, float> m_previousCornersHeight;
    std::unordered_map<TileCorner *, float> m_cornersHeightValues;
    float m_minWeightThreshold;
};


#endif //LANDCRAFT_SETTILESCORNERSHEIGHTCOMMAND_HPP
