//
// Created by fran on 17/03/2026.
//

#ifndef LANDCRAFT_SETTILESCORNERSHEIGHTCOMMAND_HPP
#define LANDCRAFT_SETTILESCORNERSHEIGHTCOMMAND_HPP
#include "ICommand.hpp"

class SetTilesCornersHeightCommand : public ICommand
{
public:
    SetTilesCornersHeightCommand(const std::vector<TileCorner *> corners, const int heightStep);
    void execute(WorldModel& model, WorldView& view) override;
    void undo(WorldModel& model, WorldView& view) override;
    std::string getName() override;
private:
    std::vector<TileCorner *> m_corners;
    int m_heightStep;
};


#endif //LANDCRAFT_SETTILESCORNERSHEIGHTCOMMAND_HPP