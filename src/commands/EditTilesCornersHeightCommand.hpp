//
// Created by fran on 17/03/2026.
//

#ifndef LANDCRAFT_SETTILESCORNERSHEIGHTCOMMAND_HPP
#define LANDCRAFT_SETTILESCORNERSHEIGHTCOMMAND_HPP
#include "ICommand.hpp"
#include <map>

class EditTilesCornersHeightCommand : public ICommand
{
public:
    EditTilesCornersHeightCommand();
    void addCorners(const std::vector<TileCorner *> &corners, float heightStep, WorldModel &model, const WorldView &view);
    void execute(WorldModel& model, WorldView& view) override;
    void undo(WorldModel& model, WorldView& view) override;
    std::string getName() override;
private:
    std::map<TileCorner *, float> m_cornersHeightStep;
};


#endif //LANDCRAFT_SETTILESCORNERSHEIGHTCOMMAND_HPP